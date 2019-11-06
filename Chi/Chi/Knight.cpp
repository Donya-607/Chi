#include "Knight.h"

#include "Donya/Easing.h"
#include "Donya/FilePath.h"
#include "Donya/Useful.h"		// For IsShowCollision().

#include "gameLib.h"
#include "gameLibFunctions.h"	// Use FBXRender().
#include "skinned_mesh.h"

#undef max
#undef min

#define scast static_cast

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

			ImGui::DragFloat( "MoveSpeed.\"Move\"State", &m.moveMoveSpeed );
			ImGui::Text( "" );

			ImGui::SliderFloat( "SlerpFactor.\"Idle\"State", &m.idleSlerpFactor, 0.0f, 1.0f );
			ImGui::SliderFloat( "SlerpFactor.\"Move\"State", &m.moveSlerpFactor, 0.0f, 1.0f );
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
				
				ShowSphere( "Body.HurtBox", &m.hitBoxBody );

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

Knight::Knight() :
	status( KnightAI::ActionState::WAIT ),
	AI(),
	timer(),
	fieldRadius(), slerpFactor( 1.0f ),
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

void Knight::Draw( const Donya::Vector4x4 &matView, const Donya::Vector4x4 &matProjection )
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
		FBXRender( models.pIdle.get(), WVP, W );
		break;
	case KnightAI::ActionState::MOVE:
		FBXRender( models.pIdle.get(), WVP, W );
		break;
	case KnightAI::ActionState::ATTACK_EXPLOSION:
		FBXRender( models.pIdle.get(), WVP, W );
		break;
	case KnightAI::ActionState::ATTACK_SWING:
		FBXRender( models.pIdle.get(), WVP, W );
		break;
	case KnightAI::ActionState::ATTACK_RAID:
		FBXRender( models.pIdle.get(), WVP, W );
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

	auto DrawCube = [&]( const Donya::Vector3 &cubeOffset, const Donya::Vector3 &cubeScale, const Donya::Quaternion &orientation, const Donya::Vector4 &color )
	{
		Donya::Vector4x4 CS = Donya::Vector4x4::MakeScaling( cubeScale * 2.0f ); // Half size->Whole size.
		Donya::Vector4x4 CR = orientation.RequireRotationMatrix();
		Donya::Vector4x4 CT = Donya::Vector4x4::MakeTranslation( cubeOffset );
		Donya::Vector4x4 CW = ( CS * CR * CT ) * W;
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
			break;
		case KnightAI::ActionState::ATTACK_RAID:
			break;
		default: break;
		}
	}

#endif // DEBUG_MODE
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
	loadFBX( models.pIdle.get(), GetModelPath( ModelAttribute::KnightIdle ) );
}

float Knight::CalcNormalizedDistance( Donya::Vector3 wsTargetPos )
{
	float distance = ( wsTargetPos - GetPos() ).Length();
	return ( ZeroEqual( fieldRadius ) )
	? fieldRadius
	: distance / fieldRadius;
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

	setAnimFlame( models.pIdle.get(), 0 );
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
	setAnimFlame( models.pIdle.get(), 0 );
}

void Knight::AttackExplosionInit( TargetStatus target )
{
	status = KnightAI::ActionState::ATTACK_EXPLOSION;

	slerpFactor = KnightParam::Get().SlerpFactor( status );

	// setAnimFlame( models.pAtkFast.get(), 0 );
}
void Knight::AttackExplosionUpdate( TargetStatus target )
{
	velocity = orientation.LocalFront() * KnightParam::Get().MoveSpeed( status );
}
void Knight::AttackExplosionUninit()
{
	// setAnimFlame( models.pAtkFast.get(), 0 );
}

void Knight::AttackSwingInit( TargetStatus target )
{
	status		= KnightAI::ActionState::ATTACK_SWING;;
	slerpFactor	= KnightParam::Get().SlerpFactor( status );
	velocity	= 0.0f;

	// setAnimFlame( models.pAtkSwing.get(), 0 );
}
void Knight::AttackSwingUpdate( TargetStatus target )
{

}
void Knight::AttackSwingUninit()
{
	timer = 0;

	// setAnimFlame( models.pAtkSwing.get(), 0 );
}

void Knight::AttackRaidInit( TargetStatus target )
{
	status = KnightAI::ActionState::ATTACK_RAID;

	slerpFactor = KnightParam::Get().SlerpFactor( status );
	extraOffset = 0.0f;

	// setAnimFlame( models.pAtkRotate.get(), 0 );
}
void Knight::AttackRaidUpdate( TargetStatus target )
{
#if DEBUG_MODE
	extraOffset = orientation.LocalFront() * KnightParam::Get().MoveSpeed( status );
#endif // DEBUG_MODE
}
void Knight::AttackRaidUninit()
{
	pos += extraOffset;

	extraOffset = 0.0f;

	// setAnimFlame( models.pAtkRotate.get(), 0 );
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
