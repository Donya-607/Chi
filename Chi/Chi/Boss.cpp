#include "Boss.h"

#include "Donya/FilePath.h"
#include "Donya/Useful.h"		// For IsShowCollision().

#include "gameLib.h"
#include "gameLibFunctions.h"	// Use FBXRender().
#include "skinned_mesh.h"

#define scast static_cast

BossParam::BossParam() :
	scale( 1.0f ),
	initPosPerStage(), drawOffsetsPerStage(), hitBoxesBody(),
	OBBAttacksFast(), OBBAttacksSwing()
{
	initPosPerStage.resize( 1U );
	drawOffsetsPerStage.resize( 1U );
}
BossParam::~BossParam() = default;

void BossParam::Init()
{
	LoadParameter();
}
void BossParam::Uninit()
{
	// No op.
}

Donya::Vector3 BossParam::GetInitPosition( int stageNo ) const
{
	_ASSERT_EXPR( stageNo <= scast<int>( initPosPerStage.size() ), L"Error : Specified stage number over than stage count !" );

	return initPosPerStage[stageNo];
}
Donya::Vector3 BossParam::GetDrawOffset( int stageNo ) const
{
	_ASSERT_EXPR( stageNo <= scast<int>( drawOffsetsPerStage.size() ), L"Error : Specified stage number over than stage count !" );

	return drawOffsetsPerStage[stageNo];
}

void BossParam::LoadParameter( bool isBinary )
{
	Donya::Serializer::Extension ext = ( isBinary )
	? Donya::Serializer::Extension::BINARY
	: Donya::Serializer::Extension::JSON;
	std::string filePath = GenerateSerializePath( SERIAL_ID, ext );

	Donya::Serializer seria;
	seria.Load( ext, filePath.c_str(), SERIAL_ID, *this );
}

#if USE_IMGUI

void BossParam::SaveParameter()
{
	Donya::Serializer::Extension bin  = Donya::Serializer::Extension::BINARY;
	Donya::Serializer::Extension json = Donya::Serializer::Extension::JSON;
	std::string binPath  = GenerateSerializePath( SERIAL_ID, bin  );
	std::string jsonPath = GenerateSerializePath( SERIAL_ID, json );

	Donya::Serializer seria;
	seria.Save( bin,  binPath.c_str(),  SERIAL_ID, *this );
	seria.Save( json, jsonPath.c_str(), SERIAL_ID, *this );
}

void BossParam::UseImGui()
{
	if ( ImGui::BeginIfAllowed() )
	{
		if ( ImGui::TreeNode( "Boss.AdjustData" ) )
		{
			ImGui::SliderFloat( "Scale", &scale, 0.0f, 8.0f );

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
			
			if ( ImGui::TreeNode( "Collisions" ) )
			{
				auto ShowAABB	= []( const std::string &prefix, Donya::AABB		*pAABB	)
				{
					ImGui::DragFloat3( ( prefix + ".Offset" ).c_str(), &pAABB->pos.x );
					ImGui::DragFloat3( ( prefix + ".Scale" ).c_str(), &pAABB->size.x );
					ImGui::Checkbox( ( prefix + ".ExistCollision" ).c_str(), &pAABB->exist );
				};
				auto ShowSphere	= []( const std::string &prefix, Donya::Sphere		*pSphere)
				{
					ImGui::DragFloat3( ( prefix + ".Offset" ).c_str(), &pSphere->pos.x );
					ImGui::DragFloat( ( prefix + ".Scale" ).c_str(), &pSphere->radius );
					ImGui::Checkbox( ( prefix + ".ExistCollision" ).c_str(), &pSphere->exist );
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

				auto ShowAABBs	= [&ShowAABB]( const std::string &prefix, std::vector<Donya::AABB> *pAABBs )
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
				auto ShowOBBFs	= [&ShowOBBF]( const std::string &prefix, std::vector<Donya::OBBFrame> *pOBBFs )
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

				ShowOBBFs( "Attack.Fast",  &OBBAttacksFast  );
				ShowOBBFs( "Attack.Swing", &OBBAttacksSwing );

				ShowAABBs( "Body.HurtBox", &hitBoxesBody );

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

void ResetCurrentOBBFrame( std::vector<Donya::OBBFrame> *pOBBFs )
{
	const size_t COUNT = pOBBFs->size();
	for ( size_t i = 0; i < COUNT; ++i )
	{
		auto &OBBF = pOBBFs->at( i );
		OBBF.currentFrame = 0;
	}
}

Boss::Boss() :
	status( BossAI::ActionState::WAIT ),
	AI(),
	stageNo( 1 ),
	pos(), velocity(),
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
	};
	for ( auto &it : modelRefs )
	{
		InitializeModel( it );
	}
}
Boss::~Boss() = default;

void Boss::Init( int stageNumber )
{
	BossParam::Get().Init();

	LoadModel();

	stageNo = stageNumber;
	pos = BossParam::Get().GetInitPosition( stageNo );

	Donya::Vector3 dirToOrigin = Donya::Vector3::Zero() - pos;
	dirToOrigin.y = 0.0f;
	orientation = Donya::Quaternion::LookAt( orientation, dirToOrigin );

	AI.Init();
}
void Boss::Uninit()
{
	BossParam::Get().Uninit();
}

void Boss::Update()
{
#if USE_IMGUI

	BossParam::Get().UseImGui();
	UseImGui();

#endif // USE_IMGUI

	AI.Update();

	ChangeStatus();
	UpdateCurrentStatus();

	ApplyVelocity();
	CollideToWall();
}

void Boss::Draw( const Donya::Vector4x4 &matView, const Donya::Vector4x4 &matProjection )
{
	const auto &PARAM = BossParam::Get();

	Donya::Vector3 drawOffset = PARAM.GetDrawOffset( stageNo );

	Donya::Vector4x4 S = Donya::Vector4x4::MakeScaling( PARAM.Scale() );
	Donya::Vector4x4 R = orientation.RequireRotationMatrix();
	Donya::Vector4x4 T = Donya::Vector4x4::MakeTranslation( pos + drawOffset );
#if DEBUG_MODE
	if ( status == decltype( status )::END ) { R = Donya::Quaternion::Make( Donya::Vector3::Front(), ToRadian( 180.0f ) ).RequireRotationMatrix(); };
#endif // DEBUG_MODE
	Donya::Vector4x4 W = S * R * T;
	Donya::Vector4x4 WVP = W * matView * matProjection;

	switch ( status )
	{
	case BossAI::ActionState::WAIT:
		{
			FBXRender( models.pIdle.get(), WVP, W );
		}
		break;
	case BossAI::ActionState::MOVE:
		{
			FBXRender( models.pIdle.get(), WVP, W );
		}
		break;
	case BossAI::ActionState::ATTACK_SWING:
		{
			FBXRender( models.pAtkSwing.get(), WVP, W );
		}
		break;
	case BossAI::ActionState::ATTACK_FAST:
		{
			FBXRender( models.pAtkFast.get(), WVP, W );
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

	auto DrawCube	= [&]( const Donya::Vector3 &cubeOffset, const Donya::Vector3 &cubeScale, const Donya::Vector4 &color )
	{
		Donya::Vector4x4 CS = Donya::Vector4x4::MakeScaling( cubeScale * 2.0f ); // Half size->Whole size.
		Donya::Vector4x4 CT = Donya::Vector4x4::MakeTranslation( cubeOffset - drawOffset );
		Donya::Vector4x4 CW = ( CS * CT ) * W;
		Donya::Vector4x4 CWVP = CW * matView * matProjection;

		OBJRender( pCube.get(), CWVP, CW, color );
	};
	auto DrawSphere	= [&]( const Donya::Vector3 &sphereOffset, float sphereScale, const Donya::Vector4 &color )
	{
		Donya::Vector4x4 CS = Donya::Vector4x4::MakeScaling( sphereScale * 2.0f ); // Half size->Whole size.
		Donya::Vector4x4 CT = Donya::Vector4x4::MakeTranslation( sphereOffset - drawOffset );
		Donya::Vector4x4 CW = ( CS * CT ) * W;
		Donya::Vector4x4 CWVP = CW * matView * matProjection;

		OBJRender( pSphere.get(), CWVP, CW, color );
	};
	auto DrawOBB	= [&]( const Donya::OBB &OBB, const Donya::Vector4 &color )
	{
		Donya::Vector4x4 CS = Donya::Vector4x4::MakeScaling( OBB.size * 2.0f ); // Half size->Whole size.
		Donya::Vector4x4 CR = OBB.orientation.RequireRotationMatrix();
		Donya::Vector4x4 CT = Donya::Vector4x4::MakeTranslation( OBB.pos - drawOffset );
		Donya::Vector4x4 CW = ( CS * CR * CT ) * W;
		Donya::Vector4x4 CWVP = CW * matView * matProjection;

		OBJRender( pCube.get(), CWVP, CW, color );
	};

	if ( Donya::IsShowCollision() )
	{
		setBlendMode_ALPHA( 0.5f );

		constexpr Donya::Vector4 COLOR_BODY   { 0.0f, 0.5f, 0.0f, 0.6f };
		
		const auto *pAABBs = PARAM.BodyHitBoxes();
		for ( const auto &it : *pAABBs )
		{
			DrawCube( it.pos, it.size, COLOR_BODY );
		}

		constexpr Donya::Vector4 COLOR_VALID  { 1.0f, 0.8f, 0.4f, 0.6f };
		constexpr Donya::Vector4 COLOR_INVALID{ 0.4f, 0.4f, 1.0f, 0.6f };

		switch ( status )
		{
		case BossAI::ActionState::WAIT:
			break;
		case BossAI::ActionState::MOVE:
			break;
		case BossAI::ActionState::ATTACK_SWING:
			{
				const auto  *pOBBs = BossParam::Get().OBBAtksSwing();
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
		case BossAI::ActionState::ATTACK_FAST:
			{
				const auto  *pOBBs = BossParam::Get().OBBAtksFast();
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
		default: break;
		}
	}

#endif // DEBUG_MODE
}

std::vector<Donya::OBB> Boss::GetAttackHitBoxes() const
{
	const auto &PARAM = BossParam::Get();
	std::vector<Donya::OBB> collisions{};

	auto ToWorldOBB = [&]( Donya::OBB obb )
	{
		obb.pos += pos;
		obb.orientation.RotateBy( orientation );
		return obb;
	};

	switch ( status )
	{
	case BossAI::ActionState::WAIT:
		break;
	case BossAI::ActionState::MOVE:
		break;
	case BossAI::ActionState::ATTACK_SWING:
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
	case BossAI::ActionState::ATTACK_FAST:
		{
			const auto  *pOBBs = PARAM.OBBAtksFast();
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
	default: break;
	}

	return collisions;
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
std::vector<Donya::OBB> Boss::GetBodyHitBoxes() const
{
	const auto &PARAM = BossParam::Get();
	std::vector<Donya::OBB> collisions{};

	const auto *pAABBs = PARAM.BodyHitBoxes();
	for ( const auto &it : *pAABBs )
	{
		collisions.emplace_back( MakeOBB( it, pos, orientation ) );
	}

	return collisions;
}

void Boss::ReceiveImpact()
{
	status = decltype( status )::END;
}

void Boss::LoadModel()
{
	loadFBX( models.pIdle.get(),		GetModelPath( ModelAttribute::BossIdle ) );
	loadFBX( models.pAtkFast.get(),		GetModelPath( ModelAttribute::BossAtkFast ) );
	loadFBX( models.pAtkSwing.get(),	GetModelPath( ModelAttribute::BossAtkSwing ) );
}

void Boss::ChangeStatus()
{
	int lotteryStatus = AI.GetState();
	if ( status == lotteryStatus ) { return; }
	// else

	switch ( status )
	{
	case BossAI::ActionState::WAIT:			WaitUninit();		break;
	case BossAI::ActionState::MOVE:			MoveUninit();		break;
	case BossAI::ActionState::ATTACK_SWING:	AttackSwingUninit();break;
	case BossAI::ActionState::ATTACK_FAST:	AttackFastUninit();	break;
	default: break;
	}
	switch ( lotteryStatus )
	{
	case BossAI::ActionState::WAIT:			WaitInit();			break;
	case BossAI::ActionState::MOVE:			MoveInit();			break;
	case BossAI::ActionState::ATTACK_SWING:	AttackSwingInit();	break;
	case BossAI::ActionState::ATTACK_FAST:	AttackFastInit();	break;
	default: break;
	}

	status = scast<BossAI::ActionState>( lotteryStatus );
}
void Boss::UpdateCurrentStatus()
{
	switch ( status )
	{
	case BossAI::ActionState::WAIT:			WaitUpdate();			break;
	case BossAI::ActionState::MOVE:			MoveUpdate();			break;
	case BossAI::ActionState::ATTACK_SWING:	AttackSwingUpdate();	break;
	case BossAI::ActionState::ATTACK_FAST:	AttackFastUpdate();		break;
	default: break;
	}
}

/*
XXXInit : call by ChangeStatus when changing status. after XXXUninit.
XXXUpdate : call by UpdateCurrentStatus.
XXXUninit : call by ChangeStatus when changing status. before YYYInit.
*/

void Boss::WaitInit()
{
	status = BossAI::ActionState::WAIT;

	setAnimFlame( models.pIdle.get(), 0 );
}
void Boss::WaitUpdate()
{

}
void Boss::WaitUninit()
{
	setAnimFlame( models.pIdle.get(), 0 );
}

void Boss::MoveInit()
{
	status = BossAI::ActionState::MOVE;

#if DEBUG_MODE
	setAnimFlame( models.pIdle.get(), 0 );
#endif // DEBUG_MODE
}
void Boss::MoveUpdate()
{

}
void Boss::MoveUninit()
{
#if DEBUG_MODE
	setAnimFlame( models.pIdle.get(), 0 );
#endif // DEBUG_MODE
}

void Boss::AttackSwingInit()
{
	status = BossAI::ActionState::ATTACK_SWING;

	ResetCurrentOBBFrame( BossParam::Get().OBBAtksSwing() );
	setAnimFlame( models.pAtkSwing.get(), 0 );
}
void Boss::AttackSwingUpdate()
{
	auto *pOBBs = BossParam::Get().OBBAtksSwing();
	const size_t COUNT = pOBBs->size();
	for ( size_t i = 0; i < COUNT; ++i )
	{
		auto &OBB = pOBBs->at( i );
		OBB.Update();
	}
}
void Boss::AttackSwingUninit()
{
	ResetCurrentOBBFrame( BossParam::Get().OBBAtksSwing() );
	setAnimFlame( models.pAtkSwing.get(), 0 );
}

void Boss::AttackFastInit()
{
	status = BossAI::ActionState::ATTACK_FAST;

	ResetCurrentOBBFrame( BossParam::Get().OBBAtksFast() );
	setAnimFlame( models.pAtkFast.get(), 0 );
}
void Boss::AttackFastUpdate()
{
	auto *pOBBs = BossParam::Get().OBBAtksFast();
	const size_t COUNT = pOBBs->size();
	for ( size_t i = 0; i < COUNT; ++i )
	{
		auto &OBB = pOBBs->at( i );
		OBB.Update();
	}
}
void Boss::AttackFastUninit()
{
	ResetCurrentOBBFrame( BossParam::Get().OBBAtksFast() );
	setAnimFlame( models.pAtkFast.get(), 0 );
}

void Boss::ApplyVelocity()
{
	if ( velocity.IsZero() )
	{
		pos += velocity;
	}
}

void Boss::CollideToWall()
{

}

#if USE_IMGUI

void Boss::UseImGui()
{
	if ( ImGui::BeginIfAllowed() )
	{
		if ( ImGui::TreeNode( "Boss.CurrentParameter" ) )
		{
			auto GetStatusName = []( BossAI::ActionState status )->std::string
			{
				switch ( status )
				{
				case BossAI::ActionState::WAIT:			return { "Wait" };			break;
				case BossAI::ActionState::MOVE:			return { "Move" };			break;
				case BossAI::ActionState::ATTACK_SWING:	return { "AttackSwing" };	break;
				case BossAI::ActionState::ATTACK_FAST:	return { "AttackFast" };	break;
				default: break;
				}

				return { "Unsupported status" };
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
