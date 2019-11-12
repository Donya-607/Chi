#include "Player.h"

#include <array>

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
			ImGui::SliderInt( "Defence.WholeExpandingFrame", &frameUnfoldableDefence, 1, 120 );
			ImGui::SliderInt( "Defence.RecastFrame", &shieldsRecastFrame, 0, 360 );
			ImGui::SliderInt( "Attack.CancelableFrame", &frameCancelableAttack, 1, frameWholeAttacking );
			ImGui::SliderInt( "Attack.WholeAttackingFrame", &frameWholeAttacking, 1, 300 );
			ImGui::Text( "" );

			ImGui::SliderFloat( "Scale", &scale, 0.0f, 8.0f );
			ImGui::DragFloat( "Running Speed", &runSpeed );
			ImGui::SliderFloat( "SlerpPercent of Rotation", &rotSlerpFactor, 0.05f, 1.0f );
			ImGui::Text( "" );

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

Player::Player() :
	status( State::Idle ),
	timer( 0 ), shieldsRecastTime( 0 ),
	fieldRadius( 0 ),
	pos(), velocity(), lookDirection(),
	orientation(),
	models(),
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

	std::vector<std::shared_ptr<skinned_mesh> *> dontLoopModels
	{
		&models.pDefend,
		&models.pAttack,
	};
	for ( auto &it : dontLoopModels )
	{
		setLoopFlg( it->get(), /* is_loop = */ false );
	}
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

	models.pIdle.reset();
	models.pRun.reset();
	models.pDefend.reset();
	models.pAttack.reset();
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

	Donya::OBB wsOBB = MakeOBB( HITBOX, pos, orientation );
	return wsOBB;
}
Donya::OBB Player::GetShieldHitBox() const
{
	const auto &HITBOX = PlayerParam::Get().HitBoxShield();

	Donya::OBB wsOBB = MakeOBB( HITBOX, pos, orientation );
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
	// loadFBX( models.pDefend.get(),	GetModelPath( ModelAttribute::PlayerDefend	) );
	// Donya::OutputDebugStr( "Done PlayerModel.Defend\n" );
	loadFBX( models.pAttack.get(),	GetModelPath( ModelAttribute::PlayerAtk		) );
	Donya::OutputDebugStr( "Done PlayerModel.Attack\n" );

	Donya::OutputDebugStr( "End Player::LoadModel.\n" );
}

Donya::Vector4x4 Player::CalcWorldMatrix() const
{
	Donya::Vector4x4 S = Donya::Vector4x4::MakeScaling( PlayerParam::Get().Scale() );
	Donya::Vector4x4 R = orientation.RequireRotationMatrix();
#if DEBUG_MODE
	if ( status == State::Dead ) { R = Donya::Quaternion::Make( Donya::Vector3::Front(), ToRadian( 180.0f ) ).RequireRotationMatrix(); }
#endif // DEBUG_MODE
	Donya::Vector4x4 T = Donya::Vector4x4::MakeTranslation( pos );
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
	status		= State::Attack;
	velocity	= 0.0f; // Each member set to zero.

	timer		= PlayerParam::Get().FrameWholeAttacking();

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

	Donya::OBBFrame *pOBBF = PlayerParam::Get().HitBoxAttackF();
	pOBBF->Update();
}
void Player::AttackUninit()
{
	timer = 0;

	Donya::OBBFrame *pOBBF = PlayerParam::Get().HitBoxAttackF();
	pOBBF->currentFrame = 0;
	setAnimFlame( models.pAttack.get(), 0 );
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
	const float trueFieldRadius = fieldRadius - bodyRadius;

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
