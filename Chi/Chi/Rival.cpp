#include "Rival.h"

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

Donya::Sphere RivalParam::SphereFrameWithName::CalcTransformedSphere( skinned_mesh *pMesh, const Donya::Vector4x4 &parentSpaceMatrix ) const
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

RivalParam::RivalParam() :
	m()
{}
RivalParam::~RivalParam() = default;

void RivalParam::Init()
{
	LoadParameter();
}
void RivalParam::Uninit()
{
	// No op.
}

float RivalParam::MoveSpeed( RivalAI::ActionState status )
{
	switch ( status )
	{
	case RivalAI::ActionState::WAIT:				return 0.0f;				// break;
	case RivalAI::ActionState::MOVE:				return m.move.moveSpeed;	// break;
	case RivalAI::ActionState::ATTACK_BARRAGE:		return m.barrage.moveSpeed;	// break;
	case RivalAI::ActionState::ATTACK_LINE:			return m.line.moveSpeed;	// break;
	case RivalAI::ActionState::ATTACK_RAID:			return m.raid.moveSpeed;	// break;
	case RivalAI::ActionState::ATTACK_RUSH:			return m.rush.moveSpeed;	// break;
	default: break;
	}

	return NULL;
}
float RivalParam::SlerpFactor( RivalAI::ActionState status )
{
	switch ( status )
	{
	case RivalAI::ActionState::WAIT:				return m.idleSlerpFactor;		// break;
	case RivalAI::ActionState::MOVE:				return m.move.slerpFactor;		// break;
	case RivalAI::ActionState::ATTACK_BARRAGE:		return m.barrage.slerpFactor;	// break;
	case RivalAI::ActionState::ATTACK_LINE:			return m.line.slerpFactor;		// break;
	case RivalAI::ActionState::ATTACK_RAID:			return m.raid.slerpFactor;		// break;
	case RivalAI::ActionState::ATTACK_RUSH:			return m.rush.slerpFactor;		// break;
	default: break;
	}

	return NULL;
}

RivalParam::Member RivalParam::Open()
{
	return Get().Content();
}

void RivalParam::LoadParameter( bool isBinary )
{
	Donya::Serializer::Extension ext = ( isBinary )
	? Donya::Serializer::Extension::BINARY
	: Donya::Serializer::Extension::JSON;
	std::string filePath = GenerateSerializePath( SERIAL_ID, ext );

	Donya::Serializer seria;
	seria.Load( ext, filePath.c_str(), SERIAL_ID, *this );
}

#if USE_IMGUI

void RivalParam::SaveParameter()
{
	Donya::Serializer::Extension bin  = Donya::Serializer::Extension::BINARY;
	Donya::Serializer::Extension json = Donya::Serializer::Extension::JSON;
	std::string binPath  = GenerateSerializePath( SERIAL_ID, bin  );
	std::string jsonPath = GenerateSerializePath( SERIAL_ID, json );

	Donya::Serializer seria;
	seria.Save( bin,  binPath.c_str(),  SERIAL_ID, *this );
	seria.Save( json, jsonPath.c_str(), SERIAL_ID, *this );
}

void RivalParam::UseImGui()
{
	if ( ImGui::BeginIfAllowed() )
	{
		if ( ImGui::TreeNode( "Rival.AdjustData" ) )
		{
			ImGui::SliderFloat( "Scale", &m.scale, 0.0f, 8.0f );
			ImGui::DragFloat( "BodyRadiusToStage", &m.stageBodyRadius );
			ImGui::Text( "" );

			ImGui::SliderFloat( "Distance.ThinkTo\"Near\"", &m.targetDistNear, 0.01f, 1.0f );
			ImGui::SliderFloat( "Distance.ThinkTo\"Far\"",  &m.targetDistFar,  0.0f, 0.99f );
			ImGui::Text( "" );

			ImGui::DragFloat( "MoveSpeed.\"Move\"State",			&m.move.moveSpeed );
			ImGui::DragFloat( "MoveSpeed.\"Atk.Barrage\"State",		&m.barrage.moveSpeed );
			ImGui::DragFloat( "MoveSpeed.\"Atk.Line\"State",		&m.line.moveSpeed );
			ImGui::DragFloat( "MoveSpeed.\"Atk.Raid\"State",		&m.raid.moveSpeed );
			ImGui::DragFloat( "MoveSpeed.\"Atk.Rush\"State",		&m.rush.moveSpeed );
			ImGui::Text( "" );

			ImGui::SliderFloat( "SlerpFactor.\"Idle\"State",			&m.idleSlerpFactor,		0.0f, 1.0f );
			ImGui::SliderFloat( "SlerpFactor.\"Move\"State",			&m.move.slerpFactor,	0.0f, 1.0f );
			ImGui::SliderFloat( "SlerpFactor.\"Atk.Barrage\"State",		&m.barrage.slerpFactor,	0.0f, 1.0f );
			ImGui::SliderFloat( "SlerpFactor.\"Atk.Line\"State",		&m.line.slerpFactor,	0.0f, 1.0f );
			ImGui::SliderFloat( "SlerpFactor.\"Atk.Raid\"State",		&m.raid.slerpFactor,	0.0f, 1.0f );
			ImGui::SliderFloat( "SlerpFactor.\"Atk.Rush\"State",		&m.rush.slerpFactor,	0.0f, 1.0f );
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
				
				ShowAABB( "Body.HurtBox", &m.hitBoxBody );
				ImGui::Text( "" );

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

static void ResetCurrentSphereF( Donya::SphereFrame *pSphereF )
{
	pSphereF->currentFrame = 0;
	pSphereF->collision.enable = true;
}
static void ResetCurrentSphereFN( RivalParam::SphereFrameWithName *pSphereFN )
{
	pSphereFN->sphereF.currentFrame = 0;
	pSphereFN->sphereF.collision.enable = true;
}

Rival::Rival() :
	status( RivalAI::ActionState::WAIT ), extraStatus( ExtraState::NONE ),
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
		&models.pRun,
		&models.pBreak,
		&models.pLeave,
		&models.pDefeat,
		&models.pAtkBarrage,
		&models.pAtkLine,
		&models.pAtkRaid,
		&models.pAtkRushPre,
		&models.pAtkRushRaid,
		&models.pAtkRushPost,
	};
	for ( auto &it : modelRefs )
	{
		InitializeModel( it );
	}
}
Rival::~Rival() = default;

void Rival::Init( int stageNumber )
{
	RivalParam::Get().Init();

	LoadModel();

	status	= RivalAI::ActionState::WAIT;
	pos		= RivalParam::Open().initPos;

	SetFieldRadius( 0.0f ); // Set to body's radius.

	Donya::Vector3 dirToOrigin = Donya::Vector3::Zero() - pos;
	dirToOrigin.y = 0.0f;
	orientation = Donya::Quaternion::LookAt( orientation, dirToOrigin );

	AI.Init();
}
void Rival::Uninit()
{
	RivalParam::Get().Uninit();
}

void Rival::Update( TargetStatus target )
{
#if USE_IMGUI

	RivalParam::Get().UseImGui();
	UseImGui();

#endif // USE_IMGUI

	AI.Update();

	ChangeStatus( target );
	UpdateCurrentStatus( target );

	ApplyVelocity( target );
	CollideToWall();
}

void Rival::Draw( fbx_shader &HLSL, const Donya::Vector4x4 &matView, const Donya::Vector4x4 &matProjection )
{
	const auto &PARAM = RivalParam::Open();

	const Donya::Vector3 drawOffset = PARAM.drawOffset;
	const Donya::Vector4x4 DRAW_OFFSET = Donya::Vector4x4::MakeTranslation( drawOffset );

	Donya::Vector4x4 W = CalcWorldMatrix() * DRAW_OFFSET;
	Donya::Vector4x4 WVP = W * matView * matProjection;

	if ( extraStatus != ExtraState::NONE )
	{
		switch ( extraStatus )
		{
		case ExtraState::BREAK:
			FBXRender( models.pBreak.get(), HLSL, WVP, W );
			break;
		case ExtraState::LEAVE:
			FBXRender( models.pBreak.get(), HLSL, WVP, W );
			break;
		case ExtraState::DEFEAT:
			FBXRender( models.pDefeat.get(), HLSL, WVP, W );
			break;
		default: break;
		}
	}
	else
	{
		switch ( status )
		{
		case RivalAI::ActionState::WAIT:
			FBXRender( models.pIdle.get(), HLSL, WVP, W );
			break;
		case RivalAI::ActionState::MOVE:
			FBXRender( models.pRun.get(), HLSL, WVP, W );
			break;
		case RivalAI::ActionState::ATTACK_BARRAGE:
			FBXRender( models.pAtkBarrage.get(), HLSL, WVP, W );
			break;
		case RivalAI::ActionState::ATTACK_LINE:
			FBXRender( models.pAtkLine.get(), HLSL, WVP, W );
			break;
		case RivalAI::ActionState::ATTACK_RAID:
			FBXRender( models.pAtkRaid.get(), HLSL, WVP, W );
			break;
		case RivalAI::ActionState::ATTACK_RUSH:
			FBXRender( models.pAtkRushPre.get(), HLSL, WVP, W );
			break;
		default: break;
		}
	}

	// For debug, helpers of drawing primitive. and drawing collisions.
#if DEBUG_MODE

	auto GenerateCube	= []()->std::shared_ptr<static_mesh>
	{
		std::shared_ptr<static_mesh> tmpCube = std::make_shared<static_mesh>();
		createCube( tmpCube.get() );
		return tmpCube;
	};
	auto GenerateSphere	= []()->std::shared_ptr<static_mesh>
	{
		std::shared_ptr<static_mesh> tmpSphere = std::make_shared<static_mesh>();
		createSphere( tmpSphere.get(), 6, 12 );
		return tmpSphere;
	};
	static std::shared_ptr<static_mesh> pCube = GenerateCube();
	static std::shared_ptr<static_mesh> pSphere = GenerateSphere();

	// Except DRAW_OFFSET matrix. because I wanna draw collisions to actual position.
	W = CalcWorldMatrix();

	auto DrawCube	= [&]( const Donya::Vector3 &cubeOffset, const Donya::Vector3 &cubeScale, const Donya::Quaternion &orientation, const Donya::Vector4 &color, bool applyParentMatrix = true, bool applyParentDrawOffset = false )
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
	auto DrawSphere	= [&]( const Donya::Vector3 &sphereOffset, float sphereScale, const Donya::Vector4 &color, bool applyParentMatrix = true, bool applyParentDrawOffset = false )
	{
		Donya::Vector4x4 CS = Donya::Vector4x4::MakeScaling( sphereScale * 2.0f ); // Half size->Whole size.
		Donya::Vector4x4 CT = Donya::Vector4x4::MakeTranslation( sphereOffset );
		Donya::Vector4x4 CW = ( CS * CT );
		if ( applyParentMatrix ) { CW *= W; }
		if ( applyParentDrawOffset ) { CW *= DRAW_OFFSET; }
		Donya::Vector4x4 CWVP = CW * matView * matProjection;

		OBJRender( pSphere.get(), CWVP, CW, color );
	};
	auto DrawAABB	= [&]( const Donya::AABB &AABB, const Donya::Vector4 &color, bool applyParentMatrix = true, bool applyParentDrawOffset = false )
	{
		Donya::Vector4x4 CS = Donya::Vector4x4::MakeScaling( AABB.size * 2.0f ); // Half size->Whole size.
		Donya::Vector4x4 CT = Donya::Vector4x4::MakeTranslation( AABB.pos );
		Donya::Vector4x4 CW = ( CS * CT );
		if ( applyParentDrawOffset )
		{
			Donya::Vector4x4 OFS = Donya::Vector4x4::MakeTranslation( drawOffset );
			CW *= OFS;
		}
		if ( applyParentMatrix ) { CW *= W; }
		Donya::Vector4x4 CWVP = CW * matView * matProjection;

		OBJRender( pCube.get(), CWVP, CW, color );
	};
	auto DrawOBB	= [&]( const Donya::OBB &OBB, const Donya::Vector4 &color, bool applyParentMatrix = true, bool applyParentDrawOffset = false )
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

		DrawAABB( PARAM.hitBoxBody, COLOR_BODY );

		constexpr Donya::Vector4 COLOR_VALID{ 1.0f, 0.5f, 0.0f, 0.6f };
		constexpr Donya::Vector4 COLOR_INVALID{ 0.4f, 0.4f, 1.0f, 0.6f };

		// Attacks collision.
		switch ( status )
		{
		case RivalAI::ActionState::WAIT:
			break;
		case RivalAI::ActionState::MOVE:
			break;
		case RivalAI::ActionState::ATTACK_BARRAGE:
			break;
		case RivalAI::ActionState::ATTACK_LINE:
			break;
		case RivalAI::ActionState::ATTACK_RAID:
			break;
		case RivalAI::ActionState::ATTACK_RUSH:
			break;
		default: break;
		}
	}

#endif // DEBUG_MODE
}

bool Rival::IsCollideAttackHitBoxes( const Donya::AABB   other, bool disableCollidingHitBoxes )
{
	if ( !RivalAI::IsAction( status ) || !other.enable || !other.exist ) { return false; }
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
bool Rival::IsCollideAttackHitBoxes( const Donya::OBB    other, bool disableCollidingHitBoxes )
{
	if ( !RivalAI::IsAction( status ) || !other.enable || !other.exist ) { return false; }
	// else

	bool wasCollided = false;

	switch ( status )
	{
	case RivalAI::ActionState::ATTACK_BARRAGE:
		break;
	case RivalAI::ActionState::ATTACK_LINE:
		break;
	case RivalAI::ActionState::ATTACK_RAID:
		break;
	case RivalAI::ActionState::ATTACK_RUSH:
		break;
	default: break;
	}

	return wasCollided;
}

static Donya::OBB MakeOBB( const Donya::AABB &AABB, const Donya::Vector3 &wsPos, const Donya::Quaternion &orientation )
{
	Donya::OBB OBB{};
	OBB.pos				= AABB.pos + wsPos;
	OBB.size			= AABB.size;
	OBB.orientation		= orientation;
	OBB.exist			= AABB.exist;
	return OBB;
}
Donya::AABB Rival::GetBodyHitBox() const
{
	Donya::AABB wsBody = RivalParam::Open().hitBoxBody;
	wsBody.pos += GetPos();
	return wsBody;
}
void Rival::WasDefended()
{
	if ( status == RivalAI::ActionState::ATTACK_RUSH )
	{
		status = RivalAI::ActionState::WAIT;
		extraStatus = ExtraState::BREAK;
	}
}
void Rival::ReceiveImpact()
{
	status = decltype( status )::END;
}

void Rival::SetFieldRadius( float newFieldRadius )
{
	const float bodyRadius = RivalParam::Open().stageBodyRadius;
	fieldRadius = std::max( newFieldRadius, bodyRadius );
}

void Rival::LoadModel()
{
	Donya::OutputDebugStr( "Begin Rival::LoadModel.\n" );

	loadFBX( models.pIdle.get(), GetModelPath( ModelAttribute::RivalIdle ) );
	Donya::OutputDebugStr( "Done RivalModel.Idle.\n" );
	loadFBX( models.pRun.get(), GetModelPath( ModelAttribute::RivalRun ) );
	Donya::OutputDebugStr( "Done RivalModel.Run.\n" );
	loadFBX( models.pBreak.get(), GetModelPath( ModelAttribute::RivalBreak ) );
	Donya::OutputDebugStr( "Done RivalModel.Break.\n" );
	loadFBX( models.pDefeat.get(), GetModelPath( ModelAttribute::RivalDefeat ) );
	Donya::OutputDebugStr( "Done RivalModel.Defeat.\n" );
	loadFBX( models.pAtkBarrage.get(), GetModelPath( ModelAttribute::RivalAtkBarrage ) );
	Donya::OutputDebugStr( "Done RivalModel.Attack.Barrage.\n" );
	loadFBX( models.pAtkLine.get(), GetModelPath( ModelAttribute::RivalAtkLine ) );
	Donya::OutputDebugStr( "Done RivalModel.Attack.Line.\n" );
	loadFBX( models.pAtkRaid.get(), GetModelPath( ModelAttribute::RivalAtkRaid ) );
	Donya::OutputDebugStr( "Done RivalModel.Attack.Raid.\n" );
	loadFBX( models.pAtkRushPre.get(), GetModelPath( ModelAttribute::RivalAtkRushPre ) );
	Donya::OutputDebugStr( "Done RivalModel.Attack.Rush.Pre.\n" );
	loadFBX( models.pAtkRushRaid.get(), GetModelPath( ModelAttribute::RivalAtkRushRaid ) );
	Donya::OutputDebugStr( "Done RivalModel.Attack.Rush.Raid.\n" );
	loadFBX( models.pAtkRushPost.get(), GetModelPath( ModelAttribute::RivalAtkRushPost ) );
	Donya::OutputDebugStr( "Done RivalModel.Attack.Rush.Post.\n" );

	Donya::OutputDebugStr( "End Rival::LoadModel.\n" );
}

float Rival::CalcNormalizedDistance( Donya::Vector3 wsTargetPos )
{
	float distance = ( wsTargetPos - GetPos() ).Length();
	return ( ZeroEqual( fieldRadius ) )
	? fieldRadius
	: distance / ( fieldRadius * 2.0f );
}

Donya::Vector4x4 Rival::CalcWorldMatrix() const
{
	Donya::Vector4x4 S = Donya::Vector4x4::MakeScaling( RivalParam::Open().scale );
	Donya::Vector4x4 R = orientation.RequireRotationMatrix();
	Donya::Vector4x4 T = Donya::Vector4x4::MakeTranslation( GetPos() );
#if DEBUG_MODE
	if ( status == decltype( status )::END ) { R = Donya::Quaternion::Make( Donya::Vector3::Front(), ToRadian( 180.0f ) ).RequireRotationMatrix(); };
#endif // DEBUG_MODE

	return S * R * T;
}

void Rival::ChangeStatus( TargetStatus target )
{
	RivalAI::ActionState lotteryStatus = AI.GetState();
	if ( status == lotteryStatus ) { return; }
	// else

	switch ( status )
	{
	case RivalAI::ActionState::WAIT:				WaitUninit();			break;
	case RivalAI::ActionState::MOVE:				MoveUninit();			break;
	case RivalAI::ActionState::ATTACK_BARRAGE:		AttackBarrageUninit();	break;
	case RivalAI::ActionState::ATTACK_LINE:			AttackLineUninit();		break;
	case RivalAI::ActionState::ATTACK_RAID:			AttackRaidUninit();		break;
	case RivalAI::ActionState::ATTACK_RUSH:			AttackRushUninit();		break;
	default: break;
	}
	switch ( lotteryStatus )
	{
	case RivalAI::ActionState::WAIT:				WaitInit( target );				break;
	case RivalAI::ActionState::MOVE:				MoveInit( target );				break;
	case RivalAI::ActionState::ATTACK_BARRAGE:		AttackBarrageInit( target );	break;
	case RivalAI::ActionState::ATTACK_LINE:			AttackLineInit( target );		break;
	case RivalAI::ActionState::ATTACK_RAID:			AttackRaidInit( target );		break;
	case RivalAI::ActionState::ATTACK_RUSH:			AttackRushInit( target );		break;
	default: break;
	}

	status = scast<RivalAI::ActionState>( lotteryStatus );
}
void Rival::UpdateCurrentStatus( TargetStatus target )
{
	switch ( status )
	{
	case RivalAI::ActionState::WAIT:				WaitUpdate( target );			break;
	case RivalAI::ActionState::MOVE:				MoveUpdate( target );			break;
	case RivalAI::ActionState::ATTACK_BARRAGE:		AttackBarrageUpdate( target );	break;
	case RivalAI::ActionState::ATTACK_LINE:			AttackLineUpdate( target );		break;
	case RivalAI::ActionState::ATTACK_RAID:			AttackRaidUpdate( target );		break;
	case RivalAI::ActionState::ATTACK_RUSH:			AttackRushUpdate( target );		break;
	default: break;
	}
}

/*
XXXInit   : call by ChangeStatus when changing status. after XXXUninit.
XXXUpdate : call by UpdateCurrentStatus.
XXXUninit : call by ChangeStatus when changing status. before YYYInit.
*/

void Rival::WaitInit( TargetStatus target )
{
	status		= RivalAI::ActionState::WAIT;
	slerpFactor	= RivalParam::Get().SlerpFactor( status );
	velocity	= 0.0f;

	setAnimFlame( models.pIdle.get(), 0 );
}
void Rival::WaitUpdate( TargetStatus target )
{
	// No op.
}
void Rival::WaitUninit()
{
	setAnimFlame( models.pIdle.get(), 0 );
}

void Rival::MoveInit( TargetStatus target )
{
	status		= RivalAI::ActionState::MOVE;
	slerpFactor	= RivalParam::Get().SlerpFactor( status );

	setAnimFlame( models.pRun.get(), 0 );
}
void Rival::MoveUpdate( TargetStatus target )
{
	const float distNear	= RivalParam::Open().targetDistNear;
	const float distFar		= RivalParam::Open().targetDistFar;
	const float nDistance	= CalcNormalizedDistance( target.pos );

	if ( distNear <= nDistance && nDistance <= distFar )
	{
		const auto IdleState = RivalAI::ActionState::WAIT;
		status = IdleState;
		AI.OverwriteState( IdleState );

		velocity = 0.0f;
		return;
	}
	// else

	const float speed = RivalParam::Get().MoveSpeed( status );
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
void Rival::MoveUninit()
{
	setAnimFlame( models.pRun.get(), 0 );
}

void Rival::AttackBarrageInit( TargetStatus target )
{
	status		= RivalAI::ActionState::ATTACK_BARRAGE;
	slerpFactor	= RivalParam::Get().SlerpFactor( status );
	velocity	= 0.0f;

	setAnimFlame( models.pAtkBarrage.get(), 0 );
}
void Rival::AttackBarrageUpdate( TargetStatus target )
{
	
}
void Rival::AttackBarrageUninit()
{
	setAnimFlame( models.pAtkBarrage.get(), 0 );
}

void Rival::AttackLineInit( TargetStatus target )
{
	status		= RivalAI::ActionState::ATTACK_LINE;
	slerpFactor	= RivalParam::Get().SlerpFactor( status );
	velocity	= 0.0f;

	setAnimFlame( models.pAtkLine.get(), 0 );
}
void Rival::AttackLineUpdate( TargetStatus target )
{
	
}
void Rival::AttackLineUninit()
{
	setAnimFlame( models.pAtkLine.get(), 0 );
}

void Rival::AttackRaidInit( TargetStatus target )
{
	status		= RivalAI::ActionState::ATTACK_RAID;
	timer		= 0;
	slerpFactor	= RivalParam::Get().SlerpFactor( status );
	extraOffset	= 0.0f;
	velocity	= 0.0f;

	// ResetCurrentSphereFN( &RivalParam::Get().HitBoxRaid() );
	setAnimFlame( models.pAtkRaid.get(), 0 );
}
void Rival::AttackRaidUpdate( TargetStatus target )
{
	/*
	const int START_TIME	= RivalParam::Open().raidJumpStartFrame;
	const int LANDING_TIME	= RivalParam::Open().raidJumpLastFrame;
	
	auto ApplyExtraOffset	= [&]()->void
	{
		// Apply movement and reset extraOffset.
		pos += extraOffset;
		extraOffset = 0.0f;
	};

	timer++;

	if ( timer <= START_TIME )
	{
		// Before jump. move to front slowly.

		extraOffset += orientation.LocalFront() * RivalParam::Get().MoveSpeed( status );

		if ( timer == START_TIME )
		{
			ApplyExtraOffset();
		}
	}
	else
	if ( timer <= START_TIME + LANDING_TIME )
	{
		// Before landing. move to front with easing.
		
		float percent	= scast<float>( timer - START_TIME ) / scast<float>( LANDING_TIME );
		float ease		= Donya::Easing::Ease
		(
			scast<Donya::Easing::Kind>( RivalParam::Open().raidEaseKind ),
			scast<Donya::Easing::Type>( RivalParam::Open().raidEaseType ),
			percent
		);

		extraOffset = orientation.LocalFront() * ( RivalParam::Open().raidJumpDistance * ease );

		if ( timer == START_TIME + LANDING_TIME )
		{
			ApplyExtraOffset();
		}
	}
	else
	{
		// No op.
	}

	auto &hitBox = RivalParam::Get().HitBoxRaid();
	hitBox.sphereF.Update();
	*/
}
void Rival::AttackRaidUninit()
{
	timer		= 0;
	extraOffset	= 0.0f;
	velocity	= 0.0f;

	// ResetCurrentSphereFN( &RivalParam::Get().HitBoxRaid() );
	setAnimFlame( models.pAtkRaid.get(), 0 );
}

void Rival::AttackRushInit( TargetStatus target )
{
	status		= RivalAI::ActionState::ATTACK_RUSH;
	slerpFactor	= RivalParam::Get().SlerpFactor( status );
	velocity	= 0.0f;

	setAnimFlame( models.pAtkRushPre.get(), 0 );
	setAnimFlame( models.pAtkRushRaid.get(), 0 );
	setAnimFlame( models.pAtkRushPost.get(), 0 );
}
void Rival::AttackRushUpdate( TargetStatus target )
{
	
}
void Rival::AttackRushUninit()
{
	setAnimFlame( models.pAtkRushPre.get(), 0 );
	setAnimFlame( models.pAtkRushRaid.get(), 0 );
	setAnimFlame( models.pAtkRushPost.get(), 0 );
}

void Rival::ApplyVelocity( TargetStatus target )
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

void Rival::CollideToWall()
{
	const float bodyRadius = RivalParam::Open().stageBodyRadius;
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

void Rival::UseImGui()
{
	if ( ImGui::BeginIfAllowed() )
	{
		if ( ImGui::TreeNode( "Rival.CurrentParameter" ) )
		{
			auto GetStatusName = []( RivalAI::ActionState status )->std::string
			{
				switch ( status )
				{
				case RivalAI::ActionState::WAIT:				return { "Wait" };				break;
				case RivalAI::ActionState::MOVE:				return { "Move" };				break;
				case RivalAI::ActionState::ATTACK_BARRAGE:		return { "Attack.Barrage" };	break;
				case RivalAI::ActionState::ATTACK_LINE:			return { "Attack.Line" };		break;
				case RivalAI::ActionState::ATTACK_RAID:			return { "Attack.Raid" };		break;
				case RivalAI::ActionState::ATTACK_RUSH:			return { "Attack.Rush" };		break;
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
