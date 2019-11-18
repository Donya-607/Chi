#include "GolemAI.h"

#include <numeric> // Use accumulate().

#include "Donya/FilePath.h"
#include "Donya/Serializer.h"
#include "Donya/Random.h"

#undef max
#undef min

#define scast static_cast

void GolemAI::Init()
{
	status		= ActionState::WAIT;
	timer		= 0;
	coolTime	= 0;

	wholeFrame.fill( 1 );
	coolTimeFrame.fill( 1 );
	
	LoadParameter();

	for ( auto &it : pAttackChoosers )
	{
		// Set temporary instance(prevent nullptr. The LoadParameter() requires the pointer(it) is not null).
		if ( !it ) { ResetLotteryKind( &it, ChooserKind::Fixed ); }

		it->LoadParameter();
	}

	timer	= initCoolTime;
	storage	= initStorage;
	status	= ToActionState( scast<WaitState>( initStorage.waitNo ) );

	if ( gapIntervals.empty() ) { gapIntervals.resize( 1U ); }
	attackTimes = gapIntervals.front();
}

void GolemAI::Update( float normalizedTargetDistance )
{
#if USE_IMGUI

	ImGui( normalizedTargetDistance );

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
			AssignAttackState( normalizedTargetDistance );
		}
	}
}

void GolemAI::OverwriteState( ActionState newState )
{
	status = newState;
}

GolemAI::ActionState GolemAI::ToActionState( WaitState waitStatus ) const
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
GolemAI::ActionState GolemAI::ToActionState( AttackState attackStatus ) const
{
	ActionState to = ActionState::END;

	if ( attackStatus == AttackState::END ) { return to; }
	// else

	switch ( attackStatus )
	{
	case AttackState::FAST:		to = ActionState::ATTACK_FAST;		break;
	case AttackState::ROTATE:	to = ActionState::ATTACK_ROTATE;	break;
	default: break;
	}

	return to;
}
GolemAI::AttackState GolemAI::ToAttackState( ActionState status ) const
{
	AttackState to = AttackState::END;

	if ( status == ActionState::END ) { return to; }
	// else

	switch ( status )
	{
	case ActionState::ATTACK_FAST:		to = AttackState::FAST;		break;
	case ActionState::ATTACK_ROTATE:	to = AttackState::ROTATE;	break;
	default: break;
	}

	return to;
}

void GolemAI::AssignWaitState()
{
	timer  = coolTime;
	status = ToActionState( scast<WaitState>( storage.waitNo ) );
}
void GolemAI::AssignAttackState( float targetDistance )
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
GolemAI::ActionState GolemAI::LotteryAttack()
{
	const int lottery = Donya::Random::GenerateInt( ATTACK_STATE_COUNT );
	return ToActionState( scast<AttackState>( lottery ) );
}

GolemAI::ActionState GolemAI::GetGapAttack() const
{
	return ActionState::ATTACK_SWING;
}

void GolemAI::LoadParameter( bool isBinary )
{
	Donya::Serializer::Extension ext = ( isBinary )
	? Donya::Serializer::Extension::BINARY
	: Donya::Serializer::Extension::JSON;
	std::string filePath = GenerateSerializePath( SERIAL_ID, ext );

	Donya::Serializer seria;
	seria.Load( ext, filePath.c_str(), SERIAL_ID, *this );
}

#if USE_IMGUI

void GolemAI::SaveParameter()
{
	Donya::Serializer::Extension bin  = Donya::Serializer::Extension::BINARY;
	Donya::Serializer::Extension json = Donya::Serializer::Extension::JSON;
	std::string binPath  = GenerateSerializePath( SERIAL_ID, bin  );
	std::string jsonPath = GenerateSerializePath( SERIAL_ID, json );

	Donya::Serializer seria;
	seria.Save( bin,  binPath.c_str(),  SERIAL_ID, *this ); 
	seria.Save( json, jsonPath.c_str(), SERIAL_ID, *this );
}

void GolemAI::ImGui( float normalizedTargetDistance )
{
	if ( ImGui::BeginIfAllowed( "GolemAI" ) )
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
				"Attack.Swing",
				"Attack.Fast",
				"Attack.Rotate",
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
				"Attack.Fast",
				"Attack.Rotate",
				"Attack.Swing",
			};

			if ( ALL_ATTACK_COUNT <= i ) { return "Error Name"; }
			// else
			return std::string{ NAMES[i] };
		};

		if ( ImGui::TreeNode( "CurrentParameter" ) )
		{
			ImGui::Text( "Status : %s", GetActionName( scast<int>( status ) ).c_str() );
			ImGui::Text( "Next.WaitNo : %d", storage.waitNo );
			ImGui::Text( "Next.AttackNo : %d", storage.nextAttackNo );
			ImGui::Text( "Next.AttackKind : %s", GetAttackName( storage.nextAttackNo ) );
			ImGui::Text( "AttackTimes : %d", attackTimes );
			ImGui::Text( "Timer : %d", timer );
			ImGui::Text( "CoolTime : %d", coolTime );
			ImGui::Text( "Distance.Target : %f", normalizedTargetDistance );

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

		if ( ImGui::TreeNode( "AttackNumberList" ) )
		{
			std::string atkName{};
			for ( int i = 0; i < ALL_ATTACK_COUNT; ++i )
			{
				atkName = GetAttackName( i );

				ImGui::Text( "[%d]:%s", i, atkName.c_str() );
			}

			ImGui::Text( "[%d ~ ]:Error", ALL_ATTACK_COUNT );

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
				std::string atkName{};
				for ( int i = 0; i < ALL_ATTACK_COUNT; ++i )
				{
					atkName = GetAttackName( i );
					ShowImGuiChooserNode( atkName, &pAttackChoosers[i], &chooserKinds[i] );
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
