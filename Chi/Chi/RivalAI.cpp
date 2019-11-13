#include "RivalAI.h"

#include <numeric> // Use accumulate().

#include "Donya/FilePath.h"
#include "Donya/Serializer.h"
#include "Donya/Random.h"

#undef max
#undef min

#define scast static_cast

void RivalAI::Init()
{
	status		= ActionState::WAIT;
	timer		= 0;
	coolTime	= 0;

	wholeFrame.fill( 1 );
	coolTimeFrame.fill( 1 );
	waitPercents.fill( 1 );
	attackPercents.fill( 1 );

	LoadParameter();

	if ( gapIntervals.empty() )
	{
		gapIntervals.resize( 1U );
	}
}

void RivalAI::Update()
{
#if USE_IMGUI

	ImGui();

#endif // USE_IMGUI

	if ( stopUpdate ) { return; }
	// else

	timer--;
	if ( timer <= 0 )
	{
		timer = 0;

		LotteryState();
	}
}

void RivalAI::OverwriteState( ActionState newState )
{
	status = newState;
}
void RivalAI::FinishCurrentState()
{
	timer = 0;
	LotteryState();
}

RivalAI::ActionState RivalAI::ToActionState( WaitState waitStatus ) const
{
	ActionState to = ActionState::END;

	if ( waitStatus == WaitState::END ) { return to; }
	// else

	switch ( waitStatus )
	{
	case WaitState::WAIT:	to = ActionState::WAIT;	break;
	case WaitState::MOVE:	to = ActionState::MOVE;	break;
	default: break;
	}

	return to;
}
RivalAI::ActionState RivalAI::ToActionState( AttackState attackStatus ) const
{
	ActionState to = ActionState::END;

	if ( attackStatus == AttackState::END ) { return to; }
	// else

	switch ( attackStatus )
	{
	case AttackState::BARRAGE:	to = ActionState::ATTACK_BARRAGE;	break;
	case AttackState::LINE:		to = ActionState::ATTACK_LINE;		break;
	case AttackState::RAID:		to = ActionState::ATTACK_RAID;		break;
	default: break;
	}

	return to;
}

void RivalAI::LotteryState()
{
	// Repeat wait-status <-> attack-status.

	if ( IsAction( status ) )
	{
		LotteryWaitState();
	}
	else
	{
		LotteryAttackState();
	}
}
void RivalAI::LotteryWaitState()
{
	timer = coolTime;

	const int percentSum = std::accumulate( waitPercents.begin(), waitPercents.end(), 0 );

	ActionState oldState = status;
	while ( oldState == status )
	{
		const int percentRand = Donya::Random::GenerateInt( percentSum );

		for ( int i = 0; i < WAIT_STATE_COUNT; ++i )
		{
			int partSum = ( i == WAIT_STATE_COUNT - 1 )
			? percentSum
			: std::accumulate( waitPercents.begin(), ( waitPercents.begin() + i + 1 ), 0 );

			if ( percentRand <= partSum )
			{
				status = ToActionState( scast<WaitState>( i ) );
				break;
			}
		}
	}
}
void RivalAI::LotteryAttackState()
{
	if ( attackTimes <= 0 )
	{
		status		= GetGapAttack();
		timer		= wholeFrame.back();
		coolTime	= coolTimeFrame.back();

		intervalIndex	= ( intervalIndex <= scast<int>( gapIntervals.size() ) - 1 ) ? 0 : intervalIndex + 1;
		attackTimes		= gapIntervals[intervalIndex];
		return;
	}
	// else

	attackTimes--;

	const int percentSum = std::accumulate( attackPercents.begin(), attackPercents.end(), 0 );

	ActionState oldState = status;
	while ( oldState == status )
	{
		const int percentRand = Donya::Random::GenerateInt( percentSum );

		for ( int i = 0; i < ATTACK_STATE_COUNT; ++i )
		{
			int partSum = ( i == ATTACK_STATE_COUNT - 1 )
			? percentSum
			: std::accumulate( attackPercents.begin(), ( attackPercents.begin() + i + 1 ), 0 );

			if ( percentRand <= partSum )
			{
				status = ToActionState( scast<AttackState>( i ) );
				timer = wholeFrame[i];
				coolTime = coolTimeFrame[i];
				break;
			}
		}
	}
}

RivalAI::ActionState RivalAI::GetGapAttack() const
{
	return ActionState::ATTACK_RUSH;
}

void RivalAI::LoadParameter( bool isBinary )
{
	Donya::Serializer::Extension ext = ( isBinary )
	? Donya::Serializer::Extension::BINARY
	: Donya::Serializer::Extension::JSON;
	std::string filePath = GenerateSerializePath( SERIAL_ID, ext );

	Donya::Serializer seria;
	seria.Load( ext, filePath.c_str(), SERIAL_ID, *this );
}

#if USE_IMGUI

void RivalAI::SaveParameter()
{
	Donya::Serializer::Extension bin  = Donya::Serializer::Extension::BINARY;
	Donya::Serializer::Extension json = Donya::Serializer::Extension::JSON;
	std::string binPath  = GenerateSerializePath( SERIAL_ID, bin  );
	std::string jsonPath = GenerateSerializePath( SERIAL_ID, json );

	Donya::Serializer seria;
	seria.Save( bin,  binPath.c_str(),  SERIAL_ID, *this );
	seria.Save( json, jsonPath.c_str(), SERIAL_ID, *this );
}

void RivalAI::ImGui()
{
	if ( ImGui::BeginIfAllowed( "RivalAI" ) )
	{
		auto GetActionName	= []( int i )->std::string
		{
			constexpr std::array<const char *, ACTION_STATE_COUNT> NAMES
			{
				"Wait",
				"Move",
				"Attack.Barrage",
				"Attack.Line",
				"Attack.Raid",
				"Attack.Rush",
			};

			if ( ACTION_STATE_COUNT <= i ) { return "Error Name"; }
			// else
			return std::string{ NAMES[i] };
		};
		auto GetWaitName	= []( int i )->std::string
		{
			constexpr std::array<const char *, WAIT_STATE_COUNT> NAMES
			{
				"Wait",
				"Move",
			};

			if ( WAIT_STATE_COUNT <= i ) { return "Error Name"; }
			// else
			return std::string{ NAMES[i] };
		};
		auto GetAttackName	= []( int i )->std::string
		{
			constexpr std::array<const char *, ALL_ATTACK_COUNT> NAMES
			{
				"Attack.Barrage",
				"Attack.Line",
				"Attack.Raid",
				/* Gap */ "Attack.Rush",
			};

			if ( ALL_ATTACK_COUNT <= i ) { return "Error Name"; }
			// else
			return std::string{ NAMES[i] };
		};

		if ( ImGui::TreeNode( "CurrentParameter" ) )
		{
			ImGui::Text( "Status : %d", scast<int>( status ) );
			ImGui::Text( "Timer : %d", timer );
			ImGui::Text( "CoolTime : %d", coolTime );

			// Show whole frame.
			{
				const std::string caption{ ".WholeFrame : %d" };
				int i = 0;
				for ( const auto &it : wholeFrame )
				{
					ImGui::Text( ( GetAttackName( i ) + caption ).c_str(), it );
					i++;
				}
			}

			// Show cool-time.
			{
				const std::string caption{ ".CoolTime : %d" };
				int i = 0;
				for ( const auto &it : coolTimeFrame )
				{
					ImGui::Text( ( GetAttackName( i ) + caption ).c_str(), it );
					i++;
				}
			}

			ImGui::TreePop();
		}

		if ( ImGui::TreeNode( "AdjustData" ) )
		{
			if ( ImGui::TreeNode( "Wait.Percents" ) )
			{
				int i = 0;
				for ( auto &it : waitPercents )
				{
					ImGui::SliderInt( GetWaitName( i ).c_str(), &it, 1, 1024 );
					i++;
				}

				ImGui::TreePop();
			}
			if ( ImGui::TreeNode( "Attack.Percents" ) )
			{
				int i = 0;
				for ( auto &it : attackPercents )
				{
					ImGui::SliderInt( GetAttackName( i ).c_str(), &it, 1, 1024 );
					i++;
				}

				ImGui::TreePop();
			}

			if ( ImGui::TreeNode( "Gap.Intervals" ) )
			{
				if ( ImGui::Button( "List.Append" ) )
				{
					gapIntervals.push_back( 0 );
				}
				if ( 1 < gapIntervals.size() && ImGui::Button( "List.PopBack" ) )
				{
					gapIntervals.pop_back();
				}

				const size_t COUNT = gapIntervals.size();
				for ( size_t i = 0; i < COUNT; ++i )
				{
					ImGui::SliderInt( ( "AttackTimes[" + std::to_string( i ) + "]" ).c_str(), &gapIntervals[i], 0, 16 );
				}

				ImGui::TreePop();
			}

			if ( ImGui::TreeNode( "MovingFrame" ) )
			{
				int i = 0;
				for ( auto &it : wholeFrame )
				{
					ImGui::SliderInt( GetAttackName( i ).c_str(), &it, 1, 1024 );
					i++;
				}

				ImGui::TreePop();
			}

			if ( ImGui::TreeNode( "CoolTimeFrame" ) )
			{
				int i = 0;
				for ( auto &it : coolTimeFrame )
				{
					ImGui::SliderInt( GetAttackName( i ).c_str(), &it, 0, 1024 );
					i++;
				}

				ImGui::TreePop();
			}

			ImGui::TreePop();
		}

		if ( ImGui::TreeNode( "File.I/O" ) )
		{
			static bool isBinary = true;
			if ( ImGui::RadioButton( "Binary", isBinary ) ) { isBinary = true; }
			if ( ImGui::RadioButton( "JSON", !isBinary ) ) { isBinary = false; }
			std::string loadStr{ "Load " };
			loadStr += ( isBinary ) ? "Binary" : "JSON";

			if ( ImGui::Button( "Save" ) )
			{
				SaveParameter();
			}
			// if ( ImGui::Button( Donya::MultiToUTF8( loadStr ).c_str() ) )
			if ( ImGui::Button( loadStr.c_str() ) )
			{
				LoadParameter( isBinary );
			}

			ImGui::TreePop();
		}

		ImGui::End();
	}
}

#endif // USE_IMGUI