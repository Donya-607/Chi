#include "Player.h"

#include <array>

#include "Donya/Keyboard.h"
#include "Donya/Easing.h"
#include "Donya/FilePath.h"
#include "Donya/Sound.h"
#include "Donya/Useful.h"		// Use convert character-code function.
#include "Donya/UseImGui.h"

#include "GameLibFunctions.h"	// For load and render model.
#include "skinned_mesh.h"

#if DEBUG_MODE

#include "static_mesh.h"		// For drawing collision.

#endif // DEBUG_MODE

#undef max
#undef min

#define scast static_cast

PlayerParam::PlayerParam() :
	frameUnfoldableDefence( 1 ), shieldsRecastFrame( 1 ), frameCancelableAttack( 1 ), frameWholeAttacking( 1 ),
	advanceStartFrame(), advanceFinFrame(), returnFinFrame(),
	advanceEaseKind(), advanceEaseType(), returnEaseKind(), returnEaseType(),
	scale( 1.0f ), runSpeed( 1.0f ), rotSlerpFactor( 0.3f ),
	hitBoxBody(), hitBoxPhysic(), hitBoxShield(), hitBoxLance(),
	lanceMeshName()
{
	hitBoxBody.size			= 1.0f;
	hitBoxPhysic.radius		= 1.0f;
	hitBoxShield.size		= 1.0f;
	hitBoxLance.OBB.size	= 1.0f;

	lanceMeshName.reserve( 512U );
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
			ImGui::SliderInt( "Defence.WholeExpandingFrame",	&frameUnfoldableDefence,	1, 120 );
			ImGui::SliderInt( "Defence.RecastFrame",			&shieldsRecastFrame,		0, 360 );
			ImGui::SliderInt( "Attack.StopAnime.Timing(Frame)",	&frameStopTiming,			1, 360 );
			ImGui::SliderInt( "Attack.StopAnime.Lenth(Frame)",	&frameStopAnime,			1, 360 );
			ImGui::SliderInt( "Attack.CancelableFrame",			&frameCancelableAttack,		1, frameWholeAttacking );
			ImGui::SliderInt( "Attack.WholeAttackingFrame",		&frameWholeAttacking,		1, 300 );
			ImGui::Text( "" );

			ImGui::SliderFloat( "Scale", &scale, 0.0f, 8.0f );
			ImGui::DragFloat( "Running Speed", &runSpeed );
			ImGui::SliderFloat( "SlerpPercent of Rotation", &rotSlerpFactor, 0.05f, 1.0f );
			ImGui::Text( "" );

			if ( ImGui::TreeNode( "Attack.Advance" ) )
			{
				ImGui::SliderFloat( "Advance.MoveDistance",	&advanceDistance,	0.0f, 360.0f );
				ImGui::SliderInt( "Advance.StartFrame",		&advanceStartFrame,	1, frameWholeAttacking );
				ImGui::SliderInt( "Advance.FinishFrame",	&advanceFinFrame,	1, frameWholeAttacking );
				ImGui::SliderInt( "Return.FinishFrame",		&returnFinFrame,	1, frameWholeAttacking );

				// Easing parameter.
				{
					using namespace Donya;
					constexpr int KIND_COUNT = Easing::GetKindCount();
					constexpr int TYPE_COUNT = Easing::GetTypeCount();
					std::string strKind{};
					std::string strType{};

					ImGui::SliderInt( "Advance.EasingKind", &advanceEaseKind, 0, KIND_COUNT - 1 );
					ImGui::SliderInt( "Advance.EasingType", &advanceEaseType, 0, TYPE_COUNT - 1 );
					strKind = Easing::KindName( advanceEaseKind );
					strType = Easing::TypeName( advanceEaseType );
					ImGui::Text( ( "Advance.Easing : " + strKind + "." + strType ).c_str() );

					ImGui::SliderInt( "Return.EasingKind", &returnEaseKind, 0, KIND_COUNT - 1 );
					ImGui::SliderInt( "Return.EasingType", &returnEaseType, 0, TYPE_COUNT - 1 );
					strKind = Easing::KindName( returnEaseKind );
					strType = Easing::TypeName( returnEaseType );
					ImGui::Text( ( "Return.Easing : " + strKind + "." + strType ).c_str() );
				}

				ImGui::TreePop();
			}

			static std::array<char, 512U> lanceNameBuffer{};
			ImGui::InputText( "MeshName.Lance", lanceNameBuffer.data(), lanceNameBuffer.size() );
			if ( ImGui::Button( "Apply MeshName.Lance" ) )
			{
				lanceMeshName = lanceNameBuffer.data();
			}

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

				auto ShowOBB	= []( const std::string &prefix, Donya::OBB			*pOBB	)
				{
					ImGui::DragFloat3( ( prefix + ".Offset" ).c_str(), &pOBB->pos.x );
					ImGui::DragFloat3( ( prefix + ".Scale" ).c_str(), &pOBB->size.x );

					Donya::Vector3 euler = pOBB->orientation.GetEulerAngles();
					ImGui::SliderFloat3( ( prefix + ".EulerAngle(Radian)" ).c_str(), &euler.x, ToRadian( -360.0f ), ToRadian( 360.0f ) );
					pOBB->orientation = Donya::Quaternion::Make( euler.x, euler.y, euler.z );

					ImGui::Checkbox( ( prefix + ".ExistCollision" ).c_str(), &pOBB->exist );
				};
				auto ShowOBBF	= [&ShowOBB]( const std::string &prefix, Donya::OBBFrame	*pOBBF	)
				{
					ImGui::DragInt( ( prefix + ".EnableFrame.Start" ).c_str(), &pOBBF->enableFrameStart );
					ImGui::DragInt( ( prefix + ".EnableFrame.Last"  ).c_str(), &pOBBF->enableFrameLast  );

					bool oldExistFlag = pOBBF->OBB.exist;
					ShowOBB( prefix, &pOBBF->OBB );
					pOBBF->OBB.exist = oldExistFlag;
				};
				
				ShowAABB  ( "VS_BossAttacks",	&hitBoxBody   );
				ShowSphere( "VS_StageWall",		&hitBoxPhysic );
				ShowAABB  ( "Shield",			&hitBoxShield );
				ShowOBBF  ( "Lance",			&hitBoxLance  );

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

Player::Input Player::Input::MakeByExternalInput( Donya::Vector4x4 viewMat )
{
	Player::Input input{};

	auto ConnectedControllerCount = []()
	{
		return GameLib::input::xInput::getState();
	};
#if !DEBUG_MODE
	if ( 0 < ConnectedControllerCount() )
#endif // !DEBUG_MODE
	{
		// XINPUT_GAMEPAD : https://docs.microsoft.com/ja-jp/windows/win32/api/xinput/ns-xinput-xinput_gamepad

		constexpr int   PAD_NO = 0;
		constexpr float STICK_RANGE_MAX = 32768.0f;
		const auto leftStick = GameLib::input::xInput::getThumbL( PAD_NO );
		if ( leftStick.x != 0 ) { input.moveVector.x = scast<float>( leftStick.x ) / STICK_RANGE_MAX; }
		if ( leftStick.y != 0 ) { input.moveVector.z = scast<float>( leftStick.y ) / STICK_RANGE_MAX; }

		constexpr int TRIGGER_FLAG = 1;
		if ( GameLib::input::xInput::pressedButtons( PAD_NO, XboxPad_Button::RIGHT_SHOULDER ) == TRIGGER_FLAG ) { input.doDefend = true; }
		if ( GameLib::input::xInput::pressedButtons( PAD_NO, XboxPad_Button::X ) == TRIGGER_FLAG ) { input.doAttack = true; }
	}
#if !DEBUG_MODE
	// else
#endif // !DEBUG_MODE
	{
		if ( Donya::Keyboard::Press( VK_UP		) ) { input.moveVector.z = +1.0f; }
		if ( Donya::Keyboard::Press( VK_DOWN	) ) { input.moveVector.z = -1.0f; }
		if ( Donya::Keyboard::Press( VK_LEFT	) ) { input.moveVector.x = -1.0f; }
		if ( Donya::Keyboard::Press( VK_RIGHT	) ) { input.moveVector.x = +1.0f; }

		if ( Donya::Keyboard::Trigger( 'Z' ) ) { input.doDefend = true; }
		if ( Donya::Keyboard::Trigger( 'X' ) ) { input.doAttack = true; }
	}

	input.moveVector.Normalize();

	// Transform to camera space from world space.
	{
		Donya::Vector4x4 cameraRotation{};
		{
			// Extract inverse rotation matrix here.
			cameraRotation = viewMat;
			cameraRotation._14 = 0.0f;
			cameraRotation._24 = 0.0f;
			cameraRotation._34 = 0.0f;
			cameraRotation._41 = 0.0f;
			cameraRotation._42 = 0.0f;
			cameraRotation._43 = 0.0f;
			cameraRotation._44 = 1.0f;

			// XXX : If "viewMat" is invalid matrix, Inverse() will returns NaN.

			cameraRotation = cameraRotation.Inverse();
		}

		Donya::Vector4 moveVector4{};
		moveVector4.x = input.moveVector.x;
		moveVector4.y = input.moveVector.y;
		moveVector4.z = input.moveVector.z;
		moveVector4.w = 0.0f;

		moveVector4 = cameraRotation * moveVector4;

		input.moveVector.x = moveVector4.x;
		input.moveVector.z = moveVector4.z;
	}

	return input;
}

Player::Player() :
	status( State::Idle ),
	timer( 0 ), shieldsRecastTime( 0 ),
	fieldRadius( 0 ),
	pos(), velocity(), lookDirection(), extraOffset(),
	orientation(),
	models(),
	wallCollisions(),
	wasSucceededDefence( false )
{
	auto InitializeModel = []( std::shared_ptr<skinned_mesh> *ppMesh )
	{
		*ppMesh = std::make_shared<skinned_mesh>();
		setAnimFlame( ppMesh->get(), 0 );
	};

	std::vector<std::shared_ptr<skinned_mesh> *> modelRefs
	{
		&models.pIdle,
		&models.pRun,
		&models.pDefend,
		&models.pAttack,
	};
	for ( auto &it : modelRefs )
	{
		InitializeModel( it );
	}
}
Player::~Player() = default;

void Player::Init( const Donya::Vector3 &wsInitPos, const Donya::Vector3 &initRadians, const std::vector<Donya::Box> &wsWallCollisions )
{
	PlayerParam::Get().Init();
	SetFieldRadius( 0.0f ); // Set to body's radius.

	LoadModel();
	
	pos				= wsInitPos;
	orientation		= Donya::Quaternion::Make( initRadians.x, initRadians.y, initRadians.z );
	lookDirection	= orientation.LocalFront();
	wallCollisions	= wsWallCollisions;

	IdleInit( Input::NoOperation() );
}
void Player::Uninit()
{
	PlayerParam::Get().Uninit();

	models.pIdle.reset();
	models.pRun.reset();
	models.pDefend.reset();
	models.pAttack.reset();

	wallCollisions.clear();
}

void Player::Update( Input input )
{
#if USE_IMGUI

	PlayerParam::Get().UseImGui();
	UseImGui();

#endif // USE_IMGUI

	ChangeStatus( input );
	UpdateCurrentStatus( input );
}
void Player::PhysicUpdate( const std::vector<Donya::Circle> &xzCylinderWalls )
{
	ApplyVelocity( xzCylinderWalls );
	CollideToStagesWall();
}

void Player::Draw( fbx_shader &HLSL, const Donya::Vector4x4 &matView, const Donya::Vector4x4 &matProjection )
{
	Donya::Vector4x4 W = CalcWorldMatrix();
	Donya::Vector4x4 WVP = W * matView * matProjection;

	switch ( status )
	{
	case Player::State::Idle:
		FBXRender( models.pIdle.get(), HLSL, WVP, W );
		break;
	case Player::State::Run:
		FBXRender( models.pRun.get(), HLSL,WVP, W );
		break;
	case Player::State::Defend:
		FBXRender( models.pDefend.get(), HLSL,WVP, W );
		break;
	case Player::State::Attack:
		FBXRender( models.pAttack.get(), HLSL, WVP, W );
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

	auto DrawCube	= [&]( const Donya::Vector3 &cubeOffset, const Donya::Vector3 &cubeScale, const Donya::Vector4 &color )
	{
		Donya::Vector4x4 CS = Donya::Vector4x4::MakeScaling( cubeScale * 2.0f ); // Half size->Whole size.
		Donya::Vector4x4 CT = Donya::Vector4x4::MakeTranslation( cubeOffset );
		Donya::Vector4x4 CW = ( CS * CT ) * W;
		Donya::Vector4x4 CWVP = CW * matView * matProjection;

		OBJRender( pCube.get(), CWVP, CW, color );
	};
	auto DrawSphere	= [&]( const Donya::Vector3 &sphereOffset, float sphereScale, const Donya::Vector4 &color )
	{
		Donya::Vector4x4 CS = Donya::Vector4x4::MakeScaling( sphereScale * 2.0f ); // Half size->Whole size.
		Donya::Vector4x4 CT = Donya::Vector4x4::MakeTranslation( sphereOffset );
		Donya::Vector4x4 CW = ( CS * CT ) * W;
		Donya::Vector4x4 CWVP = CW * matView * matProjection;

		OBJRender( pSphere.get(), CWVP, CW, color );
	};
	auto DrawOBB	= [&]( const Donya::OBB &OBB, const Donya::Vector4 &color, bool applyParentMatrix = true )
	{
		Donya::Vector4x4 CS = Donya::Vector4x4::MakeScaling( OBB.size * 2.0f ); // Half size->Whole size.
		Donya::Vector4x4 CR = OBB.orientation.RequireRotationMatrix();
		Donya::Vector4x4 CT = Donya::Vector4x4::MakeTranslation( OBB.pos );
		Donya::Vector4x4 CW = CS * CR * CT;
		if ( applyParentMatrix ) { CW *= W; }
		Donya::Vector4x4 CWVP = CW * matView * matProjection;

		OBJRender( pCube.get(), CWVP, CW, color );
	};

	if ( Donya::IsShowCollision() )
	{
		setBlendMode_ALPHA( 0.5f );

		const auto &PARAM = PlayerParam::Get();

		constexpr Donya::Vector4 BODY_COLOR  { 1.0f, 1.0f, 1.0f, 0.6f };
		constexpr Donya::Vector4 PHYSIC_COLOR{ 0.0f, 0.3f, 0.8f, 0.6f };
		const auto BODY   = PARAM.HitBoxBody();
		const auto PHYSIC = PARAM.HitBoxPhysic();
		if ( BODY.exist   ) { DrawCube  ( BODY.pos,   BODY.size,     BODY_COLOR   ); }
		if ( PHYSIC.exist ) { DrawSphere( PHYSIC.pos, PHYSIC.radius, PHYSIC_COLOR ); }

		if ( status == State::Defend )
		{
			constexpr Donya::Vector4 SHIELD_COLOR{ 0.2f, 0.8f, 0.2f, 0.6f };
			constexpr Donya::Vector4 SHIELD_COLOR_SUCCEEDED{ 0.8f, 1.0f, 1.0f, 0.6f };
			const auto SHIELD = PARAM.HitBoxShield();
			Donya::Vector4 color = ( wasSucceededDefence ) ? SHIELD_COLOR_SUCCEEDED : SHIELD_COLOR;
			DrawCube( SHIELD.pos, SHIELD.size, color );
		}
		if ( status == State::Attack )
		{
			constexpr Donya::Vector4 LANCE_COLOR_VALID{ 1.0f, 0.8f, 0.5f, 0.6f };
			constexpr Donya::Vector4 LANCE_COLOR_INVALID{ 0.5f, 0.5f, 0.5f, 0.6f };
			const Donya::OBB actualOBB = CalcAttackHitBox();
			Donya::Vector4 color = ( actualOBB.exist ) ? LANCE_COLOR_VALID: LANCE_COLOR_INVALID;
			DrawOBB( actualOBB, color, /* applyParentMatrix = */ false );
		}
	}

#endif // DEBUG_MODE
}

static Donya::OBB MakeOBB( const Donya::AABB &AABB, const Donya::Vector3 &wsPos, const Donya::Quaternion &orientation )
{
	Donya::OBB OBB{};
	OBB.pos		= AABB.pos + wsPos;
	OBB.size	= AABB.size;
	OBB.orientation = orientation;
	OBB.exist	= AABB.exist;
	return OBB;
}
Donya::OBB Player::GetHurtBox() const
{
	const auto &HITBOX = PlayerParam::Get().HitBoxBody();

	Donya::OBB wsOBB = MakeOBB( HITBOX, GetPosition(), orientation );
	return wsOBB;
}
Donya::OBB Player::GetShieldHitBox() const
{
	const auto &HITBOX = PlayerParam::Get().HitBoxShield();

	Donya::OBB wsOBB = MakeOBB( HITBOX, GetPosition(), orientation );
	if ( status != State::Defend ) { wsOBB.exist = false; }
	return wsOBB;
}
Donya::OBB Player::CalcAttackHitBox() const
{
	const auto &PARAM = PlayerParam::Get();
	const auto *pOBBF = PARAM.HitBoxAttackF();

	const std::string meshName = PARAM.LanceMeshName();

	Donya::OBB oldOBB = pOBBF->OBB;

	// To world space from local space.
	Donya::Vector4x4 parentMatrix = CalcWorldMatrix();

	// To local space from lance's mesh space.
	Donya::Vector4x4 lanceMatrix{};
	{
		// Input:offset in local-space. Output:transformed position.
		Donya::Vector3 outputPos = oldOBB.pos;
		bool succeeded = calcTransformedPosBySpecifyMesh( models.pAttack.get(), outputPos, meshName );
		if ( !succeeded )
		{
			_ASSERT_EXPR( 0, L"Error : specified mesh name was not found !" );
			return Donya::OBB::Nil();
		}
		Donya::Vector4x4 T = Donya::Vector4x4::MakeTranslation( outputPos );

		// Convert to world space from local space.
		lanceMatrix = T * parentMatrix;
	}

	Donya::OBB resultOBB = oldOBB;
	resultOBB.pos.x = lanceMatrix._41;
	resultOBB.pos.y = lanceMatrix._42;
	resultOBB.pos.z = lanceMatrix._43;

	// Rotation.
	{
		Donya::Vector3 localFront = oldOBB.pos + Donya::Vector3::Front();
		calcTransformedPosBySpecifyMesh( models.pAttack.get(), localFront, meshName );
		localFront.Normalize();

		Donya::Vector4x4 LT = Donya::Vector4x4::MakeTranslation( localFront );
		Donya::Vector4x4 M = LT * parentMatrix;

		Donya::Vector3 resultFront{};
		resultFront.x = M._41;
		resultFront.y = M._42;
		resultFront.z = M._43;

		Donya::Vector3 dir = resultFront - resultOBB.pos;

		oldOBB.orientation = Donya::Quaternion::LookAt( oldOBB.orientation, dir );
		resultOBB.orientation = oldOBB.orientation;
	}

	// Should I also apply parent's scale ?
	return resultOBB;
}

void Player::SucceededDefence()
{
	wasSucceededDefence = true;
	Donya::Sound::Play( scast<int>( MusicAttribute::PlayerProtected ) );
}

void Player::ReceiveImpact()
{
	status = State::Dead;
	velocity = 0.0f;
}

void Player::SetFieldRadius( float newFieldRadius )
{
	const float bodyRadius = PlayerParam::Get().HitBoxPhysic().radius;
	fieldRadius = std::max( newFieldRadius, bodyRadius );
}

void Player::LoadModel()
{
	Donya::OutputDebugStr( "Begin Player::LoadModel.\n" );

	loadFBX( models.pIdle.get(),	GetModelPath( ModelAttribute::PlayerIdle	) );
	Donya::OutputDebugStr( "Done PlayerModel.Wait\n" );
	loadFBX( models.pRun.get(),		GetModelPath( ModelAttribute::PlayerRun		) );
	Donya::OutputDebugStr( "Done PlayerModel.Run\n" );
	loadFBX( models.pDefend.get(),	GetModelPath( ModelAttribute::PlayerDefend	) );
	Donya::OutputDebugStr( "Done PlayerModel.Defend\n" );
	loadFBX( models.pAttack.get(),	GetModelPath( ModelAttribute::PlayerAtk		) );
	Donya::OutputDebugStr( "Done PlayerModel.Attack\n" );

	std::vector<std::shared_ptr<skinned_mesh> *> dontLoopModels
	{
		&models.pDefend,
		&models.pAttack,
	};
	for ( auto &it : dontLoopModels )
	{
		setLoopFlg( it->get(), /* is_loop = */ false );
	}

	Donya::OutputDebugStr( "End Player::LoadModel.\n" );
}

Donya::Vector4x4 Player::CalcWorldMatrix() const
{
	Donya::Vector4x4 S = Donya::Vector4x4::MakeScaling( PlayerParam::Get().Scale() );
	Donya::Vector4x4 R = orientation.RequireRotationMatrix();
#if DEBUG_MODE
	if ( status == State::Dead ) { R = Donya::Quaternion::Make( Donya::Vector3::Front(), ToRadian( 180.0f ) ).RequireRotationMatrix(); }
#endif // DEBUG_MODE
	Donya::Vector4x4 T = Donya::Vector4x4::MakeTranslation( GetPosition() );
	return S * R * T;
}

void Player::ChangeStatus( Input input )
{
#if DEBUG_MODE

	if ( status == State::Dead && input.doAttack )
	{
		status = State::Idle;
	}

#endif // DEBUG_MODE

	if ( 0 < shieldsRecastTime )
	{
		shieldsRecastTime--;
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
	if ( input.doDefend && !shieldsRecastTime )
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

	setAnimFlame( models.pIdle.get(), 0 );
}
void Player::IdleUpdate( Input input )
{
	// No op.
}
void Player::IdleUninit()
{
	setAnimFlame( models.pIdle.get(), 0 );
}

void Player::ChangeStatusFromRun( Input input )
{
	if ( input.doDefend && !shieldsRecastTime )
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

	setAnimFlame( models.pRun.get(), 0 );
}
void Player::RunUpdate( Input input )
{
	AssignInputVelocity( input );
}
void Player::RunUninit()
{
	setAnimFlame( models.pRun.get(), 0 );
}

void Player::ChangeStatusFromDefend( Input input )
{
	bool cancelable = wasSucceededDefence;
	bool doCancel   = cancelable && ( !input.moveVector.IsZero() || input.doAttack );
	if ( doCancel )
	{
		DefendUninit();

		if ( input.doAttack )
		{
			AttackInit( input );
		}
		else
		{
			RunInit( input );
		}
	}
	else
	if ( timer <= 0 )
	{
		DefendUninit();

		// If allows hold attack button, insert AttackInit() here.

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
	shieldsRecastTime	= 0;
	wasSucceededDefence	= false;

	const auto &PARAM = PlayerParam::Get();
	timer = PARAM.FrameWholeDefence();

	setAnimFlame( models.pDefend.get(), 0 );

	Donya::Sound::Play( scast<int>( MusicAttribute::PlayerDefend ) );
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
	shieldsRecastTime	= ( wasSucceededDefence ) ? 0 : PlayerParam::Get().FrameReuseShield();
	wasSucceededDefence	= false;

	setAnimFlame( models.pDefend.get(), 0 );
}

void Player::ChangeStatusFromAttack( Input input )
{
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

	const auto &PARAM		= PlayerParam::Get();
	const int  WHOLE_FRAME	= PARAM.FrameWholeAttacking();
	const int  CANCELABLE	= PARAM.FrameCancelableAttack();
	const bool withinCancelableRange = ( WHOLE_FRAME - CANCELABLE ) <= timer;
	if ( input.doDefend && !shieldsRecastTime && withinCancelableRange )
	{
		AttackUninit();
		DefendInit( input );

		return;
	}
	// else
}
void Player::AttackInit( Input input )
{
	status				= State::Attack;
	timer				= PlayerParam::Get().FrameWholeAttacking();
	velocity			= 0.0f; // Each member set to zero.
	extraOffset			= 0.0f;
	shieldsRecastTime	= 0;	// This allows attack cancel anytime.

	Donya::OBBFrame *pOBBF = PlayerParam::Get().HitBoxAttackF();
	pOBBF->currentFrame = 0;
	setAnimFlame( models.pAttack.get(), 0 );
}
void Player::AttackUpdate( Input input )
{
	timer--;

	const int  WHOLE_FRAME	= PlayerParam::Get().FrameWholeAttacking();
	const int  CANCELABLE	= PlayerParam::Get().FrameCancelableAttack();
	const bool withinCancelableRange = ( WHOLE_FRAME - CANCELABLE ) <= timer;
	if ( !input.moveVector.IsZero() && withinCancelableRange )
	{
		lookDirection = input.moveVector;
	}

	const int  STOP_TIMING	= PlayerParam::Get().FrameStopAnimeTiming();
	const int  STOP_LENGTH	= PlayerParam::Get().FrameStopAnimeLength();
	if ( timer == ( WHOLE_FRAME - STOP_TIMING ) )
	{
		setStopAnimation( models.pAttack.get(), /* is_stop = */ true );
	}
	else
	if ( timer == ( WHOLE_FRAME - STOP_TIMING ) - STOP_LENGTH )
	{
		setStopAnimation( models.pAttack.get(), /* is_stop = */ false );
	}

	// Easing process.
	{
		const int ADVANCE_START	= PlayerParam::Get().FrameAdvanceStart();
		const int ADVANCE_FIN	= PlayerParam::Get().FrameAdvanceFin();
		const int RETURN_START	= ADVANCE_FIN;
		const int RETURN_FIN	= PlayerParam::Get().FrameReturnFin();

		auto CalcEaseFactor		= []( int elapsedTime, int start, int fin, int kind, int type )
		{
			float percent = scast<float>( elapsedTime - start ) / scast<float>( fin - start );
			return Donya::Easing::Ease
			(
				scast<Donya::Easing::Kind>( kind ),
				scast<Donya::Easing::Type>( type ),
				percent
			);
		};

		auto ApplyExtraOffset	= [&]()->void
		{
			// Apply movement and reset extraOffset.
			pos += extraOffset;
			extraOffset = 0.0f;
		};

		int elapsedTime = WHOLE_FRAME - timer;
		if ( ADVANCE_START <= elapsedTime && elapsedTime < ADVANCE_FIN )
		{
			float ease = CalcEaseFactor
			(
				elapsedTime,
				ADVANCE_START, ADVANCE_FIN,
				PlayerParam::Get().AdvanceEaseKind(),
				PlayerParam::Get().AdvanceEaseType()
			);
			
			extraOffset = orientation.LocalFront() * ( PlayerParam::Get().AdvanceDistance() * ease );

			if ( elapsedTime == ADVANCE_FIN - 1 )
			{
				ApplyExtraOffset();
			}
		}
		else
		if ( RETURN_START <= elapsedTime && elapsedTime < RETURN_FIN )
		{
			float ease = CalcEaseFactor
			(
				elapsedTime,
				RETURN_START, RETURN_FIN,
				PlayerParam::Get().ReturnEaseKind(),
				PlayerParam::Get().ReturnEaseType()
			);

			extraOffset = -orientation.LocalFront() * ( PlayerParam::Get().AdvanceDistance() * ease );

			if ( elapsedTime == RETURN_FIN - 1 )
			{
				ApplyExtraOffset();
			}
		}
	}

	Donya::OBBFrame *pOBBF	= PlayerParam::Get().HitBoxAttackF();
	pOBBF->Update();
}
void Player::AttackUninit()
{
	timer		= 0;
	extraOffset	= 0.0f;

	Donya::OBBFrame *pOBBF = PlayerParam::Get().HitBoxAttackF();
	pOBBF->currentFrame = 0;
	setAnimFlame( models.pAttack.get(), 0 );
}

void Player::AssignInputVelocity( Input input )
{
	velocity = input.moveVector;
	velocity *= PlayerParam::Get().RunSpeed();
}

void Player::ApplyVelocity( const std::vector<Donya::Circle> &xzCylinderWalls )
{
	/// <summary>
	/// The "xzNAxis" is specify moving axis. please only set to { 1, 0 } or { 0, 1 }.
	/// </summary>
	auto MoveSpecifiedAxis = [&]( Donya::Vector2 xzNAxis, float moveSpeed )->void
	{
		if ( ZeroEqual( moveSpeed ) ) { return; }
		// else

		// Only either X or Z is valid.
		const Donya::Vector2 xzVelocity = xzNAxis * moveSpeed;
		pos.x += xzVelocity.x;
		pos.z += xzVelocity.y;

		// Take a value of +1 or -1.
		const float moveSign = scast<float>( Donya::SignBit( xzVelocity.x ) + Donya::SignBit( xzVelocity.y ) );
		const auto  actualBody = PlayerParam::Get().HitBoxPhysic();

		// VS Cylinder walls.
		{
			Donya::Circle xzBody{};
			{
				xzBody.cx		= GetPosition().x;
				xzBody.cy		= GetPosition().z;
				xzBody.radius	= actualBody.radius;
				xzBody.exist	= true;
			}
			Donya::Vector2 xzBodyCenter{ xzBody.cx, xzBody.cy };
			for ( const auto &it : xzCylinderWalls )
			{
				if ( !Donya::Circle::IsHitCircle( xzBody, it ) ) { continue; }
				// else

				Donya::Vector2 xzWallCenter{ it.cx, it.cy };

				Donya::Vector2 xznVecToBody = xzBodyCenter - xzWallCenter;
				xznVecToBody.Normalize();

				float correctLength = xzBody.radius + it.radius;
				correctLength += 0.1f; // Prevent the two circles onto same place(the collision detective allows same(equal) value).

				Donya::Vector2 correction{};
				// correction.x = ( xznVecToBody.x * ( xzNAxis.x * correctLength ) ); // The "xzNAxis" is unnecessary.
				// correction.y = ( xznVecToBody.y * ( xzNAxis.y * correctLength ) ); // The "xzNAxis" is unnecessary.
				correction.x = ( xznVecToBody.x * ( correctLength ) );
				correction.y = ( xznVecToBody.y * ( correctLength ) );

				pos.x = xzWallCenter.x + correction.x;
				pos.z = xzWallCenter.y + correction.y;

				// Calculating position contains "extraOffset", so I should except "extraOffset".
				pos.x -= extraOffset.x;
				pos.z -= extraOffset.z;
			}
		}

		// VS Rectangle walls.
		{
			const float bodyWidth = actualBody.radius;

			// The player's hit box of stage is circle, but doing with rectangle for easily correction.
			Donya::Box xzBody{};
			{
				xzBody.cx		= GetPosition().x;
				xzBody.cy		= GetPosition().z;
				xzBody.w		= bodyWidth;
				xzBody.h		= bodyWidth;
				xzBody.exist	= true;
			}
			Donya::Vector2 xzBodyCenter{ xzBody.cx, xzBody.cy };

			for ( const auto &wall : wallCollisions )
			{
				if ( !Donya::Box::IsHitBox( xzBody, wall ) ) { continue; }
				// else

				Donya::Vector2 xzWallCenter{ wall.cx, wall.cy };
				Donya::Vector2 wallSize{ wall.w * xzNAxis.x, wall.h * xzNAxis.y }; // Only either X or Z is valid.
				float wallWidth = wallSize.Length(); // Extract valid member by Length().

				// Calculate colliding length.
				// First, calculate body's edge of moving side.
				// Then, calculate wall's edge of inverse moving side.
				// After that, calculate colliding length from two edges.
				// Finally, correct the position to inverse moving side only that length.

				Donya::Vector2 bodyEdge = xzBodyCenter + ( xzNAxis * bodyWidth * moveSign );
				Donya::Vector2 wallEdge = xzWallCenter + ( xzNAxis * wallWidth * -moveSign );
				Donya::Vector2 diff = bodyEdge - wallEdge;
				Donya::Vector2 axisDiff{ diff.x * xzNAxis.x, diff.y * xzNAxis.y };
				float collidingLength = axisDiff.Length();
				collidingLength += 0.1f; // Prevent the two edges onto same place(the collision detective allows same(equal) value).

				Donya::Vector2 xzCorrection
				{
					xzNAxis.x * ( collidingLength * -moveSign ),
					xzNAxis.y * ( collidingLength * -moveSign )
				};
				pos.x += xzCorrection.x;
				pos.z += xzCorrection.y;

				// Calculating position contains "extraOffset", so I should except "extraOffset".
				pos.x -= extraOffset.x;
				pos.z -= extraOffset.z;
			}
		}
	};

	if ( !velocity.IsZero() )
	{
		// Move to X-axis with collision.
		MoveSpecifiedAxis( Donya::Vector2{ 1.0f, 0.0f }, velocity.x );
		// Move to Y-axis only.
		pos.y += velocity.y;
		// Move to Z-axis with collision.
		MoveSpecifiedAxis( Donya::Vector2{ 0.0f, 1.0f }, velocity.z );

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

void Player::CollideToStagesWall()
{
	const float bodyRadius = PlayerParam::Get().HitBoxPhysic().radius;
	const float trueFieldRadius = fieldRadius - bodyRadius;

	constexpr Donya::Vector3 ORIGIN = Donya::Vector3::Zero();
	const Donya::Vector3 currentDistance = GetPosition() - ORIGIN;
	const float currentLength = currentDistance.Length();

	if ( trueFieldRadius < currentLength )
	{
		float diff = currentLength - trueFieldRadius;
		const Donya::Vector3 direction = currentDistance.Normalized();
		pos = ORIGIN + ( direction * ( currentLength - diff ) );
		pos -= extraOffset; // Calculating position contains "extraOffset", so I should except "extraOffset".
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
			ImGui::Text( "Timer : %d",				timer );
			ImGui::Text( "ShieldRecast : %d",		shieldsRecastTime );
			ImGui::Text( "ShieldSucceeded : %d",	wasSucceededDefence ? 1 : 0 );
			ImGui::Text( "FieldRadius : %f",		fieldRadius );
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
