#include "Rival.h"

#include <array>

#include "Donya/Easing.h"
#include "Donya/FilePath.h"
#include "Donya/Random.h"
#include "Donya/Sound.h"
#include "Donya/Useful.h"		// For IsShowCollision().

#include "gameLib.h"
#include "gameLibFunctions.h"	// Use FBXRender().
#include "skinned_mesh.h"

#include "Effect.h"

#undef max
#undef min

#define scast static_cast

static Donya::Vector3 CalcBoneTransformedPosition( skinned_mesh *pMesh, const std::string &meshName, const Donya::Vector4x4 &parentSpaceMatrix, const Donya::Vector3 &transformPos )
{
	// To local space from mesh's mesh space.
	Donya::Vector4x4 meshMatrix{};
	{
		// Input:offset in local-space. Output:transformed position.
		Donya::Vector3 outputPos = transformPos;
		bool succeeded = calcTransformedPosBySpecifyMesh( pMesh, outputPos, meshName );
		if ( !succeeded )
		{
			_ASSERT_EXPR( 0, L"Error : specified mesh name was not found !" );
			return Donya::Vector3::Zero();
		}
		Donya::Vector4x4 T = Donya::Vector4x4::MakeTranslation( outputPos );

		// Convert to world space from local space.
		meshMatrix = T * parentSpaceMatrix;
	}

	Donya::Vector3 resultPos
	{
		meshMatrix._41,
		meshMatrix._42,
		meshMatrix._43
	};
	return resultPos;
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
	case RivalAI::ActionState::MOVE_GET_NEAR:		return m.move.moveSpeed;	// break;
	case RivalAI::ActionState::MOVE_GET_FAR:		return m.move.moveSpeed;	// break;
	case RivalAI::ActionState::MOVE_SIDE:			return m.move.moveSpeed;	// break;
	case RivalAI::ActionState::MOVE_AIM_SIDE:		return m.move.moveSpeed;	// break;
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
	case RivalAI::ActionState::MOVE_GET_NEAR:		return m.move.slerpFactor;		// break;
	case RivalAI::ActionState::MOVE_GET_FAR:		return m.move.slerpFactor;		// break;
	case RivalAI::ActionState::MOVE_SIDE:			return m.move.slerpFactor;		// break;
	case RivalAI::ActionState::MOVE_AIM_SIDE:		return m.move.slerpFactor;		// break;
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

void RivalParam::UpdateBarrage( int elapsedTime )
{
	m.barrage.Update( elapsedTime );
}
void RivalParam::ResetBarrage()
{
	for ( auto &it : m.barrage.collisions )
	{
		it.currentFrame		= 0;
		it.collision.enable	= true;
	}
	
	for ( auto &it : m.barrage.walkTimings )
	{
		it.current		= 0;
		it.withinFrame	= false;
	}
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
		#pragma region CommonSettings

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
			// ImGui::DragFloat( "MoveSpeed.\"Atk.Rush\"State",		&m.rush.moveSpeed ); // Unused.
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

		// CommonSettings
		#pragma endregion

			if ( ImGui::TreeNode( "Defeat" ) )
			{
				ImGui::DragInt( "Motion.Length(Frame)", &m.defeat.motionLength );
				ImGui::SliderFloat( "AfterMotion.HideSpeed", &m.defeat.hideSpeed, 0.00001f, 1.0f );

				ImGui::TreePop();
			}

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
				auto ShowSphereFs = [&ShowSphereF]( const std::string &prefix, std::vector<Donya::SphereFrame> *pSphereFs )
				{
					if ( ImGui::Button( ( prefix + ".Append" ).c_str() ) )
					{
						pSphereFs->push_back( {} );
					}
					if ( !pSphereFs->empty() && ImGui::Button( ( prefix + ".PopBack" ).c_str() ) )
					{
						pSphereFs->pop_back();
					}

					const size_t COUNT = pSphereFs->size();
					for ( size_t i = 0; i < COUNT; ++i )
					{
						ShowSphereF( prefix + "[" + std::to_string( i ) + "]", &( *pSphereFs )[i] );
					}
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
		
				if ( ImGui::TreeNode( "Body" ) )
				{
					ShowAABB( "Body.HurtBox", &m.hitBoxBody );
					ImGui::TreePop();
				}

				if ( ImGui::TreeNode( "Attack.Barrage" ) )
				{
					static std::array<char, 512U> meshNameBuffer{};
					ImGui::InputText( "MeshName", meshNameBuffer.data(), meshNameBuffer.size() );
					if ( ImGui::Button( "Apply.MeshName" ) )
					{
						m.barrage.collideMeshName = meshNameBuffer.data();
					}
					ImGui::Text( "" );

					ShowSphereFs( "Barrage", &m.barrage.collisions );

					// WalkTiming.
					{
						auto &timings = m.barrage.walkTimings;

						if ( ImGui::Button( "WalkTiming.Append" ) )
						{
							timings.push_back( {} );
						}
						if ( !timings.empty() && ImGui::Button( "WalkTiming.PopBack" ) )
						{
							timings.pop_back();
						}

						const size_t COUNT = timings.size();
						for ( size_t i = 0; i < COUNT; ++i )
						{
							ImGui::DragInt  ( ( "[" + std::to_string( i ) + "].StartFrame" ).c_str(), &timings[i].start );
							ImGui::DragInt  ( ( "[" + std::to_string( i ) + "].LastFrame" ).c_str(),  &timings[i].last  );
							ImGui::DragFloat( ( "[" + std::to_string( i ) + "].WalkSpeed" ).c_str(),  &timings[i].speed );
							ImGui::Text( "" );
						}
					}

					ImGui::TreePop();
				}

				if ( ImGui::TreeNode( "Attack.Line" ) )
				{
					ImGui::DragInt( "Line.GenerateTiming(Frame)", &m.line.generateFrame );

					ImGui::TreePop();
				}
				
				if ( ImGui::TreeNode( "Attack.Raid" ) )
				{
					// Easing parameter.
					{
						using namespace Donya;
						constexpr int KIND_COUNT = Easing::GetKindCount();
						constexpr int TYPE_COUNT = Easing::GetTypeCount();
						ImGui::SliderInt( "Jump.EasingKind", &m.raid.easingKind, 0, KIND_COUNT - 1 );
						ImGui::SliderInt( "Jump.EasingType", &m.raid.easingType, 0, TYPE_COUNT - 1 );
						std::string strKind = Easing::KindName( m.raid.easingKind );
						std::string strType = Easing::TypeName( m.raid.easingType );
						ImGui::Text( ( "Easing : " + strKind + "." + strType ).c_str() );
					}

					ImGui::DragInt( "Jump.StartFrame", &m.raid.jumpStartFrame );
					ImGui::DragInt( "Jump.LastFrame",  &m.raid.jumpLastFrame  );
					ImGui::DragFloat( "Jump.Length(Distance)", &m.raid.jumpDistance );

					static std::array<char, 512U> meshNameBuffer{};
					ShowSphereFN( "HitBox", &m.raid.hitBox, &meshNameBuffer );

					ImGui::TreePop();
				}

				if ( ImGui::TreeNode( "Attack.Rush" ) )
				{
					ImGui::DragInt( "Rush.WaitLength(Frame)", &m.rush.waitLength );
					ImGui::DragFloat( "Rush.Speed", &m.rush.rushSpeed );
					ImGui::DragInt( "Slash.StopAnimeFrame", &m.rush.slashStopAnimeFrame );

					ShowSphere( "Slash.TriggerRange", &m.rush.slashOccurRange );

					ShowSphereF( "HitBox", &m.rush.hitBox );

					ImGui::TreePop();
				}

				if ( ImGui::TreeNode( "Attack.Break" ) )
				{
					ImGui::DragInt( "Break.WholeLength(Frame)", &m.breakdown.breakFrame );
					ImGui::DragInt( "Leave.WholeLength(Frame)", &m.breakdown.leaveWholeFrame );

					// Easing parameter.
					{
						using namespace Donya;
						constexpr int KIND_COUNT = Easing::GetKindCount();
						constexpr int TYPE_COUNT = Easing::GetTypeCount();
						ImGui::SliderInt( "Leave.EasingKind", &m.breakdown.leaveEasingKind, 0, KIND_COUNT - 1 );
						ImGui::SliderInt( "Leave.EasingType", &m.breakdown.leaveEasingType, 0, TYPE_COUNT - 1 );
						std::string strKind = Easing::KindName( m.breakdown.leaveEasingKind );
						std::string strType = Easing::TypeName( m.breakdown.leaveEasingType );
						ImGui::Text( ( "Easing : " + strKind + "." + strType ).c_str() );
					}

					ImGui::DragInt( "Leave.MovingFrame", &m.breakdown.leaveMoveFrame );
					ImGui::DragFloat( "Leave.Length(Distance)", &m.breakdown.leaveDistance );

					ImGui::TreePop();
				}

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
static void ResetCurrentSphereFs( std::vector<Donya::SphereFrame> *pSphereFs )
{
	const size_t COUNT = pSphereFs->size();
	for ( size_t i = 0; i < COUNT; ++i )
	{
		pSphereFs->at( i ).currentFrame = 0;
		pSphereFs->at( i ).collision.enable = true;
	}
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
	moveSign(), fieldRadius(), slerpFactor( 1.0f ),
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
		&models.pRunLeft,
		&models.pRunRight,
		&models.pBreak,
		&models.pLeave,
		&models.pDefeat,
		&models.pAtkBarrage,
		&models.pAtkLine,
		&models.pAtkRaid,
		&models.pAtkRushWait,
		&models.pAtkRushSlash,
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
	UseImGui( CalcNormalizedDistance( target.pos ) );

#endif // USE_IMGUI

	AI.Update( CalcNormalizedDistance( target.pos ) );

	ChangeStatus( target );
	UpdateCurrentStatus( target );

	ApplyVelocity( target );
	CollideToWall();
}

void Rival::Draw( fbx_shader &HLSL, const Donya::Vector4x4 &matView, const Donya::Vector4x4 &matProjection, float animeAccel )
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
		case ExtraState::RUSH_WAIT:
			FBXRender( models.pAtkRushWait.get(), HLSL, WVP, W, animeAccel );
			break;
		case ExtraState::RUSH_RUN:
			FBXRender( models.pAtkRushSlash.get(), HLSL, WVP, W, animeAccel );
			break;
		case ExtraState::RUSH_SLASH:
			FBXRender( models.pAtkRushSlash.get(), HLSL, WVP, W, animeAccel );
			break;
		case ExtraState::BREAK:
			FBXRender( models.pBreak.get(), HLSL, WVP, W, animeAccel );
			break;
		case ExtraState::LEAVE:
			FBXRender( models.pLeave.get(), HLSL, WVP, W, animeAccel );
			break;
		case ExtraState::DEFEAT:
			{
				const int MOTION_LENGTH = RivalParam::Open().defeat.motionLength;
				int timeDiff = timer - MOTION_LENGTH;
				
				float drawAlpha = 1.0f;
				if ( 0 < timeDiff )
				{
					drawAlpha -= RivalParam::Open().defeat.hideSpeed * timeDiff;
					drawAlpha =  std::max( 0.0f, drawAlpha );
				}
				FBXRender( models.pDefeat.get(), HLSL, WVP, W, animeAccel, /* is_animation = */ true, { 1.0f, 1.0f, 1.0f, drawAlpha } );
			}
			break;
		default: break;
		}
	}
	else
	{
		switch ( status )
		{
		case RivalAI::ActionState::WAIT:
			FBXRender( models.pIdle.get(), HLSL, WVP, W, animeAccel );
			break;
		case RivalAI::ActionState::MOVE_GET_NEAR:
			FBXRender( models.pRunFront.get(), HLSL, WVP, W, animeAccel );
			break;
		case RivalAI::ActionState::MOVE_GET_FAR:
			FBXRender( models.pLeave.get(), HLSL, WVP, W, animeAccel );
			break;
		case RivalAI::ActionState::MOVE_SIDE:
			( Donya::SignBit( moveSign ) == 1 ) // This sign is side of destination from target.
			? FBXRender( models.pRunLeft.get(), HLSL, WVP, W, animeAccel )
			: FBXRender( models.pRunRight.get(), HLSL, WVP, W, animeAccel );
			break;
		case RivalAI::ActionState::MOVE_AIM_SIDE:
			( Donya::SignBit( moveSign ) == 1 ) // This sign is side of destination from target.
			? FBXRender( models.pRunLeft.get(), HLSL, WVP, W, animeAccel )
			: FBXRender( models.pRunRight.get(), HLSL, WVP, W, animeAccel );
			break;
		case RivalAI::ActionState::ATTACK_BARRAGE:
			FBXRender( models.pAtkBarrage.get(), HLSL, WVP, W, animeAccel );
			break;
		case RivalAI::ActionState::ATTACK_LINE:
			FBXRender( models.pAtkLine.get(), HLSL, WVP, W, animeAccel );
			break;
		case RivalAI::ActionState::ATTACK_RAID:
			FBXRender( models.pAtkRaid.get(), HLSL, WVP, W, animeAccel );
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

		auto GetColor = [&COLOR_VALID, &COLOR_INVALID]( const Donya::Sphere &sphere )
		{
			return ( sphere.enable && sphere.exist ) ? COLOR_VALID : COLOR_INVALID;
		};

		// Attacks collision.
		switch ( status )
		{
		case RivalAI::ActionState::ATTACK_BARRAGE:
			{
				const auto &localHitBoxes = BarragesLocalHitBoxes();
				for ( const auto &sphereF : localHitBoxes )
				{
					const auto &sphere   = sphereF.collision;
					std::string meshName = RivalParam::Open().barrage.collideMeshName;
					Donya::Vector3 wsPos = CalcBoneTransformedPosition( models.pAtkBarrage.get(), meshName, W, sphere.pos );
					Donya::Vector4 color = GetColor( sphere );

					DrawSphere( wsPos, sphere.radius, color, /* applyParentMatrix = */ false );
				}
			}
			break;
		case RivalAI::ActionState::ATTACK_LINE:
			break;
		case RivalAI::ActionState::ATTACK_RAID:
			{
				const auto wsHitBox  = RaidWorldHitBox();
				Donya::Vector4 color = GetColor( wsHitBox );

				DrawSphere( wsHitBox.pos, wsHitBox.radius, color, /* applyParentMatrix = */ false );
			}
			break;
		case RivalAI::ActionState::ATTACK_RUSH:
			{
				const auto wsHitBox  = RushWorldHitBox();
				Donya::Vector4 color = GetColor( wsHitBox );
				DrawSphere( wsHitBox.pos, wsHitBox.radius, color, /* applyParentMatrix = */ false );

				Donya::Sphere wsTriggerRange = RivalParam::Open().rush.slashOccurRange;
				wsTriggerRange.pos = orientation.RotateVector( wsTriggerRange.pos ); // Rotate the offset.
				wsTriggerRange.pos += GetPos();
				color = GetColor( wsTriggerRange );
				DrawSphere( wsHitBox.pos, wsHitBox.radius, color, /* applyParentMatrix = */ false );
			}
			break;
		default: break;
		}
	}

#endif // DEBUG_MODE
}

bool Rival::IsCollideAttackHitBoxes( const Donya::AABB other, bool disableCollidingHitBoxes )
{
	if ( !RivalAI::IsAction( status ) || !other.enable || !other.exist ) { return false; }
	if ( extraStatus == ExtraState::DEFEAT ) { return false; }
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
bool Rival::IsCollideAttackHitBoxes( const Donya::OBB  other, bool disableCollidingHitBoxes )
{
	if ( !RivalAI::IsAction( status ) || !other.enable || !other.exist ) { return false; }
	if ( extraStatus == ExtraState::DEFEAT ) { return false; }
	// else

	auto ToWorldSphere			= [&]( const Donya::Sphere &sphere )
	{
		Donya::Sphere wsSphere = sphere;
		wsSphere.pos += GetPos();
		return wsSphere;
	};
	auto TransformSphereByMesh	= [&]( Donya::Sphere sphere, skinned_mesh *pMesh, const std::string &meshName )
	{
		sphere.pos = CalcBoneTransformedPosition( pMesh, meshName, CalcWorldMatrix(), sphere.pos );
		return sphere;
	};

	bool wasCollided = false;

	switch ( status )
	{
	case RivalAI::ActionState::ATTACK_BARRAGE:
		{
			auto &localHitBoxes = BarragesLocalHitBoxes();
			for ( auto &sphereF : localHitBoxes )
			{
				auto &sphere = sphereF.collision;
				std::string   meshName = RivalParam::Open().barrage.collideMeshName;
				Donya::Sphere wsSphere = TransformSphereByMesh( sphere, models.pAtkBarrage.get(), meshName );
				if ( Donya::OBB::IsHitSphere( other, wsSphere ) )
				{
					wasCollided = true;
					if ( disableCollidingHitBoxes ) { sphere.enable = false; }
				}
			}
		}
		break;
	case RivalAI::ActionState::ATTACK_LINE:
		{

		}
		break;
	case RivalAI::ActionState::ATTACK_RAID:
		{
			Donya::Sphere wsHitBox = RaidWorldHitBox();
			if ( Donya::OBB::IsHitSphere( other, wsHitBox ) )
			{
				wasCollided = true;
				if ( disableCollidingHitBoxes )
				{
					auto &refHitBox  = RivalParam::Get().RaidHitBox();
					refHitBox.sphereF.collision.enable = false;
				}
			}
		}
		break;
	case RivalAI::ActionState::ATTACK_RUSH:
		{
			Donya::Sphere wsHitBox = RushWorldHitBox();
			if ( Donya::OBB::IsHitSphere( other, wsHitBox ) )
			{
				wasCollided = true;
				if ( disableCollidingHitBoxes )
				{
					auto &refHitBox = RivalParam::Get().RushHitBox();
					refHitBox.collision.enable = false;
				}
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

std::vector<Donya::SphereFrame> &Rival::BarragesLocalHitBoxes()
{
	return RivalParam::Get().BarrageHitBoxes();
}

Donya::Sphere Rival::RaidWorldHitBox() const
{
	const auto			localHitBoxFN	= RivalParam::Get().RaidHitBox();
	const Donya::Sphere	localHitBox		= localHitBoxFN.sphereF.collision;
	const Donya::Vector3 wsPos = CalcBoneTransformedPosition
	(
		models.pAtkRaid.get(),
		localHitBoxFN.meshName,
		CalcWorldMatrix(),
		localHitBox.pos
	);
	return Donya::Sphere
	{
		wsPos,
		localHitBox.radius,
		localHitBox.exist,
		localHitBox.enable
	};
}
Donya::Sphere Rival::RushWorldHitBox() const
{
	Donya::Sphere wsHitBox = RivalParam::Get().RushHitBox().collision;
	wsHitBox.pos = orientation.RotateVector( wsHitBox.pos ); // Rotate the offset.
	wsHitBox.pos += GetPos();
	return wsHitBox;
}

void Rival::WasDefended()
{
	if ( status == RivalAI::ActionState::ATTACK_RUSH )
	{
		// Prevent place the position of myself and target.
		pos += -orientation.LocalFront() * RivalParam::Open().rush.rushSpeed;

		// Should call before BreakInit().
		AttackRushUninit();

		BreakInit();
	}
}
void Rival::ReceiveImpact()
{
	DefeatInit();
}

void Rival::SetFieldRadius( float newFieldRadius )
{
	const float bodyRadius = RivalParam::Open().stageBodyRadius;
	fieldRadius = std::max( newFieldRadius, bodyRadius );
}

bool Rival::IsDefeated() const
{
	const int MOTION_LENGTH = RivalParam::Open().defeat.motionLength;
	return ( extraStatus == ExtraState::DEFEAT && MOTION_LENGTH <= timer ) ? true : false;
}

void Rival::LoadModel()
{
	Donya::OutputDebugStr( "Begin Rival::LoadModel.\n" );

	loadFBX( models.pIdle.get(), GetModelPath( ModelAttribute::RivalIdle ) );
	Donya::OutputDebugStr( "Done RivalModel.Idle.\n" );
	loadFBX( models.pRunFront.get(), GetModelPath( ModelAttribute::RivalRunFront ) );
	Donya::OutputDebugStr( "Done RivalModel.Run.Front.\n" );
	loadFBX( models.pRunLeft.get(), GetModelPath( ModelAttribute::RivalRunLeft ) );
	Donya::OutputDebugStr( "Done RivalModel.Run.Left.\n" );
	loadFBX( models.pRunRight.get(), GetModelPath( ModelAttribute::RivalRunRight ) );
	Donya::OutputDebugStr( "Done RivalModel.Run.RIght.\n" );
	loadFBX( models.pBreak.get(), GetModelPath( ModelAttribute::RivalBreak ) );
	Donya::OutputDebugStr( "Done RivalModel.Break.\n" );
	loadFBX( models.pLeave.get(), GetModelPath( ModelAttribute::RivalLeave ) );
	Donya::OutputDebugStr( "Done RivalModel.Leave.\n" );
	loadFBX( models.pDefeat.get(), GetModelPath( ModelAttribute::RivalDefeat ) );
	Donya::OutputDebugStr( "Done RivalModel.Defeat.\n" );
	loadFBX( models.pAtkBarrage.get(), GetModelPath( ModelAttribute::RivalAtkBarrage ) );
	Donya::OutputDebugStr( "Done RivalModel.Attack.Barrage.\n" );
	loadFBX( models.pAtkLine.get(), GetModelPath( ModelAttribute::RivalAtkLine ) );
	Donya::OutputDebugStr( "Done RivalModel.Attack.Line.\n" );
	loadFBX( models.pAtkRaid.get(), GetModelPath( ModelAttribute::RivalAtkRaid ) );
	Donya::OutputDebugStr( "Done RivalModel.Attack.Raid.\n" );
	loadFBX( models.pAtkRushWait.get(), GetModelPath( ModelAttribute::RivalAtkRushWait ) );
	Donya::OutputDebugStr( "Done RivalModel.Attack.Rush.Wait.\n" );
	loadFBX( models.pAtkRushSlash.get(), GetModelPath( ModelAttribute::RivalAtkRushSlash ) );
	Donya::OutputDebugStr( "Done RivalModel.Attack.Rush.Slash.\n" );

	std::vector<std::shared_ptr<skinned_mesh> *> dontLoopModels
	{
		&models.pBreak,
		&models.pLeave,
		&models.pDefeat,
		&models.pAtkLine,
		&models.pAtkRaid,
		&models.pAtkRushSlash,
	};
	for ( auto &it : dontLoopModels )
	{
		setLoopFlg( it->get(), /* is_loop = */ false );
	}


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
	case RivalAI::ActionState::MOVE_GET_NEAR:		MoveUninit();			break;
	case RivalAI::ActionState::MOVE_GET_FAR:		MoveUninit();			break;
	case RivalAI::ActionState::MOVE_SIDE:			MoveUninit();			break;
	case RivalAI::ActionState::MOVE_AIM_SIDE:		MoveUninit();			break;
	case RivalAI::ActionState::ATTACK_BARRAGE:		AttackBarrageUninit();	break;
	case RivalAI::ActionState::ATTACK_LINE:			AttackLineUninit();		break;
	case RivalAI::ActionState::ATTACK_RAID:			AttackRaidUninit();		break;
	case RivalAI::ActionState::ATTACK_RUSH:			AttackRushUninit();		break;
	default: break;
	}

	if ( extraStatus != ExtraState::NONE )
	{
		switch ( extraStatus )
		{
		//	case Rival::ExtraState::RUSH_WAIT:		break;
		//	case Rival::ExtraState::RUSH_RUN:		break;
		//	case Rival::ExtraState::RUSH_SLASH:		break;
		case Rival::ExtraState::BREAK:	BreakUninit();	break;
		case Rival::ExtraState::LEAVE:	BreakUninit();	break;
		case Rival::ExtraState::DEFEAT:	DefeatUninit();	break;
		default: break;
		}
	}

	switch ( lotteryStatus )
	{
	case RivalAI::ActionState::WAIT:				WaitInit( target );					break;
	case RivalAI::ActionState::MOVE_GET_NEAR:		MoveInit( target, lotteryStatus );	break;
	case RivalAI::ActionState::MOVE_GET_FAR:		MoveInit( target, lotteryStatus );	break;
	case RivalAI::ActionState::MOVE_SIDE:			MoveInit( target, lotteryStatus );	break;
	case RivalAI::ActionState::MOVE_AIM_SIDE:		MoveInit( target, lotteryStatus );	break;
	case RivalAI::ActionState::ATTACK_BARRAGE:		AttackBarrageInit( target );		break;
	case RivalAI::ActionState::ATTACK_LINE:			AttackLineInit( target );			break;
	case RivalAI::ActionState::ATTACK_RAID:			AttackRaidInit( target );			break;
	case RivalAI::ActionState::ATTACK_RUSH:			AttackRushInit( target );			break;
	default: break;
	}

	status = scast<RivalAI::ActionState>( lotteryStatus );
}
void Rival::UpdateCurrentStatus( TargetStatus target )
{
	if ( extraStatus != ExtraState::NONE )
	{
		switch ( extraStatus )
		{
		//	case Rival::ExtraState::RUSH_WAIT:		break;
		//	case Rival::ExtraState::RUSH_RUN:		break;
		//	case Rival::ExtraState::RUSH_SLASH:		break;
		case Rival::ExtraState::BREAK:	BreakUpdate( target );	return; // break;
		case Rival::ExtraState::LEAVE:	BreakUpdate( target );	return; // break;
		case Rival::ExtraState::DEFEAT:	DefeatUpdate();			return; // break;
		default: break;
		}
	}

	switch ( status )
	{
	case RivalAI::ActionState::WAIT:				WaitUpdate( target );			break;
	case RivalAI::ActionState::MOVE_GET_NEAR:		MoveUpdate( target );			break;
	case RivalAI::ActionState::MOVE_GET_FAR:		MoveUpdate( target );			break;
	case RivalAI::ActionState::MOVE_SIDE:			MoveUpdate( target );			break;
	case RivalAI::ActionState::MOVE_AIM_SIDE:		MoveUpdate( target );			break;
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

#if GET_CLOSE
	const float distFar		= RivalParam::Open().targetDistFar;
	const float nDistance	= CalcNormalizedDistance( target.pos );

	if ( distFar <= nDistance )
	{
		AI.OverwriteState( RivalAI::ActionState::MOVE );
	}
#endif // GET_CLOSE

}
void Rival::WaitUninit()
{
	setAnimFlame( models.pIdle.get(), 0 );
}

void Rival::MoveInit( TargetStatus target, RivalAI::ActionState statusDetail )
{
	status		= statusDetail;
	slerpFactor	= RivalParam::Get().SlerpFactor( status );
	velocity	= 0.0f;
	moveSign	= Donya::Random::GenerateInt( 2 ) ? +1.0f : -1.0f;

	setAnimFlame( models.pRunFront.get(), 0 );
}
void Rival::MoveUpdate( TargetStatus target )
{
	const float speed = RivalParam::Get().MoveSpeed( status );
	
	switch ( status )
	{
	case RivalAI::ActionState::MOVE_GET_NEAR:
		velocity = orientation.LocalFront() * speed;
		return;
	case RivalAI::ActionState::MOVE_GET_FAR:
		velocity = -orientation.LocalFront() * speed;
		return;
	case RivalAI::ActionState::MOVE_SIDE:
		velocity = orientation.LocalRight() * moveSign * speed;
		return;
	case RivalAI::ActionState::MOVE_AIM_SIDE:
		{
			const float		distance	= ( target.pos - GetPos() ).Length() ;
			Donya::Vector3	destination	= target.pos;
			destination += ( orientation.LocalRight() * distance ) * moveSign;

			Donya::Vector3	moveVector = ( destination - GetPos() ).Normalized();
							moveVector.y = 0.0f; // Prevent Z-axis rotation by Quaternion::LookAt().
			velocity = moveVector * speed;
		}
		return;
	default: return;
	}
}
void Rival::MoveUninit()
{
	velocity = 0.0f;
	setAnimFlame( models.pRunFront.get(), 0 );
}

void Rival::AttackBarrageInit( TargetStatus target )
{
	status		= RivalAI::ActionState::ATTACK_BARRAGE;
	slerpFactor	= RivalParam::Get().SlerpFactor( status );
	velocity	= 0.0f;

	RivalParam::Get().ResetBarrage();
	setAnimFlame( models.pAtkBarrage.get(), 0 );
}
void Rival::AttackBarrageUpdate( TargetStatus target )
{
	RivalParam::Get().UpdateBarrage();
	
	const auto nowBarrage = RivalParam::Open().barrage;

	float walkSpeed = 0.0f;
	for ( auto &it : nowBarrage.walkTimings )
	{
		if ( it.withinFrame )
		{
			walkSpeed = it.speed;
			break;
		}
	}

	if ( ZeroEqual( walkSpeed ) )
	{
		velocity = 0.0f;
		return;
	}
	// else

	velocity = orientation.LocalFront() * walkSpeed;
}
void Rival::AttackBarrageUninit()
{
	RivalParam::Get().ResetBarrage();
	setAnimFlame( models.pAtkBarrage.get(), 0 );
}

void Rival::AttackLineInit( TargetStatus target )
{
	status		= RivalAI::ActionState::ATTACK_LINE;
	timer		= 0;
	slerpFactor	= RivalParam::Get().SlerpFactor( status );
	velocity	= 0.0f;

	// Revive the effects collision flags.
	{
		auto &effects = EffectManager::GetInstance()->GetLongAttackEffectVector();
		for ( auto &effect : effects )
		{
			auto &hitBoxes = effect.GetHitSphereVector();
			for ( auto &it : hitBoxes )
			{
				it.enable = true;
			}
		}
	}

	setAnimFlame( models.pAtkLine.get(), 0 );
}
void Rival::AttackLineUpdate( TargetStatus target )
{
	timer++;
	if ( timer == RivalParam::Open().line.generateFrame )
	{
		EffectManager::GetInstance()->Set
		(
			EffectManager::EffectType::LONG_ATTACK,
			target.pos,
			pos
		);
	}
}
void Rival::AttackLineUninit()
{
	timer = 0;

	setAnimFlame( models.pAtkLine.get(), 0 );
}

void Rival::AttackRaidInit( TargetStatus target )
{
	status		= RivalAI::ActionState::ATTACK_RAID;
	timer		= 0;
	slerpFactor	= RivalParam::Get().SlerpFactor( status );
	extraOffset	= 0.0f;
	velocity	= 0.0f;

	ResetCurrentSphereFN( &RivalParam::Get().RaidHitBox() );
	setAnimFlame( models.pAtkRaid.get(), 0 );
}
void Rival::AttackRaidUpdate( TargetStatus target )
{
	const int START_TIME	= RivalParam::Open().raid.jumpStartFrame;
	const int LANDING_TIME	= RivalParam::Open().raid.jumpLastFrame;
	
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
			slerpFactor = 0.0f;
		}
	}
	else
	if ( timer <= START_TIME + LANDING_TIME )
	{
		// Before landing. move to front with easing.
		
		float percent	= scast<float>( timer - START_TIME ) / scast<float>( LANDING_TIME );
		float ease		= Donya::Easing::Ease
		(
			scast<Donya::Easing::Kind>( RivalParam::Open().raid.easingKind ),
			scast<Donya::Easing::Type>( RivalParam::Open().raid.easingType ),
			percent
		);

		extraOffset = orientation.LocalFront() * ( RivalParam::Open().raid.jumpDistance * ease );

		if ( timer == START_TIME + LANDING_TIME )
		{
			ApplyExtraOffset();
		}
	}
	else
	{
		// No op.
	}

	auto &hitBox = RivalParam::Get().RaidHitBox();
	hitBox.sphereF.Update();
}
void Rival::AttackRaidUninit()
{
	timer		= 0;
	extraOffset	= 0.0f;
	velocity	= 0.0f;

	ResetCurrentSphereFN( &RivalParam::Get().RaidHitBox() );
	setAnimFlame( models.pAtkRaid.get(), 0 );
}

void Rival::AttackRushInit( TargetStatus target )
{
	status		= RivalAI::ActionState::ATTACK_RUSH;
	extraStatus	= ExtraState::RUSH_WAIT;
	timer		= RivalParam::Open().rush.waitLength;
	slerpFactor	= RivalParam::Get().SlerpFactor( status );
	velocity	= 0.0f;

	AI.StopUpdate();

	ResetCurrentSphereF( &RivalParam::Get().RushHitBox() );

	setAnimFlame( models.pAtkRushWait.get(), 0 );
	setAnimFlame( models.pAtkRushSlash.get(), 0 );
	setLoopFlg( models.pAtkRushSlash.get(), /* is_loop = */ false );
}
void Rival::AttackRushUpdate( TargetStatus target )
{
	switch ( extraStatus )
	{
	case Rival::ExtraState::RUSH_WAIT:
		{
			timer--;
			if ( timer <= 0 )
			{
				timer = RivalParam::Open().rush.slashStopAnimeFrame;
				extraStatus = ExtraState::RUSH_RUN;

				// HACK:May be set to not zero.
				slerpFactor = 0.0f;
			}
		}
		break;
	case Rival::ExtraState::RUSH_RUN:
		{
			// Animation.
			{
				timer--;

				if ( timer <= 0 )
				{
					timer = 0;
					setStopAnimation( models.pAtkRushSlash.get(), /* is_Stop = */ true );
				}
				else
				{
					RivalParam::Get().RushHitBox().Update();
				}
			}

			float trueFieldRadius{};
			float currentLength{};
			{
				// This calculate process is same as CollideToWall().

				const float bodyRadius = RivalParam::Open().stageBodyRadius;
				trueFieldRadius = fieldRadius - bodyRadius;
				
				constexpr Donya::Vector3 ORIGIN = Donya::Vector3::Zero();
				const Donya::Vector3 currentDistance = GetPos() - ORIGIN;
				currentLength = currentDistance.Length();
			}

			Donya::Sphere wsTriggerRange = RivalParam::Open().rush.slashOccurRange;
			wsTriggerRange.pos = orientation.RotateVector( wsTriggerRange.pos ); // Rotate the offset.
			wsTriggerRange.pos += GetPos();

			bool occurSlash = ( !velocity.IsZero()/* Except when first update of rush. */ && trueFieldRadius <= currentLength ) ||
				Donya::Sphere::IsHitPoint( wsTriggerRange, target.pos, /* ignoreExistFlag = */ true );

			if ( occurSlash )
			{
				timer		= 0;
				extraStatus	= ExtraState::RUSH_SLASH;
				velocity	= 0.0f;

				setStopAnimation( models.pAtkRushSlash.get(), /* is_Stop = */ false );
			}
			else
			{
				velocity = orientation.LocalFront() * RivalParam::Open().rush.rushSpeed;
			}
		}
		break;
	case Rival::ExtraState::RUSH_SLASH:
		{
			RivalParam::Get().RushHitBox().Update();

			if ( models.pAtkRushSlash->getAnimFinFlg() )
			{
				AI.ResumeUpdate();
				AI.FinishCurrentState( CalcNormalizedDistance( target.pos ) );
			}
		}
		break;
	default: break;
	}
}
void Rival::AttackRushUninit()
{
	timer		= 0;
	extraStatus	= ExtraState::NONE;

	ResetCurrentSphereF( &RivalParam::Get().RushHitBox() );

	setAnimFlame( models.pAtkRushWait.get(), 0 );
	setAnimFlame( models.pAtkRushSlash.get(), 0 );
}

void Rival::BreakInit()
{
	const auto temporaryActionState = RivalAI::ActionState::ATTACK_RUSH; // If this action is attack, after break status is wait. if this action is wait, after break status is attack.
	status		= temporaryActionState;
	extraStatus	= ExtraState::BREAK;
	timer		= RivalParam::Open().breakdown.breakFrame;
	slerpFactor	= 0.0f;
	velocity	= 0.0f;
	extraOffset	= 0.0f;

	// Should align the status.
	AI.OverwriteState( temporaryActionState );
	AI.StopUpdate();

	setAnimFlame( models.pBreak.get(), 0 );
	setAnimFlame( models.pLeave.get(), 0 );
	setLoopFlg( models.pBreak.get(), /* is_loop = */ false );
	setLoopFlg( models.pLeave.get(), /* is_loop = */ false );
}
void Rival::BreakUpdate( TargetStatus target )
{
	auto ApplyExtraOffset = [&]()->void
	{
		// Apply movement and reset extraOffset.
		pos += extraOffset;
		extraOffset = 0.0f;
	};

	switch ( extraStatus )
	{
	case Rival::ExtraState::BREAK:
		{
			timer--;
			if ( timer <= 0 )
			{
				timer = 0;
				extraStatus = ExtraState::LEAVE;
			}
		}
		break;
	case Rival::ExtraState::LEAVE:
		{
			timer++;

			const int MOTION_WHOLE_FRAME	= RivalParam::Open().breakdown.leaveWholeFrame;
			const int MOVING_FRAME			= RivalParam::Open().breakdown.leaveMoveFrame;

			float percent = scast<float>( timer ) / scast<float>( MOVING_FRAME );
			float ease = Donya::Easing::Ease
			(
				scast<Donya::Easing::Kind>( RivalParam::Open().breakdown.leaveEasingKind ),
				scast<Donya::Easing::Type>( RivalParam::Open().breakdown.leaveEasingType ),
				percent
			);

			extraOffset = -orientation.LocalFront() * ( RivalParam::Open().breakdown.leaveDistance * ease );

			if ( timer == MOVING_FRAME )
			{
				ApplyExtraOffset();

				timer = 0;
				AI.ResumeUpdate();
				AI.FinishCurrentState( CalcNormalizedDistance( target.pos ) );
			}
		}
		break;
	default: _ASSERT_EXPR( 0, L"Error : Unexpected behavior detected !" ); return; // break;
	}
}
void Rival::BreakUninit()
{
	status		= RivalAI::ActionState::WAIT;
	extraStatus	= ExtraState::NONE;
	timer		= 0;
	extraOffset	= 0.0f;

	setAnimFlame( models.pBreak.get(), 0 );
	setAnimFlame( models.pLeave.get(), 0 );
}

void Rival::DefeatInit()
{
	const auto endState = decltype( status )::END;
	status		= endState;
	extraStatus	= ExtraState::DEFEAT;
	AI.OverwriteState( endState );
	AI.StopUpdate();

	timer		= 0;
	velocity	= 0.0f;
	slerpFactor	= 0.0f;
}
void Rival::DefeatUpdate()
{
	timer++;
}
void Rival::DefeatUninit()
{
	timer = 0;
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

void Rival::UseImGui( float normalizedTargetDistance )
{
	if ( ImGui::BeginIfAllowed() )
	{
		if ( ImGui::TreeNode( "Rival.CurrentParameter" ) )
		{
			auto GetStatusName = []( RivalAI::ActionState status )->std::string
			{
				switch ( status )
				{
				case RivalAI::ActionState::WAIT:				return { "Wait" };				// break;
				case RivalAI::ActionState::MOVE_GET_NEAR:		return { "Move.Near" };			// break;
				case RivalAI::ActionState::MOVE_GET_FAR:		return { "Move.Far" };			// break;
				case RivalAI::ActionState::MOVE_SIDE:			return { "Move.Side" };			// break;
				case RivalAI::ActionState::MOVE_AIM_SIDE:		return { "Move.AimSide" };		// break;
				case RivalAI::ActionState::ATTACK_BARRAGE:		return { "Attack.Barrage" };	// break;
				case RivalAI::ActionState::ATTACK_LINE:			return { "Attack.Line" };		// break;
				case RivalAI::ActionState::ATTACK_RAID:			return { "Attack.Raid" };		// break;
				case RivalAI::ActionState::ATTACK_RUSH:			return { "Attack.Rush" };		// break;
				default: break;
				}

				return { "Unexpected status" };
			};
			std::string statusCaption = "Status : " + GetStatusName( status );
			ImGui::Text( statusCaption.c_str() );
			ImGui::Text( "Timer : %d", timer );
			ImGui::Text( "SlerpFactor : %5.3f", slerpFactor );
			ImGui::Text( "Distance.Target : %5.3f", normalizedTargetDistance );
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
