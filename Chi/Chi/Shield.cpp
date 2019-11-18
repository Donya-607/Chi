#include "Shield.h"

#include "Donya/FilePath.h"
#include "Donya/Useful.h"	// Use ToRadian().

#include "GameLibFunctions.h"
#include "skinned_mesh.h"

#include "Effect.h"

#undef max
#undef min

#define scast static_cast

ShieldParam::ShieldParam() :
	m()
{}
ShieldParam::~ShieldParam() = default;

void ShieldParam::Init( size_t motionCount )
{
	m.motionSpeeds.resize( motionCount );
	LoadParameter();
}
void ShieldParam::Uninit()
{
	// No op.
}

ShieldParam::Member ShieldParam::Open()
{
	return Get().Content();
}

float ShieldParam::CalcUnfoldPercent( int currentTime )
{
	return scast<float>( currentTime ) / scast<float>( m.maxUnfoldableFrame );
}

void ShieldParam::LoadParameter( bool isBinary )
{
	Donya::Serializer::Extension ext = ( isBinary )
	? Donya::Serializer::Extension::BINARY
	: Donya::Serializer::Extension::JSON;
	std::string filePath = GenerateSerializePath( SERIAL_ID, ext );

	Donya::Serializer seria;
	seria.Load( ext, filePath.c_str(), SERIAL_ID, *this );
}

#if USE_IMGUI

void ShieldParam::SaveParameter()
{
	Donya::Serializer::Extension bin  = Donya::Serializer::Extension::BINARY;
	Donya::Serializer::Extension json = Donya::Serializer::Extension::JSON;
	std::string binPath  = GenerateSerializePath( SERIAL_ID, bin  );
	std::string jsonPath = GenerateSerializePath( SERIAL_ID, json );

	Donya::Serializer seria;
	seria.Save( bin,  binPath.c_str(),  SERIAL_ID, *this );
	seria.Save( json, jsonPath.c_str(), SERIAL_ID, *this );
}

void ShieldParam::UseImGui()
{
	if ( ImGui::BeginIfAllowed() )
	{
		if ( ImGui::TreeNode( "Shield.AdjustData" ) )
		{
			ImGui::DragInt( "Frame.MaxUnfoldableFrame", &m.maxUnfoldableFrame );
			ImGui::DragInt( "Frame.DecreaseSpeed", &m.decreaseSpeed );
			ImGui::DragInt( "Frame.IncreaseSpeed", &m.increaseSpeed );
			ImGui::DragInt( "Frame.When.Use.ConsumptionAmount", &m.consumptionAmount );
			ImGui::DragInt( "Frame.When.Protected.RecoveryAmount", &m.recoveryAmount );
			ImGui::Text( "" );

			ImGui::DragFloat ( "Draw.Scale.Maximum", &m.drawScaleMax );
			ImGui::DragFloat ( "Draw.Scale.Minimum", &m.drawScaleMin );
			ImGui::DragFloat3( "Draw.Offset", &m.drawOffset.x );
			ImGui::Text( "" );

			if ( ImGui::TreeNode( "Motion.Speed" ) )
			{
				const std::vector<std::string> motionNames
				{
					"Open",
					"Idle",
					"React",
				};
				const size_t COUNT = m.motionSpeeds.size();
				_ASSERT_EXPR( motionNames.size() == COUNT, L"Error : Logical error by human !" );
				for ( size_t i = 0; i < COUNT; ++i )
				{
					ImGui::DragFloat( motionNames[i].c_str(), &m.motionSpeeds[i], 0.005f );
				}

				ImGui::TreePop();
			}
			ImGui::Text( "" );

			if ( ImGui::TreeNode( "Collisions" ) )
			{
				auto ShowAABB = []( const std::string &prefix, Donya::AABB *pAABB )
				{
					ImGui::DragFloat3( ( prefix + ".Offset" ).c_str(), &pAABB->pos.x );
					ImGui::DragFloat3( ( prefix + ".Scale" ).c_str(), &pAABB->size.x );
					ImGui::Checkbox( ( prefix + ".ExistCollision" ).c_str(), &pAABB->exist );
				};
				auto ShowOBB  = []( const std::string &prefix, Donya::OBB *pOBB )
				{
					ImGui::DragFloat3( ( prefix + ".Offset" ).c_str(), &pOBB->pos.x );
					ImGui::DragFloat3( ( prefix + ".Scale" ).c_str(), &pOBB->size.x );

					Donya::Vector3 euler = pOBB->orientation.GetEulerAngles();
					ImGui::SliderFloat3( ( prefix + ".EulerAngle(Radian)" ).c_str(), &euler.x, ToRadian( -360.0f ), ToRadian( 360.0f ) );
					pOBB->orientation = Donya::Quaternion::Make( euler.x, euler.y, euler.z );

					ImGui::Checkbox( ( prefix + ".ExistCollision" ).c_str(), &pOBB->exist );
				};

				if ( ImGui::TreeNode( "Maximum Size" ) )
				{
					ShowAABB( "HitBox", &m.hitBoxMax );
					ImGui::TreePop();
				}
				if ( ImGui::TreeNode( "Minimum Size" ) )
				{
					ShowAABB( "HitBox", &m.hitBoxMin );
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

Shield::Shield() :
	status( State::NotExist ),
	unfoldTimer(), fluctuation(),
	currentMotion( Idle ),
	models(),
	nowUnfolding( false )
{
	auto InitializeModel = []( std::shared_ptr<skinned_mesh> *ppMesh )
	{
		*ppMesh = std::make_shared<skinned_mesh>();
		setAnimFlame( ppMesh->get(), 0 );
	};

	std::vector<std::shared_ptr<skinned_mesh> *> modelRefs
	{
		&models.pOpen,
		&models.pIdle,
		&models.pReact,
	};
	for ( auto &it : modelRefs )
	{
		InitializeModel( it );
	}
}
Shield::~Shield() = default;

void Shield::Init()
{
	ShieldParam::Get().Init( MOTION_COUNT );

	LoadModels();

	unfoldTimer = ShieldParam::Open().maxUnfoldableFrame;
}
void Shield::Uninit()
{
	ShieldParam::Get().Uninit();
}

void Shield::Update( bool isUnfolding )
{
#if USE_IMGUI

	ShieldParam::Get().UseImGui();
	UseImGui();

#endif // USE_IMGUI

	// I want detect the end after once call to FBXRender() from after change the motion.
	// Because the end flag of animation is update in FBXRender(), so if not call, the flag is not updated(will detected a "true" immediately).
	DetectEndAnimation();

	ApplyState( isUnfolding );

	Fluctuate();

	Elapse();
}

void Shield::Draw( fbx_shader &HLSL, const Donya::Vector4x4 &matView, const Donya::Vector4x4 &matProjection, const Donya::Vector4x4 &matParent )
{
	if ( status == State::NotExist ) { return; }
	// else

	const auto &PARAM = ShieldParam::Open();

	const Donya::Vector3 drawOffset = PARAM.drawOffset;
	const Donya::Vector4x4 DRAW_OFFSET = Donya::Vector4x4::MakeTranslation( drawOffset );

	auto CalcCurrentScale = []( int currentUnfoldTimer )
	{
		float	percent	= ShieldParam::Get().CalcUnfoldPercent( currentUnfoldTimer );
		float	max		= ShieldParam::Open().drawScaleMax;
		float	min		= ShieldParam::Open().drawScaleMin;
		float	diff	= max - min;

		return	min + ( diff * percent );
	};

	Donya::Vector4x4 S = Donya::Vector4x4::MakeScaling( CalcCurrentScale( unfoldTimer ) );
	Donya::Vector4x4 M = S * DRAW_OFFSET;
	Donya::Vector4x4 W = M * matParent;
	Donya::Vector4x4 WVP = W * matView * matProjection;

	float motionSpeed = ShieldParam::Open().motionSpeeds[scast<int>( currentMotion )];
	switch ( status )
	{
	case State::Open:
		FBXRender( models.pOpen.get(), HLSL, WVP, W, motionSpeed );
		break;
	case State::Idle:
		FBXRender( models.pIdle.get(), HLSL, WVP, W, motionSpeed );
		break;
	case State::React:
		FBXRender( models.pReact.get(), HLSL, WVP, W, motionSpeed );
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
	W = S * matParent;

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

		constexpr Donya::Vector4 COLOR{ 0.2f, 0.8f, 0.2f, 0.6f };
		Donya::OBB lsHitBox = GetHitBox( Donya::Quaternion::Identity() );

		DrawOBB( lsHitBox, COLOR );
	}

#endif // DEBUG_MODE
}

void Shield::z_Draw(fbx_shader& HLSL, const Donya::Vector4x4& matView, const Donya::Vector4x4& matProjection, const Donya::Vector4x4& matParent)
{
	if (status == State::NotExist) { return; }
	// else

	const auto& PARAM = ShieldParam::Open();

	const Donya::Vector3 drawOffset = PARAM.drawOffset;
	const Donya::Vector4x4 DRAW_OFFSET = Donya::Vector4x4::MakeTranslation(drawOffset);

	auto CalcCurrentScale = [](int currentUnfoldTimer)
	{
		float	percent = ShieldParam::Get().CalcUnfoldPercent(currentUnfoldTimer);
		float	max = ShieldParam::Open().drawScaleMax;
		float	min = ShieldParam::Open().drawScaleMin;
		float	diff = max - min;

		return	min + (diff * percent);
	};

	Donya::Vector4x4 S = Donya::Vector4x4::MakeScaling(CalcCurrentScale(unfoldTimer));
	Donya::Vector4x4 M = S * DRAW_OFFSET;
	Donya::Vector4x4 W = M * matParent;
	Donya::Vector4x4 WVP = W * matView * matProjection;

	switch (status)
	{
	case State::Open:
		z_render(models.pOpen.get(), HLSL, WVP, W);
		break;
	case State::Idle:
		z_render(models.pIdle.get(), HLSL, WVP, W);
		break;
	case State::React:
		z_render(models.pReact.get(), HLSL, WVP, W);
		break;
	default: break;
	}
}

void Shield::bloom_Draw(fbx_shader& HLSL, const Donya::Vector4x4& matView, const Donya::Vector4x4& matProjection, const Donya::Vector4x4& matParent)
{
	if (status == State::NotExist) { return; }
	// else

	const auto& PARAM = ShieldParam::Open();

	const Donya::Vector3 drawOffset = PARAM.drawOffset;
	const Donya::Vector4x4 DRAW_OFFSET = Donya::Vector4x4::MakeTranslation(drawOffset);

	auto CalcCurrentScale = [](int currentUnfoldTimer)
	{
		float	percent = ShieldParam::Get().CalcUnfoldPercent(currentUnfoldTimer);
		float	max = ShieldParam::Open().drawScaleMax;
		float	min = ShieldParam::Open().drawScaleMin;
		float	diff = max - min;

		return	min + (diff * percent);
	};

	Donya::Vector4x4 S = Donya::Vector4x4::MakeScaling(CalcCurrentScale(unfoldTimer));
	Donya::Vector4x4 M = S * DRAW_OFFSET;
	Donya::Vector4x4 W = M * matParent;
	Donya::Vector4x4 WVP = W * matView * matProjection;

	switch (status)
	{
	case State::Open:
		bloom_SRVrender(models.pOpen.get(), HLSL, WVP, W);
		break;
	case State::Idle:
		bloom_SRVrender(models.pIdle.get(), HLSL, WVP, W);
		break;
	case State::React:
		bloom_SRVrender(models.pReact.get(), HLSL, WVP, W);
		break;
	default: break;
	}
}

bool Shield::CanUnfold() const
{
	return ( 0 < unfoldTimer ) ? true : false;
}

Donya::AABB Shield::GetHitBox() const
{
	Donya::AABB maximum = ShieldParam::Open().hitBoxMax;
	Donya::AABB minimum = ShieldParam::Open().hitBoxMin;

	float percent = ShieldParam::Get().CalcUnfoldPercent( unfoldTimer );

	Donya::Vector3 diffPos  = maximum.pos  - minimum.pos;
	Donya::Vector3 diffSize = maximum.size - minimum.size;
	
	Donya::AABB localHitBox{};
	localHitBox.pos		= minimum.pos  + ( diffPos  * percent );
	localHitBox.size	= minimum.size + ( diffSize * percent );
	localHitBox.exist	= true;
	localHitBox.enable	= true;
	return localHitBox;
}
Donya::OBB Shield::GetHitBox( const Donya::Quaternion &parentOrientation ) const
{
	Donya::AABB localAABB = GetHitBox();

	Donya::OBB  localOBB{};
	localOBB.pos			= localAABB.pos;
	localOBB.size			= localAABB.size;
	localOBB.orientation	= parentOrientation;
	localOBB.exist			= localAABB.exist;
	localOBB.enable			= localAABB.enable;
	return localOBB;
}

void Shield::Recover()
{
	AddTimer( ShieldParam::Open().recoveryAmount );

	ChangeMotion( State::React );
}

void Shield::LoadModels()
{
	Donya::OutputDebugStr( "Begin Shield::LoadModels.\n" );

	loadFBX( models.pOpen.get(), GetModelPath( ModelAttribute::ShieldOpen ) );
	Donya::OutputDebugStr( "Done ShieldModel.Open.\n" );
	loadFBX( models.pIdle.get(), GetModelPath( ModelAttribute::ShieldIdle ) );
	Donya::OutputDebugStr( "Done ShieldModel.Idle.\n" );
	loadFBX( models.pReact.get(), GetModelPath( ModelAttribute::ShieldReact ) );
	Donya::OutputDebugStr( "Done ShieldModel.React.\n" );

	std::vector<std::shared_ptr<skinned_mesh> *> dontLoopModels
	{
		&models.pOpen,
		&models.pIdle,
		&models.pReact,
	};
	for ( auto &it : dontLoopModels )
	{
		setLoopFlg( it->get(), /* is_loop = */ false );
	}


	Donya::OutputDebugStr( "End Shield::LoadModels.\n" );
}

void Shield::AddTimer( int addition )
{
	unfoldTimer += addition;
	unfoldTimer = std::max( 0, std::min( ShieldParam::Open().maxUnfoldableFrame, unfoldTimer ) );
}

void Shield::ApplyState( bool isUnfolding )
{
	auto IsUnfoldTiming = []( bool previousIsUnfolding, bool currentIsUnfolding )->bool
	{
		return ( !previousIsUnfolding && currentIsUnfolding ) ? true : false;
	};
	if ( IsUnfoldTiming( nowUnfolding, isUnfolding ) )
	{
		fluctuation -= ShieldParam::Open().consumptionAmount;
		
		ChangeMotion( State::Open );
	}

	nowUnfolding = isUnfolding;
	if ( !nowUnfolding )
	{
		ChangeMotion( State::NotExist );
	}
}

void Shield::Fluctuate()
{
	( nowUnfolding )
	? fluctuation -= ShieldParam::Open().decreaseSpeed
	: fluctuation += ShieldParam::Open().increaseSpeed;
}

void Shield::Elapse()
{
	AddTimer( fluctuation );
	fluctuation = 0;
}

void Shield::ChangeMotion( State newStatus )
{
	status = newStatus;

	auto StopAllAnimation = []( Models *pModels )
	{
		auto Stop = []( std::shared_ptr<skinned_mesh> *ppMesh )
		{
			setStopAnimation( ppMesh->get(), /* is_stop = */ true );
		};
		std::vector<std::shared_ptr<skinned_mesh> *> stopModels
		{
			&pModels->pOpen,
			&pModels->pIdle,
			&pModels->pReact,
		};
		for ( auto &it : stopModels )
		{
			Stop( it );
		}
	};
	StopAllAnimation( &models );

	switch ( status )
	{
	case Shield::State::NotExist:
		currentMotion = Idle;
		setAnimFlame( models.pOpen.get(), 0 );
		return;
	case Shield::State::Open:
		currentMotion = Open;
		setAnimFlame	( models.pOpen.get(), 0 );
		setStopAnimation( models.pOpen.get(), /* is_stop = */ false );
		return;
	case Shield::State::Idle:
		currentMotion = Idle;
		setAnimFlame	( models.pIdle.get(), 0 );
		setStopAnimation( models.pIdle.get(), /* is_stop = */ false );
		return;
	case Shield::State::React:
		currentMotion = React;
		setAnimFlame	( models.pReact.get(), 0 );
		setStopAnimation( models.pReact.get(), /* is_stop = */ false );
		return;
	default: return;
	}
}

void Shield::DetectEndAnimation()
{
	if ( currentMotion == Idle || currentMotion == MOTION_COUNT ) { return; }
	// else

	if ( bool wasOpen = models.pOpen->getAnimFinFlg() || models.pReact->getAnimFinFlg() )
	{
		ChangeMotion( State::Idle );

		if ( wasOpen )
		{
			// EffectManager::GetInstance()->ShieldEffectSet();
		}
	}
}

#if USE_IMGUI

void Shield::UseImGui()
{
	if ( ImGui::BeginIfAllowed() )
	{
		if ( ImGui::TreeNode( "Shield.CurrentParameter" ) )
		{
			auto GetStatusName = []( State status )->std::string
			{
				switch ( status )
				{
				case State::NotExist:	return { "NotExist" };	// break;
				case State::Open:		return { "Open" };		// break;
				case State::Idle:		return { "Idle" };		// break;
				case State::React:		return { "React" };		// break;
				default: break;
				}

				return { "Unsupported status" };
			};
			std::string statusCaption = "Status : " + GetStatusName( status );
			ImGui::Text( statusCaption.c_str() );
			ImGui::Text( "Timer : %d", unfoldTimer );
			ImGui::Text( "NowUnfolding : %d", nowUnfolding ? 1 : 0 );
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

			ImGui::TreePop();
		}

		ImGui::End();
	}
}

#endif // USE_IMGUI
