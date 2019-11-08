#include "Knight.h"

#include <array>

#include "Donya/Easing.h"
#include "Donya/FilePath.h"
#include "Donya/Useful.h"		// For IsShowCollision().

#include "gameLib.h"
#include "gameLibFunctions.h"	// Use FBXRender().
#include "skinned_mesh.h"

#undef max
#undef min

#define scast static_cast

Donya::Sphere KnightParam::SphereFrameWithName::CalcTransformedSphere( skinned_mesh *pMesh, const Donya::Vector4x4 &parentSpaceMatrix ) const
{
	const Donya::Sphere &sphere = sphereF.collision;

	// To local space from mesh's mesh space.
	Donya::Vector4x4 meshMatrix{};
	{
		// Input:offset in local-space. Output:transformed position.
		Donya::Vector3 outputPos = sphere.pos;
		bool succeeded = calcTransformedPosBySpecifyMesh( pMesh, outputPos, meshName );
		if ( !succeeded )
		{
			_ASSERT_EXPR( 0, L"Error : specified mesh name was not found !" );
			return Donya::Sphere::Nil();
		}
		Donya::Vector4x4 T = Donya::Vector4x4::MakeTranslation( outputPos );

		// Convert to world space from local space.
		meshMatrix = T * parentSpaceMatrix;
	}

	Donya::Sphere resultSphere = sphere;
	resultSphere.pos.x = meshMatrix._41;
	resultSphere.pos.y = meshMatrix._42;
	resultSphere.pos.z = meshMatrix._43;
	return resultSphere;
}

KnightParam::KnightParam() :
	m()
{}
KnightParam::~KnightParam() = default;

void KnightParam::Init()
{
	LoadParameter();
}
void KnightParam::Uninit()
{
	// No op.
}

float KnightParam::SlerpFactor( KnightAI::ActionState status )
{
	switch ( status )
	{
	case KnightAI::ActionState::WAIT:				return m.idleSlerpFactor;	// break;
	case KnightAI::ActionState::MOVE:				return m.moveSlerpFactor;	// break;
	case KnightAI::ActionState::ATTACK_EXPLOSION:	return m.explSlerpFactor;	// break;
	case KnightAI::ActionState::ATTACK_SWING:		return m.swingSlerpFactor;	// break;
	case KnightAI::ActionState::ATTACK_RAID:		return m.raidSlerpFactor;	// break;
	default: break;
	}

	return NULL;
}
float KnightParam::MoveSpeed( KnightAI::ActionState status )
{
	switch ( status )
	{
	case KnightAI::ActionState::WAIT:				return 0.0f;				// break;
	case KnightAI::ActionState::MOVE:				return m.moveMoveSpeed;		// break;
	case KnightAI::ActionState::ATTACK_EXPLOSION:	return m.explMoveSpeed;		// break;
	case KnightAI::ActionState::ATTACK_SWING:		return m.swingMoveSpeed;	// break;
	case KnightAI::ActionState::ATTACK_RAID:		return m.raidMoveSpeed;		// break;
	default: break;
	}

	return NULL;
}

KnightParam::Member KnightParam::Open()
{
	return Get().Content();
}

void KnightParam::LoadParameter( bool isBinary )
{
	Donya::Serializer::Extension ext = ( isBinary )
	? Donya::Serializer::Extension::BINARY
	: Donya::Serializer::Extension::JSON;
	std::string filePath = GenerateSerializePath( SERIAL_ID, ext );

	Donya::Serializer seria;
	seria.Load( ext, filePath.c_str(), SERIAL_ID, *this );
}

#if USE_IMGUI

void KnightParam::SaveParameter()
{
	Donya::Serializer::Extension bin  = Donya::Serializer::Extension::BINARY;
	Donya::Serializer::Extension json = Donya::Serializer::Extension::JSON;
	std::string binPath  = GenerateSerializePath( SERIAL_ID, bin  );
	std::string jsonPath = GenerateSerializePath( SERIAL_ID, json );

	Donya::Serializer seria;
	seria.Save( bin,  binPath.c_str(),  SERIAL_ID, *this );
	seria.Save( json, jsonPath.c_str(), SERIAL_ID, *this );
}

void KnightParam::UseImGui()
{
	if ( ImGui::BeginIfAllowed() )
	{
		if ( ImGui::TreeNode( "Knight.AdjustData" ) )
		{
			ImGui::SliderFloat( "Scale", &m.scale, 0.0f, 8.0f );
			ImGui::DragFloat( "BodyRadiusToStage", &m.stageBodyRadius );
			ImGui::Text( "" );

			ImGui::SliderFloat( "Distance.ThinkTo\"Near\"", &m.targetDistNear, 0.01f, 1.0f );
			ImGui::SliderFloat( "Distance.ThinkTo\"Far\"",  &m.targetDistFar,  0.0f, 0.99f );
			ImGui::Text( "" );

			ImGui::DragFloat( "MoveSpeed.\"Move\"State",			&m.moveMoveSpeed );
			ImGui::DragFloat( "MoveSpeed.\"Atk.Explosion\"State",	&m.explMoveSpeed );
			ImGui::DragFloat( "MoveSpeed.\"Atk.Swing\"State",		&m.swingMoveSpeed );
			ImGui::DragFloat( "MoveSpeed.\"Atk.Raid\"State",		&m.raidMoveSpeed );
			ImGui::Text( "" );

			ImGui::SliderFloat( "SlerpFactor.\"Idle\"State",			&m.idleSlerpFactor, 0.0f, 1.0f );
			ImGui::SliderFloat( "SlerpFactor.\"Move\"State",			&m.moveSlerpFactor, 0.0f, 1.0f );
			ImGui::SliderFloat( "SlerpFactor.\"Atk.Explosion\"State",	&m.explSlerpFactor, 0.0f, 1.0f );
			ImGui::SliderFloat( "SlerpFactor.\"Atk.Swing\"State",		&m.swingSlerpFactor, 0.0f, 1.0f );
			ImGui::SliderFloat( "SlerpFactor.\"Atk.Raid\"State",		&m.raidSlerpFactor, 0.0f, 1.0f );
			ImGui::Text( "" );

			ImGui::DragFloat3( "Initialize.Position", &m.initPos.x    );
			ImGui::DragFloat3( "DrawPosition.Offset", &m.drawOffset.x );
			ImGui::Text( "" );

			if ( ImGui::TreeNode( "Collisions" ) )
			{
				auto ShowAABB	= []( const std::string &prefix, Donya::AABB *pAABB )
				{
					ImGui::DragFloat3( ( prefix + ".Offset" ).c_str(), &pAABB->pos.x );
					ImGui::DragFloat3( ( prefix + ".Scale" ).c_str(), &pAABB->size.x );
					ImGui::Checkbox( ( prefix + ".ExistCollision" ).c_str(), &pAABB->exist );
				};
				auto ShowSphere	= []( const std::string &prefix, Donya::Sphere *pSphere )
				{
					ImGui::DragFloat3( ( prefix + ".Offset" ).c_str(), &pSphere->pos.x );
					ImGui::DragFloat( ( prefix + ".Scale" ).c_str(), &pSphere->radius );
					ImGui::Checkbox( ( prefix + ".ExistCollision" ).c_str(), &pSphere->exist );
				};
				auto ShowOBB	= []( const std::string &prefix, Donya::OBB *pOBB )
				{
					ImGui::DragFloat3( ( prefix + ".Offset" ).c_str(), &pOBB->pos.x );
					ImGui::DragFloat3( ( prefix + ".Scale" ).c_str(), &pOBB->size.x );

					Donya::Vector3 euler = pOBB->orientation.GetEulerAngles();
					ImGui::SliderFloat3( ( prefix + ".EulerAngle(Radian)" ).c_str(), &euler.x, ToRadian( -360.0f ), ToRadian( 360.0f ) );
					pOBB->orientation = Donya::Quaternion::Make( euler.x, euler.y, euler.z );

					ImGui::Checkbox( ( prefix + ".ExistCollision" ).c_str(), &pOBB->exist );
				};
				
				auto ShowSphereF  = [&ShowSphere]( const std::string &prefix, Donya::SphereFrame *pSphereF )
				{
					ImGui::DragInt( ( prefix + ".EnableFrame.Start" ).c_str(), &pSphereF->enableFrameStart );
					ImGui::DragInt( ( prefix + ".EnableFrame.Last" ).c_str(), &pSphereF->enableFrameLast );

					bool oldExistFlag = pSphereF->collision.exist;
					ShowSphere( prefix, &pSphereF->collision );
					pSphereF->collision.exist = oldExistFlag;
				};
				auto ShowSphereFN = [&ShowSphereF]( const std::string &prefix, SphereFrameWithName *pSphereFN, std::array<char, 512> *pMeshName )
				{
					if ( ImGui::TreeNode( prefix.c_str() ) )
					{
						ImGui::InputText( "MeshName", pMeshName->data(), pMeshName->size() );
						if ( ImGui::Button( "Apply.MeshName" ) )
						{
							pSphereFN->meshName = pMeshName->data();
						}

						ShowSphereF( prefix, &pSphereFN->sphereF );

						ImGui::TreePop();
					}
				};
				
				ShowSphere( "Body.HurtBox", &m.hitBoxBody );

				if ( ImGui::TreeNode( "Attack.Explosion" ) )
				{
					ImGui::DragFloat( "Scale.Start",	&m.explScaleStart, 0.2f );
					ImGui::DragFloat( "Scale.End",		&m.explScaleEnd,   0.2f );
					ShowSphereF( "Collision", &m.hitBoxExpl );
				}
				
				static std::array<char, 512U> swingMeshNameBuffer{};
				ShowSphereFN( "Attack.Swing", &m.hitBoxSwing, &swingMeshNameBuffer );

				static std::array<char, 512U> raidMeshNameBuffer{};
				ShowSphereFN( "Attack.Raid", &m.hitBoxRaid, &raidMeshNameBuffer );

				ImGui::TreePop();
			}
			ImGui::Text( "" );

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

void ResetCurrentSphereFN( KnightParam::SphereFrameWithName *pSphereFN )
{
	pSphereFN->sphereF.currentFrame = 0;
	pSphereFN->sphereF.collision.enable = true;
}

Knight::Knight() :
	status( KnightAI::ActionState::WAIT ),
	AI(),
	timer(),
	fieldRadius(), slerpFactor( 1.0f ),
	pos(), velocity(), extraOffset(),
	orientation(),
	models()
{
	auto InitializeModel = []( std::shared_ptr<skinned_mesh> *ppMesh )
	{
		*ppMesh = std::make_shared<skinned_mesh>();
		setAnimFlame( ppMesh->get(), 0 );
	};

	std::vector<std::shared_ptr<skinned_mesh> *> modelRefs
	{
		&models.pIdle,
		&models.pRunFront,
		&models.pAtkExpl,
		&models.pAtkSwing,
		&models.pAtkRaid,
	};
	for ( auto &it : modelRefs )
	{
		InitializeModel( it );
	}
}
Knight::~Knight() = default;

void Knight::Init( int stageNumber )
{
	KnightParam::Get().Init();

	LoadModel();

	status	= KnightAI::ActionState::WAIT;
	pos		= KnightParam::Open().initPos;

	SetFieldRadius( 0.0f ); // Set to body's radius.

	Donya::Vector3 dirToOrigin = Donya::Vector3::Zero() - pos;
	dirToOrigin.y = 0.0f;
	orientation = Donya::Quaternion::LookAt( orientation, dirToOrigin );

	AI.Init();
}
void Knight::Uninit()
{
	KnightParam::Get().Uninit();
}

void Knight::Update( TargetStatus target )
{
#if USE_IMGUI

	KnightParam::Get().UseImGui();
	UseImGui();

#endif // USE_IMGUI

	AI.Update();

	ChangeStatus( target );
	UpdateCurrentStatus( target );

	ApplyVelocity( target );
	CollideToWall();
}

void Knight::Draw( fbx_shader &HLSL, const Donya::Vector4x4 &matView, const Donya::Vector4x4 &matProjection )
{
	const auto &PARAM = KnightParam::Open();

	const Donya::Vector3 drawOffset = PARAM.drawOffset;
	const Donya::Vector4x4 DRAW_OFFSET = Donya::Vector4x4::MakeTranslation( drawOffset );

	Donya::Vector4x4 S = Donya::Vector4x4::MakeScaling( PARAM.scale );
	Donya::Vector4x4 R = orientation.RequireRotationMatrix();
	Donya::Vector4x4 T = Donya::Vector4x4::MakeTranslation( GetPos() );
#if DEBUG_MODE
	if ( status == decltype( status )::END ) { R = Donya::Quaternion::Make( Donya::Vector3::Front(), ToRadian( 180.0f ) ).RequireRotationMatrix(); };
#endif // DEBUG_MODE
	Donya::Vector4x4 W = S * R * T * DRAW_OFFSET;
	Donya::Vector4x4 WVP = W * matView * matProjection;

	switch ( status )
	{
	case KnightAI::ActionState::WAIT:
		FBXRender( models.pIdle.get(), HLSL, WVP, W );
		break;
	case KnightAI::ActionState::MOVE:
		FBXRender( models.pRunFront.get(), HLSL, WVP, W );
		break;
	case KnightAI::ActionState::ATTACK_EXPLOSION:
		FBXRender( models.pAtkExpl.get(), HLSL, WVP, W );
		break;
	case KnightAI::ActionState::ATTACK_SWING:
		FBXRender( models.pAtkSwing.get(), HLSL, WVP, W );
		break;
	case KnightAI::ActionState::ATTACK_RAID:
		FBXRender( models.pAtkRaid.get(), HLSL, WVP, W );
		break;
	default: break;
	}

	// For debug, helpers of drawing primitive. and drawing collisions.
#if DEBUG_MODE

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
	static std::shared_ptr<static_mesh> pCube = GenerateCube();
	static std::shared_ptr<static_mesh> pSphere = GenerateSphere();

	// Except DRAW_OFFSET matrix. because I wanna draw collisions to actual position.
	W = S * R * T;

	auto DrawCube = [&]( const Donya::Vector3 &cubeOffset, const Donya::Vector3 &cubeScale, const Donya::Quaternion &orientation, const Donya::Vector4 &color, bool applyParentMatrix = true, bool applyParentDrawOffset = false )
	{
		Donya::Vector4x4 CS = Donya::Vector4x4::MakeScaling( cubeScale * 2.0f ); // Half size->Whole size.
		Donya::Vector4x4 CR = orientation.RequireRotationMatrix();
		Donya::Vector4x4 CT = Donya::Vector4x4::MakeTranslation( cubeOffset );
		Donya::Vector4x4 CW = ( CS * CR * CT );
		if ( applyParentMatrix ) { CW *= W; }
		if ( applyParentDrawOffset ) { CW *= DRAW_OFFSET; }
		Donya::Vector4x4 CWVP = CW * matView * matProjection;

		OBJRender( pCube.get(), CWVP, CW, color );
	};
	auto DrawSphere = [&]( const Donya::Vector3 &sphereOffset, float sphereScale, const Donya::Vector4 &color, bool applyParentMatrix = true, bool applyParentDrawOffset = false )
	{
		Donya::Vector4x4 CS = Donya::Vector4x4::MakeScaling( sphereScale * 2.0f ); // Half size->Whole size.
		Donya::Vector4x4 CT = Donya::Vector4x4::MakeTranslation( sphereOffset );
		Donya::Vector4x4 CW = ( CS * CT );
		if ( applyParentMatrix ) { CW *= W; }
		if ( applyParentDrawOffset ) { CW *= DRAW_OFFSET; }
		Donya::Vector4x4 CWVP = CW * matView * matProjection;

		OBJRender( pSphere.get(), CWVP, CW, color );
	};
	auto DrawOBB = [&]( const Donya::OBB &OBB, const Donya::Vector4 &color, bool applyParentMatrix = true, bool applyParentDrawOffset = false )
	{
		Donya::Vector4x4 CS = Donya::Vector4x4::MakeScaling( OBB.size * 2.0f ); // Half size->Whole size.
		Donya::Vector4x4 CR = OBB.orientation.RequireRotationMatrix();
		Donya::Vector4x4 CT = Donya::Vector4x4::MakeTranslation( OBB.pos );
		Donya::Vector4x4 CW = ( CS * CR * CT );
		if ( applyParentDrawOffset )
		{
			Donya::Vector4x4 OFS = Donya::Vector4x4::MakeTranslation( drawOffset );
			CW *= OFS;
		}
		if ( applyParentMatrix ) { CW *= W; }
		Donya::Vector4x4 CWVP = CW * matView * matProjection;

		OBJRender( pCube.get(), CWVP, CW, color );
	};

	if ( Donya::IsShowCollision() )
	{
		setBlendMode_ALPHA( 0.5f );

		constexpr Donya::Vector4 COLOR_BODY{ 0.0f, 0.5f, 0.2f, 0.6f };

		DrawSphere( PARAM.hitBoxBody.pos, PARAM.hitBoxBody.radius, COLOR_BODY );

		constexpr Donya::Vector4 COLOR_VALID{ 1.0f, 0.5f, 0.0f, 0.6f };
		constexpr Donya::Vector4 COLOR_INVALID{ 0.4f, 0.4f, 1.0f, 0.6f };

		// Attacks collision.
		switch ( status )
		{
		case KnightAI::ActionState::WAIT:
			break;
		case KnightAI::ActionState::MOVE:
			break;
		case KnightAI::ActionState::ATTACK_EXPLOSION:
			break;
		case KnightAI::ActionState::ATTACK_SWING:
			{
				const auto &sphere = CalcAttackHitBoxSwing();
				Donya::Vector4 color = ( sphere.enable && sphere.exist ) ? COLOR_VALID : COLOR_INVALID;
				DrawSphere( sphere.pos, sphere.radius, color, /* applyParentMatrix = */ false );
			}
			break;
		case KnightAI::ActionState::ATTACK_RAID:
			{
				const auto &sphere = CalcAttackHitBoxRaid();
				Donya::Vector4 color = ( sphere.enable && sphere.exist ) ? COLOR_VALID : COLOR_INVALID;
				DrawSphere( sphere.pos, sphere.radius, color, /* applyParentMatrix = */ false );
			}
			break;
		default: break;
		}
	}

#endif // DEBUG_MODE
}

bool Knight::IsCollideAttackHitBoxes( const Donya::AABB   other, bool disableCollidingHitBoxes )
{
	if ( !KnightAI::IsAction( status ) || !other.enable || !other.exist ) { return false; }
	// else

	const Donya::OBB otherOBB
	{
		other.pos,
		other.size,
		Donya::Quaternion::Identity(),
		/* exist  = */ true,
		/* enable = */ true
	};
	return IsCollideAttackHitBoxes( otherOBB, disableCollidingHitBoxes );
}
bool Knight::IsCollideAttackHitBoxes( const Donya::OBB    other, bool disableCollidingHitBoxes )
{
	if ( !KnightAI::IsAction( status ) || !other.enable || !other.exist ) { return false; }
	// else

	bool wasCollided = false;

	switch ( status )
	{
	case KnightAI::ActionState::ATTACK_EXPLOSION:
		{

		}
		break;
	case KnightAI::ActionState::ATTACK_SWING:
		{
			const auto &wsSphere = CalcAttackHitBoxSwing();
			if ( Donya::OBB::IsHitSphere( other, wsSphere ) )
			{
				wasCollided = true;
				if ( disableCollidingHitBoxes )
				{ KnightParam::Get().HitBoxSwing().sphereF.collision.enable = false; }
			}
		}
		break;
	case KnightAI::ActionState::ATTACK_RAID:
		{
			const auto &wsSphere = CalcAttackHitBoxRaid();
			if ( Donya::OBB::IsHitSphere( other, wsSphere ) )
			{
				wasCollided = true;
				if ( disableCollidingHitBoxes )
				{ KnightParam::Get().HitBoxRaid().sphereF.collision.enable = false; }
			}
		}
		break;
	default: break;
	}

	return wasCollided;
}

static Donya::OBB MakeOBB( const Donya::AABB &AABB, const Donya::Vector3 &wsPos, const Donya::Quaternion &orientation )
{
	Donya::OBB OBB{};
	OBB.pos = AABB.pos + wsPos;
	OBB.size = AABB.size;
	OBB.orientation = orientation;
	OBB.exist = AABB.exist;
	return OBB;
}
Donya::Sphere Knight::GetBodyHitBoxes() const
{
	Donya::Sphere wsBody = KnightParam::Open().hitBoxBody;
	wsBody.pos += GetPos();
	return wsBody;
}
Donya::Sphere Knight::CalcAttackHitBoxSwing() const
{
	return KnightParam::Get().HitBoxSwing().CalcTransformedSphere
	(
		models.pAtkSwing.get(),
		CalcWorldMatrix()
	);
}
Donya::Sphere Knight::CalcAttackHitBoxRaid() const
{
	return KnightParam::Get().HitBoxRaid().CalcTransformedSphere
	(
		models.pAtkRaid.get(),
		CalcWorldMatrix()
	);
}

void Knight::ReceiveImpact()
{
	status = decltype( status )::END;
}

void Knight::SetFieldRadius( float newFieldRadius )
{
	const float bodyRadius = KnightParam::Open().stageBodyRadius;
	fieldRadius = std::max( newFieldRadius, bodyRadius );
}

void Knight::LoadModel()
{
	Donya::OutputDebugStr( "Begin Knight::LoadModel.\n" );

	loadFBX( models.pIdle.get(), GetModelPath( ModelAttribute::KnightIdle ) );
	Donya::OutputDebugStr( "Done KnightModel.Idle.\n" );
	loadFBX( models.pRunFront.get(), GetModelPath( ModelAttribute::KnightRunFront ) );
	Donya::OutputDebugStr( "Done KnightModel.RunFront.\n" );
	loadFBX( models.pAtkExpl.get(), GetModelPath( ModelAttribute::KnightAtkExplosion ) );
	Donya::OutputDebugStr( "Done KnightModel.Attack.Explosion.\n" );
	loadFBX( models.pAtkSwing.get(), GetModelPath( ModelAttribute::KnightAtkSwing ) );
	Donya::OutputDebugStr( "Done KnightModel.Attack.Swing.\n" );
	loadFBX( models.pAtkRaid.get(), GetModelPath( ModelAttribute::KnightAtkRaid ) );
	Donya::OutputDebugStr( "Done KnightModel.Attack.Raid.\n" );

	Donya::OutputDebugStr( "End Knight::LoadModel.\n" );
}

float Knight::CalcNormalizedDistance( Donya::Vector3 wsTargetPos )
{
	float distance = ( wsTargetPos - GetPos() ).Length();
	return ( ZeroEqual( fieldRadius ) )
	? fieldRadius
	: distance / fieldRadius;
}

Donya::Vector4x4 Knight::CalcWorldMatrix() const
{
	Donya::Vector4x4 S = Donya::Vector4x4::MakeScaling( KnightParam::Open().scale );
	Donya::Vector4x4 R = orientation.RequireRotationMatrix();
	Donya::Vector4x4 T = Donya::Vector4x4::MakeTranslation( GetPos() );
#if DEBUG_MODE
	if ( status == decltype( status )::END ) { R = Donya::Quaternion::Make( Donya::Vector3::Front(), ToRadian( 180.0f ) ).RequireRotationMatrix(); };
#endif // DEBUG_MODE

	return S * R * T;
}

void Knight::ChangeStatus( TargetStatus target )
{
	KnightAI::ActionState lotteryStatus = AI.GetState();
	if ( status == lotteryStatus ) { return; }
	// else

	switch ( status )
	{
	case KnightAI::ActionState::WAIT:				WaitUninit();				break;
	case KnightAI::ActionState::MOVE:				MoveUninit();				break;
	case KnightAI::ActionState::ATTACK_EXPLOSION:	AttackExplosionUninit();	break;
	case KnightAI::ActionState::ATTACK_SWING:		AttackSwingUninit();		break;
	case KnightAI::ActionState::ATTACK_RAID:		AttackRaidUninit();			break;
	default: break;
	}
	switch ( lotteryStatus )
	{
	case KnightAI::ActionState::WAIT:				WaitInit( target );				break;
	case KnightAI::ActionState::MOVE:				MoveInit( target );				break;
	case KnightAI::ActionState::ATTACK_EXPLOSION:	AttackExplosionInit( target );	break;
	case KnightAI::ActionState::ATTACK_SWING:		AttackSwingInit( target );		break;
	case KnightAI::ActionState::ATTACK_RAID:		AttackRaidInit( target );		break;
	default: break;
	}

	status = scast<KnightAI::ActionState>( lotteryStatus );
}
void Knight::UpdateCurrentStatus( TargetStatus target )
{
	switch ( status )
	{
	case KnightAI::ActionState::WAIT:				WaitUpdate( target );				break;
	case KnightAI::ActionState::MOVE:				MoveUpdate( target );				break;
	case KnightAI::ActionState::ATTACK_EXPLOSION:	AttackExplosionUpdate( target );	break;
	case KnightAI::ActionState::ATTACK_SWING:		AttackSwingUpdate( target );		break;
	case KnightAI::ActionState::ATTACK_RAID:		AttackRaidUpdate( target );			break;
	default: break;
	}
}

/*
XXXInit   : call by ChangeStatus when changing status. after XXXUninit.
XXXUpdate : call by UpdateCurrentStatus.
XXXUninit : call by ChangeStatus when changing status. before YYYInit.
*/

void Knight::WaitInit( TargetStatus target )
{
	status = KnightAI::ActionState::WAIT;

	slerpFactor = KnightParam::Get().SlerpFactor( status );

	velocity = 0.0f;

	setAnimFlame( models.pIdle.get(), 0 );
}
void Knight::WaitUpdate( TargetStatus target )
{

}
void Knight::WaitUninit()
{
	setAnimFlame( models.pIdle.get(), 0 );
}

void Knight::MoveInit( TargetStatus target )
{
	status = KnightAI::ActionState::MOVE;

	slerpFactor = KnightParam::Get().SlerpFactor( status );

	setAnimFlame( models.pRunFront.get(), 0 );
}
void Knight::MoveUpdate( TargetStatus target )
{
	const float distNear	= KnightParam::Open().targetDistNear;
	const float distFar		= KnightParam::Open().targetDistFar;
	const float nDistance	= CalcNormalizedDistance( target.pos );

	if ( distNear <= nDistance && nDistance <= distFar )
	{
		velocity = 0.0f;
		return;
	}
	// else

	const float speed = KnightParam::Get().MoveSpeed( status );
	const Donya::Vector3 front = orientation.LocalFront();

	if ( nDistance < distNear )
	{
		// Get far.
		velocity = -front * speed;
	}
	else
	{
		// Get near.
		velocity = front * speed;
	}
}
void Knight::MoveUninit()
{
	setAnimFlame( models.pRunFront.get(), 0 );
}

void Knight::AttackExplosionInit( TargetStatus target )
{
	status = KnightAI::ActionState::ATTACK_EXPLOSION;

	slerpFactor = KnightParam::Get().SlerpFactor( status );

	velocity = 0.0f;

	setAnimFlame( models.pAtkExpl.get(), 0 );
}
void Knight::AttackExplosionUpdate( TargetStatus target )
{
	// velocity = orientation.LocalFront() * KnightParam::Get().MoveSpeed( status );
}
void Knight::AttackExplosionUninit()
{
	setAnimFlame( models.pAtkExpl.get(), 0 );
}

void Knight::AttackSwingInit( TargetStatus target )
{
	status		= KnightAI::ActionState::ATTACK_SWING;;
	slerpFactor	= KnightParam::Get().SlerpFactor( status );
	velocity	= 0.0f;

	ResetCurrentSphereFN( &KnightParam::Get().HitBoxSwing() );
	setAnimFlame( models.pAtkSwing.get(), 0 );
}
void Knight::AttackSwingUpdate( TargetStatus target )
{
	auto &hitBox = KnightParam::Get().HitBoxSwing();
	hitBox.sphereF.Update();
}
void Knight::AttackSwingUninit()
{
	timer = 0;

	ResetCurrentSphereFN( &KnightParam::Get().HitBoxSwing() );
	setAnimFlame( models.pAtkSwing.get(), 0 );
}

void Knight::AttackRaidInit( TargetStatus target )
{
	status = KnightAI::ActionState::ATTACK_RAID;

	slerpFactor	= KnightParam::Get().SlerpFactor( status );
	extraOffset	= 0.0f;
	velocity	= 0.0f;

	ResetCurrentSphereFN( &KnightParam::Get().HitBoxRaid() );
	setAnimFlame( models.pAtkRaid.get(), 0 );
}
void Knight::AttackRaidUpdate( TargetStatus target )
{
#if DEBUG_MODE
	extraOffset = orientation.LocalFront() * KnightParam::Get().MoveSpeed( status );
#endif // DEBUG_MODE

	auto &hitBox = KnightParam::Get().HitBoxRaid();
	hitBox.sphereF.Update();
}
void Knight::AttackRaidUninit()
{
	pos += extraOffset;

	extraOffset = 0.0f;

	ResetCurrentSphereFN( &KnightParam::Get().HitBoxRaid() );
	setAnimFlame( models.pAtkRaid.get(), 0 );
}

void Knight::ApplyVelocity( TargetStatus target )
{
	if ( !velocity.IsZero() )
	{
		pos += velocity;
	}

	Donya::Vector3 dirToTarget = target.pos - GetPos();
	dirToTarget.Normalize();

	Donya::Quaternion destination = Donya::Quaternion::LookAt( orientation, dirToTarget ).Normalized();
	orientation = Donya::Quaternion::Slerp( orientation, destination, slerpFactor );
	orientation.Normalize();
}

void Knight::CollideToWall()
{
	const float bodyRadius = KnightParam::Open().stageBodyRadius;
	const float trueFieldRadius = fieldRadius - bodyRadius;

	constexpr Donya::Vector3 ORIGIN = Donya::Vector3::Zero();
	const Donya::Vector3 currentDistance = GetPos() - ORIGIN;
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

void Knight::UseImGui()
{
	if ( ImGui::BeginIfAllowed() )
	{
		if ( ImGui::TreeNode( "Knight.CurrentParameter" ) )
		{
			auto GetStatusName = []( KnightAI::ActionState status )->std::string
			{
				switch ( status )
				{
				case KnightAI::ActionState::WAIT:				return { "Wait" };				break;
				case KnightAI::ActionState::MOVE:				return { "Move" };				break;
				case KnightAI::ActionState::ATTACK_EXPLOSION:	return { "Attack.Explosion" };	break;
				case KnightAI::ActionState::ATTACK_SWING:		return { "Attack.Swing" };		break;
				case KnightAI::ActionState::ATTACK_RAID:		return { "Attack.Raid" };		break;
				default: break;
				}

				return { "Unexpected status" };
			};
			std::string statusCaption = "Status : " + GetStatusName( status );
			ImGui::Text( statusCaption.c_str() );
			ImGui::Text( "Timer : %d", timer );
			ImGui::Text( "SlerpFactor : %5.3f", slerpFactor );
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
			ShowVec3( "ExtraOffset", extraOffset );
			ShowQuaternion( "Orientation", orientation );

			ImGui::TreePop();
		}

		ImGui::End();
	}
}

#endif // USE_IMGUI
