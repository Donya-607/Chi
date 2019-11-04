#include "BossAI.h"

#include <numeric> // Use accumulate().

#include "Donya/FilePath.h"
#include "Donya/Serializer.h"
#include "Donya/Random.h"

#define scast static_cast

void BossAI::Init()
{
	status		= ActionState::WAIT;
	timer		= 0;
	coolTime	= 0;

	wholeFrame.fill( 1 );
	coolTimeFrame.fill( 1 );
	waitPercents.fill( 1 );
	attackPercents.fill( 1 );

	LoadParameter();
}

void BossAI::Update()
{
#if USE_IMGUI

	ImGui();

#endif // USE_IMGUI

	timer--;
	if ( timer <= 0 )
	{
		timer = 0;

		// Repeat wait-status <-> attack-status.

		if ( NowStatusAction() )
		{
			LotteryWaitState();
		}
		else
		{
			LotteryAttackState();
		}
	}
}

BossAI::ActionState BossAI::ToActionState( WaitState waitStatus ) const
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
BossAI::ActionState BossAI::ToActionState( AttackState attackStatus ) const
{
	ActionState to = ActionState::END;

	if ( attackStatus == AttackState::END ) { return to; }
	// else

	switch ( attackStatus )
	{
	case AttackState::SWING:	to = ActionState::ATTACK_SWING;		break;
	case AttackState::FAST:		to = ActionState::ATTACK_FAST;		break;
	case AttackState::ROTATE:	to = ActionState::ATTACK_ROTATE;	break;
	default: break;
	}

	return to;
}

void BossAI::LotteryWaitState()
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
void BossAI::LotteryAttackState()
{
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
				status		= ToActionState( scast<AttackState>( i ) );
				timer		= wholeFrame[i];
				coolTime	= coolTimeFrame[i];
				break;
			}
		}
	}
}

bool BossAI::NowStatusAction() const
{
	switch ( status )
	{
	case ActionState::ATTACK_SWING:		return true;	break;
	case ActionState::ATTACK_FAST:		return true;	break;
	case ActionState::ATTACK_ROTATE:	return true;	break;
	default: break;
	}

	return false;
}

void BossAI::LoadParameter( bool isBinary )
{
	Donya::Serializer::Extension ext = ( isBinary )
	? Donya::Serializer::Extension::BINARY
	: Donya::Serializer::Extension::JSON;
	std::string filePath = GenerateSerializePath( SERIAL_ID, ext );

	Donya::Serializer seria;
	seria.Load( ext, filePath.c_str(), SERIAL_ID, *this );
}

#if USE_IMGUI

void BossAI::SaveParameter()
{
	Donya::Serializer::Extension bin  = Donya::Serializer::Extension::BINARY;
	Donya::Serializer::Extension json = Donya::Serializer::Extension::JSON;
	std::string binPath  = GenerateSerializePath( SERIAL_ID, bin  );
	std::string jsonPath = GenerateSerializePath( SERIAL_ID, json );

	Donya::Serializer seria;
	seria.Save( bin,  binPath.c_str(),  SERIAL_ID, *this ); 
	seria.Save( json, jsonPath.c_str(), SERIAL_ID, *this );
}

void BossAI::ImGui()
{
	if ( ImGui::BeginIfAllowed( "BossAI" ) )
	{
		auto GetActionName = []( int i )->std::string
		{
			constexpr std::array<const char *, ACTION_STATE_COUNT> NAMES
			{
				"Wait",
				"Move",
				"Attack.Swing",
				"Attack.Fast",
				"Attack.Rotate",
			};

			if ( ACTION_STATE_COUNT <= i ) { return "Error Name"; }
			// else
			return std::string{ NAMES[i] };
		};
		auto GetWaitName = []( int i )->std::string
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
		auto GetAttackName = []( int i )->std::string
		{
			constexpr std::array<const char *, ATTACK_STATE_COUNT> NAMES
			{
				"Attack.Swing",
				"Attack.Fast",
				"Attack.Rotate",
			};

			if ( ATTACK_STATE_COUNT <= i ) { return "Error Name"; }
			// else
			return std::string{ NAMES[i] };
		};

		if ( ImGui::TreeNode( "CurrentParameter" ) )
		{
			ImGui::Text( "Status : %d", scast<int>( status ) );
			ImGui::Text( "Timer : %d", timer );

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
