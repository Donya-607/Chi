#include "Player.h"

#include <array>

#include "Donya/FilePath.h"
#include "Donya/Useful.h"		// Use convert character-code function.

#include "GameLibFunctions.h"	// For load and render model.
#include "skinned_mesh.h"

#if DEBUG_MODE

#include "static_mesh.h"		// For drawing collision.

#endif // DEBUG_MODE

#define scast static_cast

PlayerParam::PlayerParam() :
	scale( 1.0f ), runSpeed( 1.0f ), rotSlerpFactor( 0.3f )
{}
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
			ImGui::SliderFloat( "Scale", &scale, 0.0f, 8.0f );
			ImGui::DragFloat( "Running Speed", &runSpeed );
			ImGui::SliderFloat( "SlerpPercent of Rotation", &rotSlerpFactor, 0.05f, 1.0f );

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
	pos(), velocity(), lookDirection(),
	orientation(),
	// model()
	pModel( std::make_unique<skinned_mesh>() )
{}
Player::~Player() = default;

void Player::Init()
{
	PlayerParam::Get().Init();

	LoadModel();

	lookDirection = Donya::Vector3::Front();
	orientation   = Donya::Quaternion::Identity();

	IdleInit( Input::NoOperation() );
}
void Player::Uninit()
{
	PlayerParam::Get().Uninit();

	pModel.reset( nullptr );
}

void Player::Update( Input input )
{
#if USE_IMGUI

	PlayerParam::Get().UseImGui();
	UseImGui();

#endif // USE_IMGUI

	ChangeStatus( input );
	UpdateCurrentStatus( input );

	RunUpdate( input );

	ApplyVelocity();
}

void Player::Draw( const Donya::Vector4x4 &matView, const Donya::Vector4x4 &matProjection )
{
	const auto &PARAM = PlayerParam::Get();

	Donya::Vector4x4 S = Donya::Vector4x4::MakeScaling( PARAM.Scale() );
	Donya::Vector4x4 R = orientation.RequireRotationMatrix();
	Donya::Vector4x4 T = Donya::Vector4x4::MakeTranslation( pos );
	Donya::Vector4x4 W = S * R * T;
	Donya::Vector4x4 WVP = W * matView * matProjection;

	FBXRender( pModel.get(), WVP, W );

#if DEBUG_MODE

	if ( Donya::IsShowCollision() )
	{
		auto GenerateCube = []()->std::shared_ptr<static_mesh>
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


	}

#endif // DEBUG_MODE
}

void Player::LoadModel()
{
	loadFBX( pModel.get(), GetModelPath( ModelAttribute::Player ) );
}

void Player::ChangeStatus( Input input )
{
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

void Player::ChangeStatusFromIdle( Input input )
{
	if ( !input.moveVector.IsZero() )
	{
		IdleUninit();
		RunInit( input );
	}

	if ( input.doDefend )
	{
		IdleUninit();
		DefendInit( input );
	}

	if ( input.doAttack )
	{
		IdleUninit();
		AttackInit( input );
	}
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
	if ( input.moveVector.IsZero() )
	{
		RunUninit();
		IdleInit( input );
	}

	if ( input.doDefend )
	{
		RunUninit();
		DefendInit( input );
	}

	if ( input.doAttack )
	{
		RunUninit();
		AttackInit( input );
	}
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
#if DEBUG_MODE

	if ( !input.doDefend )
	{
		DefendUninit();
		IdleInit( input );
	}

#endif // DEBUG_MODE
}
void Player::DefendInit( Input input )
{
	status = State::Defend;

#if DEBUG_MODE
	orientation.RotateBy( Donya::Quaternion::Make( orientation.LocalUp(), ToRadian( 180.0f ) ) );
#endif // DEBUG_MODE
}
void Player::DefendUpdate( Input input )
{
	AssignInputVelocity( input );
}
void Player::DefendUninit()
{
#if DEBUG_MODE
	orientation.RotateBy( Donya::Quaternion::Make( -orientation.LocalUp(), ToRadian( 180.0f ) ) );
#endif // DEBUG_MODE
}

void Player::ChangeStatusFromAttack( Input input )
{
#if DEBUG_MODE

	if ( !input.doAttack )
	{
		AttackUninit();
		IdleInit( input );
	}

#endif // DEBUG_MODE
}
void Player::AttackInit( Input input )
{
	status = State::Attack;

#if DEBUG_MODE
	orientation.RotateBy( Donya::Quaternion::Make( orientation.LocalRight(), ToRadian( 180.0f ) ) );
#endif // DEBUG_MODE
}
void Player::AttackUpdate( Input input )
{
	AssignInputVelocity( input );
}
void Player::AttackUninit()
{
#if DEBUG_MODE
	orientation.RotateBy( Donya::Quaternion::Make( -orientation.LocalRight(), ToRadian( 180.0f ) ) );
#endif // DEBUG_MODE
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

