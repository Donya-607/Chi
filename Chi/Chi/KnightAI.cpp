#include "KnightAI.h"

#include <numeric>	// Use accumulate().

#include "Donya/FilePath.h"
#include "Donya/Serializer.h"
#include "Donya/Random.h"

#undef max
#undef min

#define scast static_cast

void KnightAI::Init()
{
	status		= ActionState::WAIT;
	timer		= 0;
	coolTime	= 0;

	wholeFrame.fill( 1 );
	coolTimeFrame.fill( 1 );

	LoadParameter();

	for ( auto &it : pAttackChoosers )
	{
		// Set temporary instance(prevent nullptr).
		if ( !it ) { ResetLotteryKind( &it, ChooserKind::Fixed ); }

		it->LoadParameter();
	}

	timer	= initCoolTime;
	status	= ToActionState( scast<WaitState>( initStorage.waitNo ) );

	if ( gapIntervals.empty() ) { gapIntervals.resize( 1U ); }
	attackTimes = gapIntervals.front();
}

void KnightAI::Update( float targetDistance )
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

		// Repeat wait-status <-> attack-status.

		if ( IsAction( status ) )
		{
			AssignWaitState();
		}
		else
		{
			AssignAttackState( targetDistance );
		}
	}
}

void KnightAI::OverwriteState( ActionState newState )
{
	status = newState;
}

KnightAI::ActionState KnightAI::ToActionState( WaitState waitStatus ) const
{
	ActionState to = ActionState::END;

	if ( waitStatus == WaitState::END ) { return to; }
	// else

	switch ( waitStatus )
	{
	case WaitState::WAIT:			to = ActionState::WAIT;				break;
	case WaitState::MOVE_GET_NEAR:	to = ActionState::MOVE_GET_NEAR;	break;
	case WaitState::MOVE_GET_FAR:	to = ActionState::MOVE_GET_FAR;		break;
	case WaitState::MOVE_SIDE:		to = ActionState::MOVE_SIDE;		break;
	case WaitState::MOVE_AIM_SIDE:	to = ActionState::MOVE_AIM_SIDE;	break;
	default: break;
	}

	return to;
}
KnightAI::ActionState KnightAI::ToActionState( AttackState attackStatus ) const
{
	ActionState to = ActionState::END;

	if ( attackStatus == AttackState::END ) { return to; }
	// else

	switch ( attackStatus )
	{
	case AttackState::SWING:	to = ActionState::ATTACK_SWING;		break;
	case AttackState::RAID:		to = ActionState::ATTACK_RAID;		break;
	default: break;
	}

	return to;
}
KnightAI::AttackState KnightAI::ToAttackState( ActionState status ) const
{
	AttackState to = AttackState::END;

	if ( status == ActionState::END ) { return to; }
	// else

	switch ( status )
	{
	case ActionState::ATTACK_SWING:		to = AttackState::SWING;	break;
	case ActionState::ATTACK_RAID:		to = AttackState::RAID;		break;
	default: break;
	}

	return to;
}

void KnightAI::AssignWaitState()
{
	timer  = coolTime;
	status = ToActionState( scast<WaitState>( storage.waitNo ) );
}
void KnightAI::AssignAttackState( float targetDistance )
{
	if ( attackTimes <= 0 )
	{
		status			= GetGapAttack();
		timer			= wholeFrame.back();
		coolTime		= coolTimeFrame.back();

		intervalIndex	= ( intervalIndex <= scast<int>( gapIntervals.size() ) - 1 ) ? 0 : intervalIndex + 1;
		attackTimes		= gapIntervals[intervalIndex];

		storage			= pAttackChoosers.back()->Lottery( targetDistance );

		return;
	}
	// else

	attackTimes--;

	int attackNo = storage.nextAttackNo;
	if ( attackNo < 0 ) { attackNo = scast<int>( ToAttackState( LotteryAttack() ) ); }

	status		= ToActionState( scast<AttackState>( attackNo ) );
	timer		= wholeFrame[attackNo];
	coolTime	= coolTimeFrame[attackNo];
	storage		= pAttackChoosers[attackNo]->Lottery( targetDistance );
}
KnightAI::ActionState KnightAI::LotteryAttack()
{
	const int lottery = Donya::Random::GenerateInt( ATTACK_STATE_COUNT );
	return ToActionState( scast<AttackState>( lottery ) );
}

KnightAI::ActionState KnightAI::GetGapAttack() const
{
	return ActionState::ATTACK_EXPLOSION;
}

void KnightAI::LoadParameter( bool isBinary )
{
	Donya::Serializer::Extension ext = ( isBinary )
	? Donya::Serializer::Extension::BINARY
	: Donya::Serializer::Extension::JSON;
	std::string filePath = GenerateSerializePath( SERIAL_ID, ext );

	Donya::Serializer seria;
	seria.Load( ext, filePath.c_str(), SERIAL_ID, *this );
}

#if USE_IMGUI

void KnightAI::SaveParameter()
{
	Donya::Serializer::Extension bin  = Donya::Serializer::Extension::BINARY;
	Donya::Serializer::Extension json = Donya::Serializer::Extension::JSON;
	std::string binPath  = GenerateSerializePath( SERIAL_ID, bin  );
	std::string jsonPath = GenerateSerializePath( SERIAL_ID, json );

	Donya::Serializer seria;
	seria.Save( bin,  binPath.c_str(),  SERIAL_ID, *this );
	seria.Save( json, jsonPath.c_str(), SERIAL_ID, *this );
}

void KnightAI::ImGui()
{
	if ( ImGui::BeginIfAllowed( "KnightAI" ) )
	{
		auto GetActionName	= []( int i )->std::string
		{
			constexpr std::array<const char *, ACTION_STATE_COUNT> NAMES
			{
				"Wait",
				"Move.Near",
				"Move.Far",
				"Move.Side",
				"Move.AimSide",
				"Attack.Explosion",
				"Attack.Swing",
				"Attack.Raid",
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
				"Move.Near",
				"Move.Far",
				"Move.Side",
				"Move.AimSide",
			};

			if ( WAIT_STATE_COUNT <= i ) { return "Error Name"; }
			// else
			return std::string{ NAMES[i] };
		};
		auto GetAttackName	= []( int i )->std::string
		{
			constexpr std::array<const char *, ALL_ATTACK_COUNT> NAMES
			{
				"Attack.Swing",
				"Attack.Raid",
				"Attack.Explosion",
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
			if ( ImGui::TreeNode( "Initialize" ) )
			{
				ImGui::DragInt( "CoolTime", &initCoolTime );
				initStorage.ShowImGuiNode( "FirstChoice" );

				ImGui::TreePop();
			}

			if ( ImGui::TreeNode( "Attack.Chooser" ) )
			{
				static std::array<int, ALL_ATTACK_COUNT> chooserKinds{};
				for ( int i = 0; i < ALL_ATTACK_COUNT; ++i )
				{
					ShowImGuiChooserNode( GetAttackName( i ), &pAttackChoosers[i], &chooserKinds[i] );
					ImGui::Text( "" );
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
