#include "Player.h"

#include <array>

#include "Donya/FilePath.h"
#include "Donya/Useful.h"		// Use convert character-code function.

#include "GameLibFunctions.h"	// For load and render model.
#include "skinned_mesh.h"

#if DEBUG_MODE

#include "static_mesh.h"		// For drawing collision.

#endif // DEBUG_MODE

#undef max
#undef min

#define scast static_cast

PlayerParam::PlayerParam() :
	frameUnfoldableDefence( 1 ), frameCancelableAttack( 1 ), frameWholeAttacking( 1 ),
	scale( 1.0f ), runSpeed( 1.0f ), rotSlerpFactor( 0.3f ),
	hitBoxBody(), hitBoxPhysic()
{
	hitBoxBody.size		= 1.0f;
	hitBoxPhysic.radius	= 1.0f;
}
PlayerParam::~PlayerParam() = default;

void PlayerParam::Init()
{
	LoadParameter();
}
void PlayerParam::Uninit()
{
	// No op.
}

void PlayerParam::LoadParameter( bool isBinary )
{
	Donya::Serializer::Extension ext = ( isBinary )
	? Donya::Serializer::Extension::BINARY
	: Donya::Serializer::Extension::JSON;
	std::string filePath = GenerateSerializePath( SERIAL_ID, ext );

	Donya::Serializer seria;
	seria.Load( ext, filePath.c_str(), SERIAL_ID, *this );
}

#if USE_IMGUI

void PlayerParam::SaveParameter()
{
	Donya::Serializer::Extension bin  = Donya::Serializer::Extension::BINARY;
	Donya::Serializer::Extension json = Donya::Serializer::Extension::JSON;
	std::string binPath  = GenerateSerializePath( SERIAL_ID, bin  );
	std::string jsonPath = GenerateSerializePath( SERIAL_ID, json );

	Donya::Serializer seria;
	seria.Save( bin,  binPath.c_str(),  SERIAL_ID, *this );
	seria.Save( json, jsonPath.c_str(), SERIAL_ID, *this );
}

void PlayerParam::UseImGui()
{
	if ( ImGui::BeginIfAllowed() )
	{
		if ( ImGui::TreeNode( "Player.AdjustData" ) )
		{
			ImGui::SliderInt( "Defence.WholeExpandingFrame", &frameUnfoldableDefence, 1, 120 );
			ImGui::SliderInt( "Attack.CancelableFrame", &frameCancelableAttack, 1, frameWholeAttacking );
			ImGui::SliderInt( "Attack.WholeAttackingFrame", &frameWholeAttacking, 1, 300 );
			ImGui::Text( "" );

			ImGui::SliderFloat( "Scale", &scale, 0.0f, 8.0f );
			ImGui::DragFloat( "Running Speed", &runSpeed );
			ImGui::SliderFloat( "SlerpPercent of Rotation", &rotSlerpFactor, 0.05f, 1.0f );

			if ( ImGui::TreeNode( "Collisions" ) )
			{
				auto ShowAABB   = []( const std::string &prefix, Donya::AABB   *pAABB   )
				{
					ImGui::DragFloat3( ( prefix + ".Offset" ).c_str(), &pAABB->pos.x  );
					ImGui::DragFloat3( ( prefix + ".Scale" ).c_str(),  &pAABB->size.x );
					ImGui::Checkbox  ( ( prefix + ".ExistCollision" ).c_str(), &pAABB->exist );
				};
				auto ShowSphere = []( const std::string &prefix, Donya::Sphere *pSphere )
				{
					ImGui::DragFloat3( ( prefix + ".Offset" ).c_str(), &pSphere->pos.x  );
					ImGui::DragFloat ( ( prefix + ".Scale" ).c_str(),  &pSphere->radius );
					ImGui::Checkbox  ( ( prefix + ".ExistCollision" ).c_str(), &pSphere->exist );
				};

				ShowAABB  ( "VS_BossAttacks",	&hitBoxBody   );
				ShowSphere( "VS_StageWall",		&hitBoxPhysic );
				ShowAABB  ( "Shield",			&hitBoxShield );

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

			ImGui::TreePop();
		}

		ImGui::End();
	}
}

#endif // USE_IMGUI

Player::Player() :
	status( State::Idle ),
	timer( 0 ),
	fieldRadius( 0 ),
	pos(), velocity(), lookDirection(),
	orientation(),
	models(),
	isHoldingDefence( false ), wasSucceededDefence( false )
{
	models.pIdle	= std::make_unique<skinned_mesh>();
	models.pAttack	= std::make_unique<skinned_mesh>();
}
Player::~Player() = default;

void Player::Init()
{
	PlayerParam::Get().Init();
	SetFieldRadius( 0.0f ); // Set to body's radius.

	LoadModel();

	lookDirection = Donya::Vector3::Front();
	orientation   = Donya::Quaternion::Identity();

	IdleInit( Input::NoOperation() );
}
void Player::Uninit()
{
	PlayerParam::Get().Uninit();

	models.pIdle.reset( nullptr );
	models.pAttack.reset( nullptr );
}

void Player::Update( Input input )
{
#if USE_IMGUI

	PlayerParam::Get().UseImGui();
	UseImGui();

#endif // USE_IMGUI

	ChangeStatus( input );
	UpdateCurrentStatus( input );

	ApplyVelocity();
	CollideToWall();
}

void Player::Draw( const Donya::Vector4x4 &matView, const Donya::Vector4x4 &matProjection )
{
	const auto &PARAM = PlayerParam::Get();

	Donya::Vector4x4 S = Donya::Vector4x4::MakeScaling( PARAM.Scale() );
	Donya::Vector4x4 R = orientation.RequireRotationMatrix();
	Donya::Vector4x4 T = Donya::Vector4x4::MakeTranslation( pos );
	Donya::Vector4x4 W = S * R * T;
	Donya::Vector4x4 WVP = W * matView * matProjection;

	switch ( status )
	{
	case Player::State::Idle:
		{
			FBXRender( models.pIdle.get(), WVP, W );
		}
		break;
	case Player::State::Run:
		{
			FBXRender( models.pIdle.get(), WVP, W );
		}
		break;
	case Player::State::Defend:
		{
			FBXRender( models.pIdle.get(), WVP, W );
		}
		break;
	case Player::State::Attack:
		{
			FBXRender( models.pAttack.get(), WVP, W );
		}
		break;
	default: break;
	}

	// For debug, helpers of drawing primitive. and drawing collisions.
#if DEBUG_MODE

	auto GenerateCube   = []()->std::shared_ptr<static_mesh>
	{
		std::shared_ptr<static_mesh> tmpCube = std::make_shared<static_mesh>();
		createCube( tmpCube.get() );
		return tmpCube;
	};
	auto GenerateSphere = []()->std::shared_ptr<static_mesh>
	{
		std::shared_ptr<static_mesh> tmpSphere = std::make_shared<static_mesh>();
		createSphere( tmpSphere.get(), 6, 12 );
		return tmpSphere;
	};
	static std::shared_ptr<static_mesh> pCube	= GenerateCube();
	static std::shared_ptr<static_mesh> pSphere	= GenerateSphere();

	auto DrawCube   = [&]( const Donya::Vector3 &cubeOffset, const Donya::Vector3 &cubeScale, const Donya::Vector4 &color )
	{
		Donya::Vector4x4 CS = Donya::Vector4x4::MakeScaling( cubeScale * 2.0f ); // Half size->Whole size.
		Donya::Vector4x4 CT = Donya::Vector4x4::MakeTranslation( cubeOffset );
		Donya::Vector4x4 CW = ( CS * CT ) * W;
		Donya::Vector4x4 CWVP = CW * matView * matProjection;

		OBJRender( pCube.get(), CWVP, CW, color );
	};
	auto DrawSphere = [&]( const Donya::Vector3 &sphereOffset, float sphereScale, const Donya::Vector4 &color )
	{
		Donya::Vector4x4 CS = Donya::Vector4x4::MakeScaling( sphereScale * 2.0f ); // Half size->Whole size.
		Donya::Vector4x4 CT = Donya::Vector4x4::MakeTranslation( sphereOffset );
		Donya::Vector4x4 CW = ( CS * CT ) * W;
		Donya::Vector4x4 CWVP = CW * matView * matProjection;

		OBJRender( pSphere.get(), CWVP, CW, color );
	};

	if ( Donya::IsShowCollision() )
	{
		setBlendMode_ALPHA( 0.5f );

		constexpr Donya::Vector4 BODY_COLOR  { 1.0f, 1.0f, 1.0f, 0.5f };
		constexpr Donya::Vector4 PHYSIC_COLOR{ 0.0f, 0.3f, 0.8f, 0.5f };
		const auto BODY   = PARAM.HitBoxBody();
		const auto PHYSIC = PARAM.HitBoxPhysic();
		if( BODY.exist   ) { DrawCube  ( BODY.pos,   BODY.size,     BODY_COLOR   ); }
		if( PHYSIC.exist ) { DrawSphere( PHYSIC.pos, PHYSIC.radius, PHYSIC_COLOR ); }

		constexpr Donya::Vector4 SHIELD_COLOR{ 0.2f, 0.8f, 0.2f, 0.5f };
		constexpr Donya::Vector4 SHIELD_COLOR_SUCCEEDED{ 0.6f, 1.0f, 0.6f, 0.5f };
		const auto SHIELD = PARAM.HitBoxShield();
		Donya::Vector4 shieldColor = ( wasSucceededDefence ) ? SHIELD_COLOR_SUCCEEDED : SHIELD_COLOR;
		if( status == State::Defend ){ DrawCube( SHIELD.pos, SHIELD.size, shieldColor ); }
	}

#endif // DEBUG_MODE
}

Donya::OBB MakeOBB( const Donya::AABB &AABB, const Donya::Quaternion &orientation )
{
	Donya::OBB OBB{};
	OBB.pos		= AABB.pos;
	OBB.size	= AABB.size;
	OBB.orientation = orientation;
	OBB.exist	= AABB.exist;
	return OBB;
}
Donya::OBB Player::GetHurtBox() const
{
	const auto &HITBOX = PlayerParam::Get().HitBoxBody();

	Donya::OBB wsOBB = MakeOBB( HITBOX, orientation );
	return wsOBB;
}
Donya::OBB Player::GetShieldHitBox() const
{
	const auto &HITBOX = PlayerParam::Get().HitBoxShield();

	Donya::OBB wsOBB = MakeOBB( HITBOX, orientation );
	if ( status != State::Defend ) { wsOBB.exist = false; }
	return wsOBB;
}

void Player::SucceededDefence()
{
	wasSucceededDefence = true;
}

void Player::SetFieldRadius( float newFieldRadius )
{
	const float bodyRadius = PlayerParam::Get().HitBoxPhysic().radius;
	fieldRadius = std::max( newFieldRadius, bodyRadius );
}

void Player::LoadModel()
{
	loadFBX( models.pIdle.get(),	GetModelPath( ModelAttribute::PlayerIdle ) );
	loadFBX( models.pAttack.get(),	GetModelPath( ModelAttribute::PlayerAtk ) );
}

void Player::ChangeStatus( Input input )
{
	if ( !input.doDefend )
	{
		// This flag not want to false if timer lower than 0.
		isHoldingDefence = false;
	}

	// These method is change my status if needs.

	switch ( status )
	{
	case Player::State::Idle:	ChangeStatusFromIdle  ( input );	break;
	case Player::State::Run:	ChangeStatusFromRun   ( input );	break;
	case Player::State::Defend:	ChangeStatusFromDefend( input );	break;
	case Player::State::Attack:	ChangeStatusFromAttack( input );	break;
	default: break;
	}
}
void Player::UpdateCurrentStatus( Input input )
{
	switch ( status )
	{
	case Player::State::Idle:	IdleUpdate  ( input );	break;
	case Player::State::Run:	RunUpdate   ( input );	break;
	case Player::State::Defend:	DefendUpdate( input );	break;
	case Player::State::Attack:	AttackUpdate( input );	break;
	default: break;
	}
}

/*
ChangeStatusXXX : call before update, changing status if necessary.
XXXInit : call by ChangeStatusXXX when changing status. after XXXUninit.
XXXUpdate : call by UpdateCurrentStatus.
XXXUninit : call by ChangeStatusXXX when changing status. before YYYInit.
*/

void Player::ChangeStatusFromIdle( Input input )
{
	if ( input.doDefend && !isHoldingDefence )
	{
		IdleUninit();
		DefendInit( input );
		return;
	}
	// else

	if ( input.doAttack )
	{
		IdleUninit();
		AttackInit( input );
		return;
	}
	// else

	if ( !input.moveVector.IsZero() )
	{
		IdleUninit();
		RunInit( input );
		return;
	}
	// else
}
void Player::IdleInit( Input input )
{
	status   = State::Idle;
	velocity = 0.0f; // Each member set to zero.
}
void Player::IdleUpdate( Input input )
{
	// No op.
}
void Player::IdleUninit()
{
	// No op.
}

void Player::ChangeStatusFromRun( Input input )
{
	if ( input.doDefend && !isHoldingDefence )
	{
		RunUninit();
		DefendInit( input );
		return;
	}
	// else

	if ( input.doAttack )
	{
		RunUninit();
		AttackInit( input );
		return;
	}
	// else

	if ( input.moveVector.IsZero() )
	{
		RunUninit();
		IdleInit( input );
		return;
	}
	// else
}
void Player::RunInit( Input input )
{
	status = State::Run;
}
void Player::RunUpdate( Input input )
{
	AssignInputVelocity( input );
}
void Player::RunUninit()
{
	// No op.
}

void Player::ChangeStatusFromDefend( Input input )
{
	if ( !input.doDefend || timer <= 0 )
	{
		DefendUninit();

		if ( input.moveVector.IsZero() )
		{
			IdleInit( input );
		}
		else
		{
			RunInit( input );
		}
	}
}
void Player::DefendInit( Input input )
{
	status   = State::Defend;
	velocity = 0.0f; // Each member set to zero.

	isHoldingDefence    = true;
	wasSucceededDefence = false;

	const auto &PARAM = PlayerParam::Get();
	timer = PARAM.FrameWholeDefence();
}
void Player::DefendUpdate( Input input )
{
	timer--;

	if ( !input.moveVector.IsZero() )
	{
		lookDirection = input.moveVector.Normalized();
	}
}
void Player::DefendUninit()
{
	timer = 0;
	wasSucceededDefence = false;
}

void Player::ChangeStatusFromAttack( Input input )
{
	const auto &PARAM = PlayerParam::Get();
	const int WHOLE_FRAME = PARAM.FrameWholeAttacking();
	const int CANCELABLE  = PARAM.FrameCancelableAttack();

	if ( timer <= 0 )
	{
		AttackUninit();

		if ( input.moveVector.IsZero() )
		{
			IdleInit( input );
		}
		else
		{
			RunInit( input );
		}

		return;
	}
	// else

	if ( input.doDefend && !isHoldingDefence && ( WHOLE_FRAME - CANCELABLE ) <= timer )
	{
		AttackUninit();
		DefendInit( input );

		return;
	}
	// else
}
void Player::AttackInit( Input input )
{
	status   = State::Attack;
	velocity = 0.0f; // Each member set to zero.

	const auto &PARAM = PlayerParam::Get();
	timer = PARAM.FrameWholeAttacking();
}
void Player::AttackUpdate( Input input )
{
	timer--;

	const auto &PARAM = PlayerParam::Get();
	const int WHOLE_FRAME = PARAM.FrameWholeAttacking();
	const int CANCELABLE  = PARAM.FrameCancelableAttack();
	if ( !input.moveVector.IsZero() && ( WHOLE_FRAME - CANCELABLE ) <= timer )
	{
		lookDirection = input.moveVector;
	}
}
void Player::AttackUninit()
{
	timer = 0;
}

void Player::AssignInputVelocity( Input input )
{
	velocity = input.moveVector;
	velocity *= PlayerParam::Get().RunSpeed();
}

void Player::ApplyVelocity()
{
	if ( !velocity.IsZero() )
	{
		pos += velocity;
		lookDirection = velocity.Normalized();
	}

	if ( lookDirection == orientation.LocalFront() ) { return; }
	// else

	Donya::Quaternion destination = Donya::Quaternion::LookAt( orientation, lookDirection );
	destination.Normalize();

	orientation = Donya::Quaternion::Slerp( orientation, destination, PlayerParam::Get().SlerpFactor() );
	orientation.Normalize();

	if ( orientation.IsSameRotation( destination ) )
	{
		orientation = destination;
		lookDirection = orientation.LocalFront();
	}
}

void Player::CollideToWall()
{
	const float bodyRadius = PlayerParam::Get().HitBoxPhysic().radius;
	const float trueFieldRadius = fieldRadius + bodyRadius;

	constexpr Donya::Vector3 ORIGIN = Donya::Vector3::Zero();
	const Donya::Vector3 currentDistance = pos - ORIGIN;
	const float currentLength = currentDistance.Length();

	if ( trueFieldRadius < currentLength )
	{
		float diff = currentLength - trueFieldRadius;
		const Donya::Vector3 direction = currentDistance.Normalized();
		pos = ORIGIN + ( direction * ( currentLength - diff ) );
	}
}

#if USE_IMGUI

void Player::UseImGui()
{
	if ( ImGui::BeginIfAllowed() )
	{
		if ( ImGui::TreeNode( u8"Player.CurrentParameter" ) )
		{
			auto GetStatusName = []( Player::State status )->std::string
			{
				switch ( status )
				{
				case Player::State::Idle:	return { "Idle" };		break;
				case Player::State::Run:	return { "Run" };		break;
				case Player::State::Defend:	return { "Defend" };	break;
				case Player::State::Attack:	return { "Attack" };	break;
				default: break;
				}

				return { "Unsupported status" };
			};
			std::string statusCaption = "Status : " + GetStatusName( status );
			ImGui::Text( statusCaption.c_str() );
			ImGui::Text( "Timer : %d", timer );
			ImGui::Text( "FieldRadius : %f", fieldRadius );
			ImGui::Text( "HoldDefence : %d", isHoldingDefence ? 1 : 0 );
			ImGui::Text( "" );

			const std::string vec3Info{ "[X:%5.3f][Y:%5.3f][Z:%5.3f]" };
			const std::string vec4Info{ "[X:%5.3f][Y:%5.3f][Z:%5.3f][W:%5.3f]" };
			auto ShowVec3 = [&vec3Info]( std::string name, const Donya::Vector3 &param )
			{
				ImGui::Text( ( name + vec3Info ).c_str(), param.x, param.y, param.z );
			};
			auto ShowVec4 = [&vec4Info]( std::string name, const Donya::Vector4 &param )
			{
				ImGui::Text( ( name + vec4Info ).c_str(), param.x, param.y, param.z, param.w );
			};
			auto ShowQuaternion = [&vec4Info]( std::string name, const Donya::Quaternion &param )
			{
				ImGui::Text( ( name + vec4Info ).c_str(), param.x, param.y, param.z, param.w );
			};

			ShowVec3( "Position", pos );
			ShowVec3( "Velocity", velocity );
			ShowVec3( "LookDirection", lookDirection );

			ShowQuaternion( "Orientation", orientation );
			if ( ImGui::Button( u8"Reset : Orientation" ) )
			{
				orientation = Donya::Quaternion::Identity();
			}

			ImGui::TreePop();
		}

		ImGui::End();
	}
}

#endif // USE_IMGUI
