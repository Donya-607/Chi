#include "Boss.h"

#include "Donya/Easing.h"
#include "Donya/FilePath.h"
#include "Donya/Useful.h"		// For IsShowCollision().

#include "gameLib.h"
#include "gameLibFunctions.h"	// Use FBXRender().
#include "skinned_mesh.h"

#undef max
#undef min

#define scast static_cast

GolemParam::GolemParam() :
	rotLeaveEaseType(), rotLeaveEaseKind(), rotLeaveStartFrame(), rotLeaveWholeFrame( 1 ), rotLeaveDistance(),
	scale( 1.0f ), stageBodyRadius( 1.0f ),
	targetDistNear( 0.0f ), targetDistFar( 1.0f ),
	idleSlerpFactor( 1.0f ),
	moveMoveSpeed( 0.0f ), moveSlerpFactor( 1.0f ),
	attackFastMoveSpeed( 0.0f ), attackFastSlerpFactor( 1.0f ),
	attackRotateMoveSpeed( 0.0f ), attackRotateSlerpFactor( 1.0f ),
	initPosPerStage(), drawOffsetsPerStage(), hitBoxesBody(),
	OBBAttacksSwing(), OBBFAttacksFast(), rotateAttackCollisions()
{
	initPosPerStage.resize( 1U );
	drawOffsetsPerStage.resize( 1U );
}
GolemParam::~GolemParam() = default;

void GolemParam::Init()
{
	LoadParameter();
}
void GolemParam::Uninit()
{
	// No op.
}

float GolemParam::MoveSpeed( GolemAI::ActionState status ) const
{
	switch ( status )
	{
	case GolemAI::ActionState::WAIT:				return 0.0f;					// break;
	case GolemAI::ActionState::MOVE:				return moveMoveSpeed;			// break;
	case GolemAI::ActionState::ATTACK_SWING:		return 0.0f;					// break;
	case GolemAI::ActionState::ATTACK_FAST:		return attackFastMoveSpeed;		// break;
	case GolemAI::ActionState::ATTACK_ROTATE:	return attackRotateMoveSpeed;	// break;
	default: break;
	}

	return NULL;
}
float GolemParam::SlerpFactor( GolemAI::ActionState status ) const
{
	switch ( status )
	{
	case GolemAI::ActionState::WAIT:				return idleSlerpFactor;			// break;
	case GolemAI::ActionState::MOVE:				return moveSlerpFactor;			// break;
	case GolemAI::ActionState::ATTACK_SWING:		return 0.0f;					// break;
	case GolemAI::ActionState::ATTACK_FAST:		return attackFastSlerpFactor;	// break;
	case GolemAI::ActionState::ATTACK_ROTATE:	return attackRotateSlerpFactor;	// break;
	default: break;
	}

	return 1.0f;
}
Donya::Vector3 GolemParam::GetInitPosition( int stageNo ) const
{
	_ASSERT_EXPR( stageNo <= scast<int>( initPosPerStage.size() ), L"Error : Specified stage number over than stage count !" );

	return initPosPerStage[stageNo];
}
Donya::Vector3 GolemParam::GetDrawOffset( int stageNo ) const
{
	_ASSERT_EXPR( stageNo <= scast<int>( drawOffsetsPerStage.size() ), L"Error : Specified stage number over than stage count !" );

	return drawOffsetsPerStage[stageNo];
}

void GolemParam::LoadParameter( bool isBinary )
{
	Donya::Serializer::Extension ext = ( isBinary )
	? Donya::Serializer::Extension::BINARY
	: Donya::Serializer::Extension::JSON;
	std::string filePath = GenerateSerializePath( SERIAL_ID, ext );

	Donya::Serializer seria;
	seria.Load( ext, filePath.c_str(), SERIAL_ID, *this );
}

#if USE_IMGUI

void GolemParam::SaveParameter()
{
	Donya::Serializer::Extension bin  = Donya::Serializer::Extension::BINARY;
	Donya::Serializer::Extension json = Donya::Serializer::Extension::JSON;
	std::string binPath  = GenerateSerializePath( SERIAL_ID, bin  );
	std::string jsonPath = GenerateSerializePath( SERIAL_ID, json );

	Donya::Serializer seria;
	seria.Save( bin,  binPath.c_str(),  SERIAL_ID, *this );
	seria.Save( json, jsonPath.c_str(), SERIAL_ID, *this );
}

void GolemParam::UseImGui()
{
	if ( ImGui::BeginIfAllowed() )
	{
		if ( ImGui::TreeNode( "Golem.AdjustData" ) )
		{
			ImGui::SliderFloat( "Scale", &scale, 0.0f, 8.0f );
			ImGui::DragFloat( "BodyRadiusToStage", &stageBodyRadius );
			ImGui::Text( "" );

			ImGui::SliderFloat( "Distance.ThinkTo\"Near\"", &targetDistNear, 0.01f, 1.0f );
			ImGui::SliderFloat( "Distance.ThinkTo\"Far\"",  &targetDistFar,  0.0f, 0.99f );
			ImGui::Text( "" );

			ImGui::DragFloat( "MoveSpeed.\"Move\"State", &moveMoveSpeed );
			ImGui::DragFloat( "MoveSpeed.\"Attack.Fast\"State", &attackFastMoveSpeed );
			ImGui::DragFloat( "MoveSpeed.\"Attack.Rotate\"State", &attackRotateMoveSpeed );
			ImGui::Text( "" );

			ImGui::SliderFloat( "SlerpFactor.\"Idle\"State", &idleSlerpFactor, 0.0f, 1.0f );
			ImGui::SliderFloat( "SlerpFactor.\"Move\"State", &moveSlerpFactor, 0.0f, 1.0f );
			ImGui::SliderFloat( "SlerpFactor.\"Attack.Fast\"State", &attackFastSlerpFactor, 0.0f, 1.0f );
			ImGui::SliderFloat( "SlerpFactor.\"Attack.Rotate\"State", &attackRotateSlerpFactor, 0.0f, 1.0f );
			ImGui::Text( "" );

			if ( ImGui::TreeNode( "Initialize.Position" ) )
			{
				if ( ImGui::Button( "Append" ) )
				{
					initPosPerStage.push_back( {} );
				}
				if ( 2 <= initPosPerStage.size() && ImGui::Button( "PopBack" ) )
				{
					initPosPerStage.pop_back();
				}

				const size_t COUNT = initPosPerStage.size();
				for ( size_t i = 0; i < COUNT; ++i )
				{
					ImGui::DragFloat3( ( "Stage[" + std::to_string( i ) + "].Pos" ).c_str(), &initPosPerStage[i].x );
				}

				ImGui::TreePop();
			}	
			if ( ImGui::TreeNode( "DrawPosition.Offset" ) )
			{
				if ( ImGui::Button( "Append" ) )
				{
					drawOffsetsPerStage.push_back( {} );
				}
				if ( 2 <= drawOffsetsPerStage.size() && ImGui::Button( "PopBack" ) )
				{
					drawOffsetsPerStage.pop_back();
				}

				const size_t COUNT = drawOffsetsPerStage.size();
				for ( size_t i = 0; i < COUNT; ++i )
				{
					ImGui::DragFloat3( ( "Stage[" + std::to_string( i ) + "].Pos" ).c_str(), &drawOffsetsPerStage[i].x );
				}

				ImGui::TreePop();
			}
			ImGui::Text( "" );
			if ( ImGui::TreeNode( "SwingAttack" ) )
			{
				ImGui::DragInt  ( "StopFrame",			&swingStopFrame  );
				ImGui::DragFloat( "StopLength(Second)",	&swingStopLength, 0.01f );
				swingStopFrame  = std::max( 0,		swingStopFrame  );
				swingStopLength = std::max( 0.0f,	swingStopLength );

				ImGui::TreePop();
			}
			ImGui::Text( "" );
			if ( ImGui::TreeNode( "RotateAttack" ) )
			{
				// Easing parameter.
				{
					using namespace Donya;
					constexpr int KIND_COUNT = Easing::GetKindCount();
					constexpr int TYPE_COUNT = Easing::GetTypeCount();
					ImGui::SliderInt( "Leave.EasingKind", &rotLeaveEaseKind, 0, KIND_COUNT - 1 );
					ImGui::SliderInt( "Leave.EasingType", &rotLeaveEaseType, 0, TYPE_COUNT - 1 );
					std::string strKind = Easing::KindName( rotLeaveEaseKind );
					std::string strType = Easing::TypeName( rotLeaveEaseType );
					ImGui::Text( ( "Easing : " + strKind + "." + strType ).c_str() );
				}

				ImGui::DragInt( "Leave.StartFrame", &rotLeaveStartFrame );
				ImGui::DragInt( "Leave.WholeFrame", &rotLeaveWholeFrame );
				ImGui::DragFloat( "Leave.Distance", &rotLeaveDistance   );
				rotLeaveStartFrame	= std::max( 0, rotLeaveStartFrame );
				rotLeaveWholeFrame	= std::max( 0, rotLeaveWholeFrame );
				rotLeaveDistance	= std::max( 0.0f, rotLeaveDistance );

				ImGui::TreePop();
			}
			ImGui::Text( "" );
			if ( ImGui::TreeNode( "Collisions" ) )
			{
				auto ShowAABB		= []( const std::string &prefix, Donya::AABB		*pAABB	)
				{
					ImGui::DragFloat3( ( prefix + ".Offset" ).c_str(), &pAABB->pos.x );
					ImGui::DragFloat3( ( prefix + ".Scale" ).c_str(), &pAABB->size.x );
					ImGui::Checkbox( ( prefix + ".ExistCollision" ).c_str(), &pAABB->exist );
				};
				auto ShowSphere		= []( const std::string &prefix, Donya::Sphere		*pSphere)
				{
					ImGui::DragFloat3( ( prefix + ".Offset" ).c_str(), &pSphere->pos.x );
					ImGui::DragFloat( ( prefix + ".Scale" ).c_str(), &pSphere->radius );
					ImGui::Checkbox( ( prefix + ".ExistCollision" ).c_str(), &pSphere->exist );
				};

				auto ShowOBB		= []( const std::string &prefix, Donya::OBB			*pOBB	)
				{
					ImGui::DragFloat3( ( prefix + ".Offset" ).c_str(), &pOBB->pos.x );
					ImGui::DragFloat3( ( prefix + ".Scale" ).c_str(), &pOBB->size.x );

					Donya::Vector3 euler = pOBB->orientation.GetEulerAngles();
					ImGui::SliderFloat3( ( prefix + ".EulerAngle(Radian)" ).c_str(), &euler.x, ToRadian( -360.0f ), ToRadian( 360.0f ) );
					pOBB->orientation = Donya::Quaternion::Make( euler.x, euler.y, euler.z );

					ImGui::Checkbox( ( prefix + ".ExistCollision" ).c_str(), &pOBB->exist );
				};
				auto ShowOBBF		= [&ShowOBB]( const std::string &prefix, Donya::OBBFrame	*pOBBF	)
				{
					ImGui::DragInt( ( prefix + ".EnableFrame.Start" ).c_str(), &pOBBF->enableFrameStart );
					ImGui::DragInt( ( prefix + ".EnableFrame.Last"  ).c_str(), &pOBBF->enableFrameLast  );

					bool oldExistFlag = pOBBF->OBB.exist;
					ShowOBB( prefix, &pOBBF->OBB );
					pOBBF->OBB.exist = oldExistFlag;
				};
				auto ShowSphereF	= [&ShowSphere]( const std::string &prefix, Donya::SphereFrame	*pSphereF	)
				{
					ImGui::DragInt( ( prefix + ".EnableFrame.Start" ).c_str(), &pSphereF->enableFrameStart );
					ImGui::DragInt( ( prefix + ".EnableFrame.Last"  ).c_str(), &pSphereF->enableFrameLast  );

					bool oldExistFlag = pSphereF->collision.exist;
					ShowSphere( prefix, &pSphereF->collision );
					pSphereF->collision.exist = oldExistFlag;
				};

				auto ShowAABBs		= [&ShowAABB]( const std::string &prefix, std::vector<Donya::AABB> *pAABBs )
				{
					if ( ImGui::TreeNode( prefix.c_str() ) )
					{

						if ( ImGui::Button( "Append" ) )
						{
							pAABBs->push_back( {} );
						}
						if ( 1 <= pAABBs->size() && ImGui::Button( "PopBack" ) )
						{
							pAABBs->pop_back();
						}

						const size_t COUNT = pAABBs->size();
						for ( size_t i = 0; i < COUNT; ++i )
						{
							auto &AABB = pAABBs->at( i );
							ShowAABB( "[" + std::to_string( i ) + "]", &AABB );
						}

						ImGui::TreePop();
					}
				};
				auto ShowOBBFs		= [&ShowOBBF]( const std::string &prefix, std::vector<Donya::OBBFrame> *pOBBFs )
				{
					if ( ImGui::TreeNode( prefix.c_str() ) )
					{
						if ( ImGui::Button( "Append" ) )
						{
							pOBBFs->push_back( {} );
						}
						if ( 1 <= pOBBFs->size() && ImGui::Button( "PopBack" ) )
						{
							pOBBFs->pop_back();
						}

						const size_t COUNT = pOBBFs->size();
						for ( size_t i = 0; i < COUNT; ++i )
						{
							auto &OBBF = pOBBFs->at( i );
							ShowOBBF( "[" + std::to_string( i ) + "]", &OBBF );
						}

						ImGui::TreePop();
					}
				};
				auto ShowSphereFs	= [&ShowSphereF]( const std::string &prefix, std::vector<Donya::SphereFrame> *pSphereFs )
				{
					if ( ImGui::TreeNode( prefix.c_str() ) )
					{
						if ( ImGui::Button( "Append" ) )
						{
							pSphereFs->push_back( {} );
						}
						if ( 1 <= pSphereFs->size() && ImGui::Button( "PopBack" ) )
						{
							pSphereFs->pop_back();
						}

						const size_t COUNT = pSphereFs->size();
						for ( size_t i = 0; i < COUNT; ++i )
						{
							auto &sphereF = pSphereFs->at( i );
							ShowSphereF( "[" + std::to_string( i ) + "]", &sphereF );
						}

						ImGui::TreePop();
					}
				};

				if ( ImGui::TreeNode( "Attacks.Fast" ) )
				{
					static std::vector<std::array<char, 512U>> atksFastNameBuffers{};

					if ( ImGui::Button( "Append" ) )
					{
						OBBFAttacksFast.push_back( {} );
						OBBFAttacksFast.back().meshName.reserve( 512U );
					}
					if ( 1 <= OBBFAttacksFast.size() && ImGui::Button( "PopBack" ) )
					{
						OBBFAttacksFast.pop_back();
					}

					if ( atksFastNameBuffers.size() != OBBFAttacksFast.size() )
					{
						atksFastNameBuffers.resize( OBBFAttacksFast.size() );
					}

					const size_t COUNT = OBBFAttacksFast.size();
					for ( size_t i = 0; i < COUNT; ++i )
					{
						auto &OBBF = OBBFAttacksFast[i].OBBF;
						ShowOBBF( "[" + std::to_string( i ) + "]", &OBBF );

						std::string elementCaption = "MeshName.Fasts[" + std::to_string( i ) + "]";
						ImGui::InputText( elementCaption.c_str(), atksFastNameBuffers[i].data(), atksFastNameBuffers[i].size() );
						if ( ImGui::Button( ( "Apply." + elementCaption ).c_str() ) )
						{
							OBBFAttacksFast[i].meshName = atksFastNameBuffers[i].data();
						}
					}

					ImGui::TreePop();
				}

				ShowOBBFs( "Attack.Swing", &OBBAttacksSwing );

				ShowSphereFs( "Attacks.Rotate", &rotateAttackCollisions );

				ShowAABBs( "Body.HurtBox", &hitBoxesBody );

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

Donya::OBB GolemParam::OBBFrameWithName::CalcTransformedOBB( skinned_mesh *pMesh, const Donya::Vector4x4 &parentSpaceMatrix ) const
{
	Donya::OBB oldOBB = OBBF.OBB;

	// To world space from local space.
	Donya::Vector4x4 parentMatrix = parentSpaceMatrix;

	// To local space from mesh's mesh space.
	Donya::Vector4x4 meshMatrix{};
	{
		// Input:offset in local-space. Output:transformed position.
		Donya::Vector3 outputPos = oldOBB.pos;
		bool succeeded = calcTransformedPosBySpecifyMesh( pMesh, outputPos, meshName );
		if ( !succeeded )
		{
			_ASSERT_EXPR( 0, L"Error : specified mesh name was not found !" );
			return Donya::OBB::Nil();
		}
		Donya::Vector4x4 T = Donya::Vector4x4::MakeTranslation( outputPos );

		// Convert to world space from local space.
		meshMatrix = T * parentMatrix;
	}

	Donya::OBB resultOBB = oldOBB;
	resultOBB.pos.x = meshMatrix._41;
	resultOBB.pos.y = meshMatrix._42;
	resultOBB.pos.z = meshMatrix._43;

	// Rotation.
	{
		Donya::Vector3 localFront = oldOBB.pos + Donya::Vector3::Front();
		calcTransformedPosBySpecifyMesh( pMesh, localFront, meshName );
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

void ResetCurrentOBBFrames( std::vector<Donya::OBBFrame> *pOBBFs )
{
	const size_t COUNT = pOBBFs->size();
	for ( size_t i = 0; i < COUNT; ++i )
	{
		auto &OBBF = pOBBFs->at( i );
		OBBF.currentFrame = 0;
	}
}
void ResetCurrentSphereFrames( std::vector<Donya::SphereFrame> *pSphereFs )
{
	const size_t COUNT = pSphereFs->size();
	for ( size_t i = 0; i < COUNT; ++i )
	{
		auto &sphereF = pSphereFs->at( i );
		sphereF.currentFrame = 0;
	}
}
void ResetCurrentOBBFNames( std::vector<GolemParam::OBBFrameWithName> *pOBBFNs )
{
	const size_t COUNT = pOBBFNs->size();
	for ( size_t i = 0; i < COUNT; ++i )
	{
		auto &OBBFN = pOBBFNs->at( i );
		OBBFN.OBBF.currentFrame = 0;
	}
}

Golem::Golem() :
	status( GolemAI::ActionState::WAIT ),
	AI(),
	stageNo( 1 ), timer(), swingTimer(),
	fieldRadius(), slerpFactor( 1.0f ), easeFactor(),
	pos(), velocity(), extraOffset(),
	orientation(),
	models()
{
	auto InitializeModel = []( std::unique_ptr<skinned_mesh> *ppMesh )
	{
		*ppMesh = std::make_unique<skinned_mesh>();
		setAnimFlame( ppMesh->get(), 0 );
	};

	std::vector<std::unique_ptr<skinned_mesh> *> modelRefs
	{
		&models.pIdle,
		&models.pAtkFast,
		&models.pAtkSwing,
		&models.pAtkRotate,
	};
	for ( auto &it : modelRefs )
	{
		InitializeModel( it );
	}
}
Golem::~Golem() = default;

void Golem::Init( int stageNumber )
{
	GolemParam::Get().Init();

	LoadModel();

	stageNo	= stageNumber;
	status	= GolemAI::ActionState::WAIT;
	pos		= GolemParam::Get().GetInitPosition( stageNo );

	SetFieldRadius( 0.0f ); // Set to body's radius.

	Donya::Vector3 dirToOrigin = Donya::Vector3::Zero() - pos;
	dirToOrigin.y = 0.0f;
	orientation = Donya::Quaternion::LookAt( orientation, dirToOrigin );

	AI.Init();
}
void Golem::Uninit()
{
	GolemParam::Get().Uninit();
}

void Golem::Update( TargetStatus target )
{
#if USE_IMGUI

	GolemParam::Get().UseImGui();
	UseImGui();

#endif // USE_IMGUI

	AI.Update();

	ChangeStatus( target );
	UpdateCurrentStatus( target );

	ApplyVelocity( target );
	CollideToWall();
}

void Golem::Draw( const Donya::Vector4x4 &matView, const Donya::Vector4x4 &matProjection )
{
	const auto &PARAM = GolemParam::Get();

	const Donya::Vector3 drawOffset = PARAM.GetDrawOffset( stageNo );
	const Donya::Vector4x4 DRAW_OFFSET = Donya::Vector4x4::MakeTranslation( drawOffset );

	Donya::Vector4x4 S = Donya::Vector4x4::MakeScaling( PARAM.Scale() );
	Donya::Vector4x4 R = orientation.RequireRotationMatrix();
	Donya::Vector4x4 T = Donya::Vector4x4::MakeTranslation( GetPos() );
#if DEBUG_MODE
	if ( status == decltype( status )::END ) { R = Donya::Quaternion::Make( Donya::Vector3::Front(), ToRadian( 180.0f ) ).RequireRotationMatrix(); };
#endif // DEBUG_MODE
	Donya::Vector4x4 W = S * R * T * DRAW_OFFSET;
	Donya::Vector4x4 WVP = W * matView * matProjection;

	switch ( status )
	{
	case GolemAI::ActionState::WAIT:
		FBXRender( models.pIdle.get(), WVP, W );
		break;
	case GolemAI::ActionState::MOVE:
		FBXRender( models.pIdle.get(), WVP, W );
		break;
	case GolemAI::ActionState::ATTACK_SWING:
		FBXRender( models.pAtkSwing.get(), WVP, W );
		break;
	case GolemAI::ActionState::ATTACK_FAST:
		FBXRender( models.pAtkFast.get(), WVP, W );
		break;
	case GolemAI::ActionState::ATTACK_ROTATE:
		FBXRender( models.pAtkRotate.get(), WVP, W );
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

	// Except DRAW_OFFSET matrix. because I wanna draw collisions to actual position.
	W = S * R * T;

	auto DrawCube	= [&]( const Donya::Vector3 &cubeOffset, const Donya::Vector3 &cubeScale, const Donya::Quaternion &orientation, const Donya::Vector4 &color )
	{
		Donya::Vector4x4 CS = Donya::Vector4x4::MakeScaling( cubeScale * 2.0f ); // Half size->Whole size.
		Donya::Vector4x4 CR = orientation.RequireRotationMatrix();
		Donya::Vector4x4 CT = Donya::Vector4x4::MakeTranslation( cubeOffset );
		Donya::Vector4x4 CW = ( CS * CR * CT ) * W;
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

		constexpr Donya::Vector4 COLOR_BODY			{ 0.0f, 0.5f, 0.0f, 0.6f };
		constexpr Donya::Vector4 COLOR_BODY_CIRCLE	{ 0.0f, 0.3f, 0.3f, 0.6f };
		
		const auto *pAABBs = PARAM.BodyHitBoxes();
		for ( const auto &it : *pAABBs )
		{
			DrawCube( it.pos, it.size, orientation, COLOR_BODY );
		}

		DrawSphere( {/*Zero*/}, PARAM.StageBodyRadius(), COLOR_BODY_CIRCLE );

		constexpr Donya::Vector4 COLOR_VALID  { 1.0f, 0.5f, 0.0f, 0.6f };
		constexpr Donya::Vector4 COLOR_INVALID{ 0.4f, 0.4f, 1.0f, 0.6f };

		// Attacks collision.
		switch ( status )
		{
		case GolemAI::ActionState::WAIT:
			break;
		case GolemAI::ActionState::MOVE:
			break;
		case GolemAI::ActionState::ATTACK_SWING:
			{
				const auto  *pOBBs = GolemParam::Get().OBBAtksSwing();
				const size_t COUNT = pOBBs->size();
				Donya::Vector4 color{};
				for ( size_t i = 0; i < COUNT; ++i )
				{
					const auto &OBB = pOBBs->at( i );
					color = ( OBB.OBB.exist ) ? COLOR_VALID : COLOR_INVALID;

					DrawOBB( OBB.OBB, color );
				}
			}
			break;
		case GolemAI::ActionState::ATTACK_FAST:
			{
				const auto  *pOBBFs = GolemParam::Get().OBBFAtksFast();
				const size_t COUNT = pOBBFs->size();
				Donya::Vector4 color{};
				for ( size_t i = 0; i < COUNT; ++i )
				{
					const auto &OBB = pOBBFs->at( i ).CalcTransformedOBB( models.pAtkFast.get(), W ); ;
					color = ( OBB.exist ) ? COLOR_VALID : COLOR_INVALID;

					DrawOBB( OBB, color, /* applyParentMatrix = */ false );
				}
			}
			break;
		case GolemAI::ActionState::ATTACK_ROTATE:
			{
				const auto *pSphereFs = GolemParam::Get().RotateAtkCollisions();
				const size_t COUNT = pSphereFs->size();
				Donya::Vector4 color{};
				for ( size_t i = 0; i < COUNT; ++i )
				{
					const auto &sphereF = pSphereFs->at( i );
					color = ( sphereF.collision.exist ) ? COLOR_VALID : COLOR_INVALID;

					DrawSphere( sphereF.collision.pos, sphereF.collision.radius, color );
				}
			}
			break;
		default: break;
		}
	}

#endif // DEBUG_MODE
}

std::vector<Donya::OBB> Golem::RequireAttackHitBoxesOBB() const
{
	const auto &PARAM = GolemParam::Get();
	std::vector<Donya::OBB> collisions{};

	auto ToWorldOBB = [&]( Donya::OBB obb )
	{
		obb.pos += GetPos();
		obb.orientation.RotateBy( orientation );
		return obb;
	};

	Donya::Vector4x4 S = Donya::Vector4x4::MakeScaling( PARAM.Scale() );
	Donya::Vector4x4 R = orientation.RequireRotationMatrix();
	Donya::Vector4x4 T = Donya::Vector4x4::MakeTranslation( GetPos() );
#if DEBUG_MODE
	if ( status == decltype( status )::END ) { R = Donya::Quaternion::Make( Donya::Vector3::Front(), ToRadian( 180.0f ) ).RequireRotationMatrix(); };
#endif // DEBUG_MODE
	Donya::Vector4x4 W = S * R * T;

	switch ( status )
	{
	case GolemAI::ActionState::WAIT:
		break;
	case GolemAI::ActionState::MOVE:
		break;
	case GolemAI::ActionState::ATTACK_SWING:
		{
			const auto  *pOBBs = PARAM.OBBAtksSwing();
			const size_t COUNT = pOBBs->size();
			for ( size_t i = 0; i < COUNT; ++i )
			{
				auto &OBB = pOBBs->at( i );
				if (  OBB.OBB.exist )
				{
					collisions.emplace_back( ToWorldOBB( OBB.OBB ) );
				}
			}
		}
		break;
	case GolemAI::ActionState::ATTACK_FAST:
		{
			auto  *pOBBFs  = PARAM.OBBFAtksFast();
			const size_t COUNT  = pOBBFs->size();
			for ( size_t i = 0; i < COUNT; ++i )
			{
				auto &OBBF = pOBBFs->at( i );
				auto &OBB  = OBBF.OBBF.OBB;
				if (  OBB.exist )
				{
					collisions.emplace_back
					(
						OBBF.CalcTransformedOBB( models.pAtkFast.get(), W )
					);
				}
			}
		}
		break;
	default: break;
	}

	return collisions;
}
std::vector<Donya::Sphere> Golem::RequireAttackHitBoxesSphere() const
{
	std::vector<Donya::Sphere> spheres{};

	const auto *pRotateAtks = GolemParam::Get().RotateAtkCollisions();
	for ( const auto &it : *pRotateAtks )
	{
		if ( !it.collision.exist ) { continue; }
		// else
		spheres.emplace_back( it.collision );
	}

	// To world space.
	for ( auto &it : spheres )
	{
		it.pos += GetPos();
	}

	return spheres;
}
static Donya::OBB MakeOBB( const Donya::AABB &AABB, const Donya::Vector3 &wsPos, const Donya::Quaternion &orientation )
{
	Donya::OBB OBB{};
	OBB.pos			= AABB.pos + wsPos;
	OBB.size		= AABB.size;
	OBB.orientation	= orientation;
	OBB.exist		= AABB.exist;
	return OBB;
}
std::vector<Donya::OBB> Golem::GetBodyHitBoxes() const
{
	const auto &PARAM = GolemParam::Get();
	std::vector<Donya::OBB> collisions{};

	const auto *pAABBs = PARAM.BodyHitBoxes();
	for ( const auto &it : *pAABBs )
	{
		collisions.emplace_back( MakeOBB( it, GetPos(), orientation ) );
	}

	return collisions;
}

void Golem::ReceiveImpact()
{
	status = decltype( status )::END;
}

void Golem::SetFieldRadius( float newFieldRadius )
{
	const float bodyRadius = GolemParam::Get().StageBodyRadius();
	fieldRadius = std::max( newFieldRadius, bodyRadius );
}

void Golem::LoadModel()
{
	loadFBX( models.pIdle.get(),		GetModelPath( ModelAttribute::BossIdle		) );
	loadFBX( models.pAtkSwing.get(),	GetModelPath( ModelAttribute::BossAtkSwing	) );
	loadFBX( models.pAtkFast.get(),		GetModelPath( ModelAttribute::BossAtkFast	) );
	loadFBX( models.pAtkRotate.get(),	GetModelPath( ModelAttribute::BossAtkRotate	) );
}

float Golem::CalcNormalizedDistance( Donya::Vector3 wsTargetPos )
{
	float distance = ( wsTargetPos - GetPos() ).Length();
	return ( ZeroEqual( fieldRadius ) )
	? fieldRadius
	: distance / fieldRadius;
}

void Golem::ChangeStatus( TargetStatus target )
{
	GolemAI::ActionState lotteryStatus = AI.GetState();
	if ( status == lotteryStatus ) { return; }
	// else

	switch ( status )
	{
	case GolemAI::ActionState::WAIT:				WaitUninit();			break;
	case GolemAI::ActionState::MOVE:				MoveUninit();			break;
	case GolemAI::ActionState::ATTACK_SWING:		AttackSwingUninit();	break;
	case GolemAI::ActionState::ATTACK_FAST:		AttackFastUninit();		break;
	case GolemAI::ActionState::ATTACK_ROTATE:	AttackRotateUninit();	break;
	default: break;
	}
	switch ( lotteryStatus )
	{
	case GolemAI::ActionState::WAIT:				WaitInit( target );			break;
	case GolemAI::ActionState::MOVE:				MoveInit( target );			break;
	case GolemAI::ActionState::ATTACK_SWING:		AttackSwingInit( target );	break;
	case GolemAI::ActionState::ATTACK_FAST:		AttackFastInit( target );	break;
	case GolemAI::ActionState::ATTACK_ROTATE:	AttackRotateInit( target );	break;
	default: break;
	}

	status = scast<GolemAI::ActionState>( lotteryStatus );
}
void Golem::UpdateCurrentStatus( TargetStatus target )
{
	switch ( status )
	{
	case GolemAI::ActionState::WAIT:				WaitUpdate( target );			break;
	case GolemAI::ActionState::MOVE:				MoveUpdate( target );			break;
	case GolemAI::ActionState::ATTACK_SWING:		AttackSwingUpdate( target );	break;
	case GolemAI::ActionState::ATTACK_FAST:		AttackFastUpdate( target );		break;
	case GolemAI::ActionState::ATTACK_ROTATE:	AttackRotateUpdate( target );	break;
	default: break;
	}
}

/*
XXXInit   : call by ChangeStatus when changing status. after XXXUninit.
XXXUpdate : call by UpdateCurrentStatus.
XXXUninit : call by ChangeStatus when changing status. before YYYInit.
*/

void Golem::WaitInit( TargetStatus target )
{
	status = GolemAI::ActionState::WAIT;

	slerpFactor = GolemParam::Get().SlerpFactor( status );

	velocity = 0.0f;

	setAnimFlame( models.pIdle.get(), 0 );
}
void Golem::WaitUpdate( TargetStatus target )
{

}
void Golem::WaitUninit()
{
	setAnimFlame( models.pIdle.get(), 0 );
}

void Golem::MoveInit( TargetStatus target )
{
	status = GolemAI::ActionState::MOVE;

	slerpFactor = GolemParam::Get().SlerpFactor( status );

	setAnimFlame( models.pIdle.get(), 0 );
}
void Golem::MoveUpdate( TargetStatus target )
{
	const float distNear  = GolemParam::Get().TargetDistNear();
	const float distFar   = GolemParam::Get().TargetDistFar();
	const float nDistance = CalcNormalizedDistance( target.pos );

	if ( distNear <= nDistance && nDistance <= distFar )
	{
		velocity = 0.0f;
		return;
	}
	// else

	const float speed = GolemParam::Get().MoveSpeed( status );
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
void Golem::MoveUninit()
{
	setAnimFlame( models.pIdle.get(), 0 );
}

void Golem::AttackSwingInit( TargetStatus target )
{
	status		= GolemAI::ActionState::ATTACK_SWING;
	timer		= GolemParam::Get().SwingStopFrame();
	swingTimer	= 0;
	slerpFactor	= GolemParam::Get().SlerpFactor( status );
	velocity	= 0.0f;

	ResetCurrentOBBFrames( GolemParam::Get().OBBAtksSwing() );
	setAnimFlame( models.pAtkSwing.get(), 0 );
	setStopAnimation( models.pAtkSwing.get(), /* is_stop = */ false );
}
void Golem::AttackSwingUpdate( TargetStatus target )
{
	// When beginning status.
	if ( 0 < timer )
	{
		timer--;
		if ( timer <= 0 )
		{
			// Start stop and spawn effects.

			constexpr float SEC_TO_FRAME = 1.0f / 60.0f;
			swingTimer = scast<int>( GolemParam::Get().SwingStopSecond() / SEC_TO_FRAME );
			if ( 0 < swingTimer )
			{
				setStopAnimation( models.pAtkSwing.get(), /* is_stop = */ true );
			}
		}
	}
	else // When stopping and spawn effects.
	if ( 0 < swingTimer )
	{
		swingTimer--;
		if ( swingTimer <= 0 )
		{
			// Finish stop and spawn effects.
			swingTimer = 0;
			setStopAnimation( models.pAtkSwing.get(), /* is_stop = */ false );
		}
	}
	else // When finish attacking status.
	{
		// No op.
	}

	bool nowStop = ( 0 < swingTimer );
	if ( !nowStop )
	{
		auto *pOBBs = GolemParam::Get().OBBAtksSwing();
		const size_t COUNT = pOBBs->size();
		for ( size_t i = 0; i < COUNT; ++i )
		{
			auto &OBB = pOBBs->at( i );
			OBB.Update();
		}
	}
}
void Golem::AttackSwingUninit()
{
	timer		= 0;
	swingTimer	= 0;

	ResetCurrentOBBFrames( GolemParam::Get().OBBAtksSwing() );
	setAnimFlame( models.pAtkSwing.get(), 0 );
	setStopAnimation( models.pAtkSwing.get(), /* is_stop = */ false );
}

void Golem::AttackFastInit( TargetStatus target )
{
	status = GolemAI::ActionState::ATTACK_FAST;

	slerpFactor = GolemParam::Get().SlerpFactor( status );

	ResetCurrentOBBFNames( GolemParam::Get().OBBFAtksFast() );
	setAnimFlame( models.pAtkFast.get(), 0 );
}
void Golem::AttackFastUpdate( TargetStatus target )
{
	velocity = orientation.LocalFront() * GolemParam::Get().MoveSpeed( status );

	auto *pOBBFNs = GolemParam::Get().OBBFAtksFast();
	const size_t COUNT = pOBBFNs->size();
	for ( size_t i = 0; i < COUNT; ++i )
	{
		auto &OBBFN = pOBBFNs->at( i );
		OBBFN.OBBF.Update();
	}
}
void Golem::AttackFastUninit()
{
	ResetCurrentOBBFNames( GolemParam::Get().OBBFAtksFast() );
	setAnimFlame( models.pAtkFast.get(), 0 );
}

void Golem::AttackRotateInit( TargetStatus target )
{
	status = GolemAI::ActionState::ATTACK_ROTATE;

	slerpFactor = GolemParam::Get().SlerpFactor( status );
	easeFactor  = 0.0f;
	extraOffset = 0.0f;

	ResetCurrentSphereFrames( GolemParam::Get().RotateAtkCollisions() );
	setAnimFlame( models.pAtkRotate.get(), 0 );
}
void Golem::AttackRotateUpdate( TargetStatus target )
{
	velocity = orientation.LocalFront() * GolemParam::Get().MoveSpeed( status );

	const int startLeaveFrame	= GolemParam::Get().RotLeaveStartFrame();
	const int currentFrame		= models.pAtkRotate->getAnimFlame();
	if ( startLeaveFrame <= currentFrame )
	{
		const auto &PARAM = GolemParam::Get();
		const float increaseSpeed = 1.0f / scast<float>( PARAM.RotLeaveWholeFrame() );

		easeFactor += increaseSpeed;
		if ( 1.0f <= easeFactor )
		{
			easeFactor = 1.0f;
		}

		float easePercent = Donya::Easing::Ease
		(
			scast<Donya::Easing::Kind>( PARAM.RotLeaveEaseKind() ),
			scast<Donya::Easing::Type>( PARAM.RotLeaveEaseType() ),
			easeFactor
		);
		extraOffset = -orientation.LocalFront() * ( PARAM.RotLeaveDistance() * easePercent );
	}

	auto *pSphereFs = GolemParam::Get().RotateAtkCollisions();
	const size_t COUNT = pSphereFs->size();
	for ( size_t i = 0; i < COUNT; ++i )
	{
		auto &sphereF = pSphereFs->at( i );
		sphereF.Update();
	}
}
void Golem::AttackRotateUninit()
{
	pos += extraOffset;

	easeFactor  = 0.0f;
	extraOffset = 0.0f;

	ResetCurrentSphereFrames( GolemParam::Get().RotateAtkCollisions() );
	setAnimFlame( models.pAtkRotate.get(), 0 );
}

void Golem::ApplyVelocity( TargetStatus target )
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

void Golem::CollideToWall()
{
	const float bodyRadius = GolemParam::Get().StageBodyRadius();
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

void Golem::UseImGui()
{
	if ( ImGui::BeginIfAllowed() )
	{
		if ( ImGui::TreeNode( "Golem.CurrentParameter" ) )
		{
			auto GetStatusName = []( GolemAI::ActionState status )->std::string
			{
				switch ( status )
				{
				case GolemAI::ActionState::WAIT:				return { "Wait" };			break;
				case GolemAI::ActionState::MOVE:				return { "Move" };			break;
				case GolemAI::ActionState::ATTACK_SWING:		return { "Attack.Swing" };	break;
				case GolemAI::ActionState::ATTACK_FAST:		return { "Attack.Fast" };	break;
				case GolemAI::ActionState::ATTACK_ROTATE:	return { "Attack.Rotate" };	break;
				default: break;
				}

				return { "Unexpected status" };
			};
			std::string statusCaption = "Status : " + GetStatusName( status );
			ImGui::Text( statusCaption.c_str() );
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
			ShowQuaternion( "Orientation", orientation );

			ImGui::TreePop();
		}

		ImGui::End();
	}
}

#endif // USE_IMGUI
