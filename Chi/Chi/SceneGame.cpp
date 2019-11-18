#include "scene.h"
#include "sceneManager.h"

#include <vector>

#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>

#include "Donya/ChiUtility.h"
#include "Donya/Constant.h"		// Use DEBUG_MODE macro.
#include "Donya/FilePath.h"
#include "Donya/Keyboard.h"
#include "Donya/Serializer.h"
#include "Donya/Sound.h"
#include "Donya/Useful.h"		// Use show collision state.
#include "Donya/UseImGui.h"		// Use helper functions of ImGui.
#include "Donya/Vector.h"

#include "gameLib.h"
#include "light.h"

#include "Effect.h"
#include "Golem.h"
#include "Knight.h"
#include "Player.h"
#include "Rival.h"
#include "Stage.h"
#include "StorageForScene.h"

#define scast static_cast

#if DEBUG_MODE
constexpr int BGM_ID = 'BGM';
constexpr int SE_ID  = 'SE';

/*
HACK : I should put-together of the bosses(golem, knight, rival) to "BaseBoss" by inheritance.
*/

#endif // DEBUG_MODE

struct SceneGame::Impl
{
public:
	enum AppearStage
	{
		KnightNo = 0,
		GolemNo  = 1,
		RivalNo  = 2,

		STAGE_COUNT
	};
	static constexpr bool IsLastStage( int stageNo )
	{
		_ASSERT_EXPR( 0 <= stageNo && stageNo < AppearStage::STAGE_COUNT, L"Error : Unexpected stage number !" );
		return ( stageNo == AppearStage::STAGE_COUNT - 1 ) ? true : false;
	}
	enum class State
	{
		Battle,
		Win,
	};
public:
	State			status;
	int				stageNo;
	float			fieldRadius;
	float			cameraLeaveDistance;	// Leave from player.
	Donya::Vector3	cameraPos;				// The Y will be serialize.
	Donya::Vector3	cameraFocusOffset;
	Lights			lights;
	Player			player;
	Stage			stage;
	Knight			knight;
	Golem			golem;
	Rival			rival;
	fbx_shader		shader;
#if DEBUG_MODE

	std::vector<Donya::Box> wallCollisions{};

#endif // DEBUG_MODE
public:
	Impl() :
		status( State::Battle ),
		stageNo( AppearStage::KnightNo ),
		fieldRadius(), cameraLeaveDistance(),
		cameraPos(), cameraFocusOffset(),
		player(),
		stage(),
		knight(), golem(), rival(),
		lights(),
		shader()
	{}
	~Impl() = default;
private:
	friend class cereal::access;
	template<class Archive>
	void serialize( Archive &archive, std::uint32_t version )
	{
		archive
		(
			CEREAL_NVP( fieldRadius )
		);

		if ( 1 <= version )
		{
			archive( CEREAL_NVP( lights ) );
		}
		if ( 2 <= version )
		{
			archive( CEREAL_NVP( cameraFocusOffset ) );
		}
		if ( 3 <= version )
		{
			archive
			(
				CEREAL_NVP( cameraPos.y ),
				CEREAL_NVP( cameraLeaveDistance )
			);
		}
		if ( 4 <= version )
		{
			// archive( CEREAL_NVP( x ) );
		}
	}
	static constexpr const char* SERIAL_ID = "Game";
public:
	void Init()
	{
		LoadParameter();

		if ( !LoadSounds() )
		{
			_ASSERT_EXPR( 0, L"Failed : Loading sound." );
			exit( -1 );
		}
	#if DEBUG_MODE
		Donya::Sound::Load( BGM_ID, "./Data/Sounds/Test/BGM.wav", true  );
		Donya::Sound::Load( SE_ID,  "./Data/Sounds/Test/SE.wav",  false );
	#endif // DEBUG_MODE

		loadShader
		(
			shader,
			"./Data/shader/skinned_mesh_has_born_vs.cso",
			"./Data/shader/skinned_mesh_ps.cso",
			"./Data/shader/skinned_mesh_vs.cso",
			"./Data/shader/skinned_mesh_no_uv_ps.cso"
		);

		getLineLight().setLineLight(lights.direction.position, lights.direction.direction, lights.direction.color );

		resetPointLight();
		for ( const auto i : lights.points )
		{
			setPointLight( i );
		}

		FetchNowStageNumber();
	#if DEBUG_MODE
		// stageNo = AppearStage::GolemNo;
	#endif // DEBUG_MODE

		Donya::OutputDebugStr( "Begin Objects initialize.\n" );
		Donya::OutputDebugStr( "Objects count : 4\n" );

		Donya::OutputDebugStr( "No.1 Begin Player::Init.\n" );
		player.Init( Donya::Vector3::Zero(), Donya::Vector3::Zero(), std::vector<Donya::Box>(/* empty */) );
		player.SetFieldRadius( fieldRadius );
		Donya::OutputDebugStr( "No.1 End Player::Init.\n" );

		Donya::OutputDebugStr( "No.2 Begin Stage::Init.\n" );
		stage.Init( stageNo );
		Donya::OutputDebugStr( "No.2 End Stage::Init.\n" );

		Donya::OutputDebugStr( "No.3 Begin Boss::Init.\n" );
		Donya::Vector3 cameraTarget{};
		switch ( stageNo )
		{
			
		case KnightNo:	knight.Init( stageNo );		knight.SetFieldRadius( fieldRadius );		cameraTarget = knight.GetPos();	break;
		case GolemNo:	golem.Init( stageNo );		golem.SetFieldRadius( fieldRadius );		cameraTarget = golem.GetPos();	break;
		case RivalNo:	rival.Init( stageNo );		rival.SetFieldRadius( fieldRadius );		cameraTarget = rival.GetPos();	break;
		default:		Donya::OutputDebugStr( "Error : The boss does not initialize !\n" );	break;
		}
		Donya::OutputDebugStr( "No.3 End Boss::Init.\n" );
		
		Donya::OutputDebugStr( "No.4 Begin Effect::Init.\n" );
		EffectManager::GetInstance()->Init();
		Donya::OutputDebugStr( "No.4 End Effect::Init.\n" );

		Donya::OutputDebugStr( "End Objects initialize.\n" );

		// Set camera's position and focus.
		CameraUpdate( cameraTarget );
	}
	void Uninit()
	{
		stage.Uninit();
		player.Uninit();

		switch ( stageNo )
		{
		case KnightNo:	knight.Uninit();	break;
		case GolemNo:	golem.Uninit();		break;
		case RivalNo:	rival.Uninit();		break;
		default:		Donya::OutputDebugStr( "Error : The boss does not uninitialize !\n" );	break;
		}

		EffectManager::GetInstance()->UnInit();
	}

	void Update()
	{
	#if DEBUG_MODE

		if ( Donya::Keyboard::Press( VK_MENU ) )
		{
			if ( Donya::Keyboard::Trigger( 'C' ) )
			{
				char breakPoint = 0;
			}
			if ( Donya::Keyboard::Trigger( 'H' ) )
			{
				Donya::ToggleShowCollision();
			}
			if ( Donya::Keyboard::Trigger( 'T' ) || Donya::Keyboard::Trigger( 'I' ) )
			{
				Donya::ToggleShowStateOfImGui();
			}
		}

	#endif // DEBUG_MODE

		switch ( status )
		{
		case State::Battle:		BattleUpdate();		return;
		case State::Win:		WinUpdate();		return;
		default: return;
		}
	}

	void Draw()
	{
		clearWindow( 0.5f, 0.5f, 0.5f, 1.0f );
		setBlendMode_ALPHA( 1.0f );

		Donya::Vector4x4 V = Donya::Vector4x4::FromMatrix( getViewMatrix() );
		Donya::Vector4x4 P = Donya::Vector4x4::FromMatrix( getProjectionMatrix() );

		// original screen
		{
			stage.Draw( shader, V, P );

			player.Draw( shader, V, P );

			switch (stageNo)
			{
			case KnightNo:	knight.Draw( shader, V, P );	break;
			case GolemNo:	golem.Draw( shader, V, P );		break;
			case RivalNo:	rival.Draw( shader, V, P );		break;
			default:		Donya::OutputDebugStr("Error : The boss does not draw !\n");	break;
			}

			EffectManager::GetInstance()->Render( shader );

#if DEBUG_MODE
			if (!wallCollisions.empty())
			{
				constexpr float HEIGHT = 1024.0f;
				constexpr Donya::Vector4 COLOR = { 0.7f, 0.2f, 0.7f, 0.6f };

				auto GenerateCube = []()->std::shared_ptr<static_mesh>
				{
					std::shared_ptr<static_mesh> tmpCube = std::make_shared<static_mesh>();
					createCube(tmpCube.get());
					return tmpCube;
				};
				static std::shared_ptr<static_mesh> pCube = GenerateCube();

				auto DrawAABB = [&V, &P](const Donya::AABB& AABB, const Donya::Vector4& color)
				{
					Donya::Vector4x4 S = Donya::Vector4x4::MakeScaling(AABB.size * 2.0f); // Half size->Whole size.
					Donya::Vector4x4 T = Donya::Vector4x4::MakeTranslation(AABB.pos);
					Donya::Vector4x4 W = (S * T);
					Donya::Vector4x4 WVP = W * V * P;

					OBJRender(pCube.get(), WVP, W, color);
				};

				Donya::AABB wall{};
				wall.size.y = HEIGHT;
				for (const auto& it : wallCollisions)
				{
					wall.pos.x = it.cx;
					wall.pos.z = it.cy;
					wall.size.x = it.w;
					wall.size.z = it.h;

					DrawAABB(wall, COLOR);
				}
			}
#endif // DEBUG_MODE
		}

		// z screen
		if ( 1 )
		{
			stage.z_Draw(shader, V, P);

			player.DrawZ(shader, V, P);

			switch (stageNo)
			{
			case KnightNo:	knight.z_Draw(shader, V, P);	break;
			case GolemNo:	golem.z_Draw(shader, V, P);		break;
			case RivalNo:	rival.z_Draw(shader, V, P);		break;
			default:		Donya::OutputDebugStr("Error : The boss does not draw !\n");	break;
			}

			EffectManager::GetInstance()->z_Render(shader);
		}

		// bloom screen
		if ( 1 )
		{
			stage.bloom_Draw(shader, V, P);

			player.DrawBloom(shader, V, P);

			switch (stageNo)
			{
			case KnightNo:	knight.bloom_Draw(shader, V, P);	break;
			case GolemNo:	golem.bloom_Draw(shader, V, P);		break;
			case RivalNo:	rival.bloom_Draw(shader, V, P);		break;
			default:		Donya::OutputDebugStr("Error : The boss does not draw !\n");	break;
			}

			EffectManager::GetInstance()->bloom_Render(shader);
		}

		//TODO blur値入れる
		postEffect_Bloom(0);

		//TODO GameOver時は画面をモノトーンにするので第一引数の値を小さくする
		filterScreen(1.0f);
	}

public:
	bool LoadSounds() const
	{
		struct Bundle
		{
			MusicAttribute	id;
			bool			isEnableLoop;
		public:
			constexpr Bundle( MusicAttribute id, bool isEnableLoop ) : id( id ), isEnableLoop( isEnableLoop ) {}
		};

		constexpr std::array<Bundle, scast<int>( MusicAttribute::MUSIC_COUNT )> BUNDLES
		{
			{	// ID, isEnableLoop
				{ MusicAttribute::PlayerDefend,		false	},
				{ MusicAttribute::PlayerProtected,	false	},
			}
		};

		bool successed = true;
		for ( const auto &it : BUNDLES )
		{
			bool  result = Donya::Sound::Load( scast<int>( it.id ), GetMusicPath( it.id ), it.isEnableLoop );
			if ( !result ) { successed = false; }
		}
		return successed;
	}

	void FetchNowStageNumber()
	{
		using namespace StorageForScene;

		const Storage *pStorage = GetStorage();
		if ( !pStorage )
		{
			_ASSERT_EXPR( 0, L"Error : The storage is invalid !" );
			exit( -1 );
			return;
		}
		// else

		stageNo = pStorage->stageNo;
	}

	void BattleUpdate()
	{
		stage.Update();

		Player::Input  playerInput = Player::Input::MakeByExternalInput( Donya::Vector4x4::FromMatrix( GameLib::camera::GetViewMatrix() ) );
		player.Update( playerInput );

		BossUpdate();

		std::vector<Donya::Circle> bossBodies = FetchBossBodies();
		player.PhysicUpdate( bossBodies );

		CameraUpdate( GetCameraTargetPos() );

		EffectManager::GetInstance()->Update();

		ProcessCollision();

		if ( IsBossDefeated() )
		{
			status = State::Win;
		}
		else // TODO : Prevent the competition between the boss's defeat and player's defeat.
		if ( player.IsDefeated() )
		{
			SetStageNo( 0 );

			pSceneManager->setNextScene( new SceneGameOver, false );
		}
	}

	void WinUpdate()
	{
		stage.Update();

		Player::Input  playerInput = Player::Input::MakeByExternalInput( Donya::Vector4x4::FromMatrix( GameLib::camera::GetViewMatrix() ) );
		player.Update( playerInput );

		BossUpdate();

		CameraUpdate( GetCameraTargetPos() );

		EffectManager::GetInstance()->Update();

		bool shouldChangeScene{};
	#if DEBUG_MODE
		shouldChangeScene = true;
	#endif // DEBUG_MODE

		if ( shouldChangeScene )
		{
			if ( IsLastStage( stageNo ) )
			{
				SetStageNo( 0 );

				pSceneManager->setNextScene( new SceneResult( NULL ), false );
			}
			else
			{
				SetStageNo( stageNo + 1 );

				pSceneManager->setNextScene( new SceneGame(), false );
			}
		}
	}

	void BossUpdate()
	{
		switch ( stageNo )
		{
		case KnightNo:
			{
				Knight::TargetStatus target{};
				target.pos = player.GetPosition();
				knight.Update( target );
			}
			return;
		case GolemNo:
			{
				Golem::TargetStatus target{};
				target.pos = player.GetPosition();
				golem.Update( target );
			}
			return;
		case RivalNo:
			{
				Rival::TargetStatus target{};
				target.pos = player.GetPosition();
				rival.Update( target );
			}
			return;
		default: Donya::OutputDebugStr( "Error : The boss does not update !\n" ); break;
		}
	}
	std::vector<Donya::Circle> FetchBossBodies() const
	{
		auto ToCircle = []( const Donya::Sphere &wsSphere )
		{
			Donya::Circle xzBody{};
			xzBody.cx		= wsSphere.pos.x;
			xzBody.cy		= wsSphere.pos.z;
			xzBody.radius	= wsSphere.radius;
			xzBody.exist	= wsSphere.exist;
			return xzBody;
		};

		std::vector<Donya::Circle> bodies{};
		switch ( stageNo )
		{
		case KnightNo:
			{
				Donya::Sphere hitBox = knight.GetBodyHitBox();
				bodies.emplace_back( ToCircle( hitBox ) );
			}
			break;
		case GolemNo:
			{
				std::vector<Donya::Sphere> hitBoxes = golem.GetBodyHitBoxes();
				for ( const auto &it : hitBoxes )
				{
					bodies.emplace_back( ToCircle( it ) );
				}
			}
			break;
		case RivalNo:
			{
				Donya::AABB hitBox = rival.GetBodyHitBox();
				Donya::Sphere sphereVersion{};
				{
					Donya::Vector2 xzSize{ hitBox.size.x, hitBox.size.z };

					sphereVersion.pos		= hitBox.pos;
					sphereVersion.radius	= xzSize.Length();
					sphereVersion.exist		= hitBox.exist;
				}
				bodies.emplace_back( ToCircle( sphereVersion ) );
			}
			break;
		default: Donya::OutputDebugStr( "Error : The boss bodies does not fetched !\n" ); break;
		}

		return bodies;
	}
	bool IsBossDefeated() const
	{
		switch ( stageNo )
		{
		case KnightNo:	return knight.IsDefeated();
		case GolemNo:	return golem.IsDefeated();
		case RivalNo:	return rival.IsDefeated();
		default:		_ASSERT_EXPR( 0, "Error : Choosed unexpected boss!\n" ); break;
		}

		return false;
	}

	void SetStageNo( int setStageNumber )
	{
		StorageForScene::Storage storage{};
		storage.stageNo = setStageNumber;
		StorageForScene::SetStorage( storage );
	}

	Donya::Vector3 GetCameraTargetPos() const
	{
		Donya::Vector3 cameraTarget{};
		switch ( stageNo )
		{
		case GolemNo:	return golem.GetPos();	// break;
		case KnightNo:	return knight.GetPos();	// break;
		case RivalNo:	return rival.GetPos();	// break;
		default:		_ASSERT_EXPR( 0, "Error : The target of camera is not valid !\n" ); // break;
		}

		return Donya::Vector3::Zero();
	}
	void CameraUpdate( const Donya::Vector3 &targetPosition )
	{
		Donya::Vector3 playerPos = player.GetPosition();
		Donya::Vector3 targetPos = targetPosition;

		// DirectX::XMFLOAT3 player_to_target_vec = DirectX::XMFLOAT3( targetPos.x - playerPos.x, targetPos.y - playerPos.y, targetPos.z - playerPos.z );
		Donya::Vector3 player_to_target_vec = targetPos - playerPos;
		if ( player_to_target_vec.IsZero() )
		{
			// Prevent NaN when calculate Player's input.
			player_to_target_vec = -Donya::Vector3::Front();
		}
		float distPlayerBoss = player_to_target_vec.Length();
		Donya::Vector3 unitvec_player_to_target = player_to_target_vec.Normalized();

		cameraPos = Donya::Vector3
		{
			playerPos.x - unitvec_player_to_target.x * cameraLeaveDistance,
			cameraPos.y,
			playerPos.z - unitvec_player_to_target.z * cameraLeaveDistance
		};

		// TODO : 後々、変更するかも。
		float tanY = tanf( 90.0f );
		float cameraFocusY = tanY * ( distPlayerBoss / 8.0f );
		// DirectX::XMFLOAT3 cameraTarget = DirectX::XMFLOAT3( targetPos.x + unitvec_player_to_target.x, targetY, targetPos.z + unitvec_player_to_target.z );
		Donya::Vector3 cameraTarget = Donya::Vector3
		{
			targetPos.x + unitvec_player_to_target.x,
			cameraFocusY,
			targetPos.z + unitvec_player_to_target.z
		};

		GameLib::camera::setPos( cameraPos );
		GameLib::camera::setTarget( cameraTarget + cameraFocusOffset );
	}

	void ProcessCollision()
	{
		switch ( stageNo )
		{
		case KnightNo:	VS_Knight();	break;
		case GolemNo:	VS_Golem();		break;
		case RivalNo:	VS_Rival();		break;
		default:		Donya::OutputDebugStr( "Error : The boss does not uninitialize !\n" );	break;
		}
	}
	void VS_Knight()
	{
		const Donya::OBB playerBodyBox		= player.GetHurtBox();
		const Donya::OBB playerShieldBox	= player.GetShieldHitBox();
		const Donya::OBB playerAttackBox	= player.CalcAttackHitBox();

		// BossAttacks VS Player(and Player's Shield)
		{
			bool wasHitToShield = false;

			bool shieldCollided = knight.IsCollideAttackHitBoxes( playerShieldBox, /* disableCollidingHitBoxes = */ true );
			if ( shieldCollided )
			{
				wasHitToShield = true;
				player.SucceededDefence();
			}

			bool bodyCollided = ( shieldCollided ) ? false : knight.IsCollideAttackHitBoxes( playerBodyBox, /* disableCollidingHitBoxes = */ false );
			if ( bodyCollided )
			{
				player.ReceiveImpact();
			}
		}
		
		// PlayerAttack VS BossBodies
		{
			if ( Donya::OBB::IsHitSphere( playerAttackBox, knight.GetBodyHitBox() ) )
			{
				knight.ReceiveImpact();
			}
		}
	}
	void VS_Golem()
	{
		const Donya::OBB playerBodyBox   = player.GetHurtBox();
		const Donya::OBB playerShieldBox = player.GetShieldHitBox();
		const Donya::OBB playerAttackBox = player.CalcAttackHitBox();

		// BossAttacks VS Player(and Player's Shield)
		{
			bool wasHitToShield = false;

			auto &eruptionEffects = EffectManager::GetInstance()->GetEruptionEffectVector();
			for ( auto &effect : eruptionEffects )
			{
				auto &hitBoxes = effect.GetHitSphereVector();
				for ( auto &it : hitBoxes )
				{
					if ( Donya::OBB::IsHitSphere( playerShieldBox, it ) )
					{
						it.enable = false;
						wasHitToShield = true;
						player.SucceededDefence();
					}
					if ( !wasHitToShield && Donya::OBB::IsHitSphere( playerBodyBox, it ) )
					{
						player.ReceiveImpact();
					}

					wasHitToShield = false;
				}
			}

			bool shieldCollided = golem.IsCollideAttackHitBoxes( playerShieldBox, /* disableCollidingHitBoxes = */ true );
			if ( shieldCollided )
			{
				wasHitToShield = true;
				player.SucceededDefence();
			}

			bool bodyCollided = ( shieldCollided ) ? false : golem.IsCollideAttackHitBoxes( playerBodyBox, /* disableCollidingHitBoxes = */ false );
			if ( bodyCollided )
			{
				player.ReceiveImpact();
			}
		}
		
		// PlayerAttack VS BossBodies
		{
			const auto pBossBodies = golem.GetBodyHitBoxes();
			for ( const auto &it : pBossBodies )
			{
				if ( Donya::OBB::IsHitSphere( playerAttackBox, it ) )
				{
					golem.ReceiveImpact();
					break;
				}
			}
		}
	}
	void VS_Rival()
	{
		const Donya::OBB playerBodyBox		= player.GetHurtBox();
		const Donya::OBB playerShieldBox	= player.GetShieldHitBox();
		const Donya::OBB playerAttackBox	= player.CalcAttackHitBox();

		// BossAttacks VS Player(and Player's Shield)
		{
			bool wasHitToShield = false;

			bool shieldCollided = rival.IsCollideAttackHitBoxes( playerShieldBox, /* disableCollidingHitBoxes = */ true );
			if ( shieldCollided )
			{
				wasHitToShield = true;
				player.SucceededDefence();
				rival.WasDefended();
			}

			bool bodyCollided = ( shieldCollided ) ? false : rival.IsCollideAttackHitBoxes( playerBodyBox, /* disableCollidingHitBoxes = */ false );
			if ( bodyCollided )
			{
				player.ReceiveImpact();
			}

			wasHitToShield = false;

			auto &effects = EffectManager::GetInstance()->GetLongAttackEffectVector();
			for ( auto &effect : effects )
			{
				auto &hitBoxes = effect.GetHitSphereVector();
				for ( auto &it : hitBoxes )
				{
					if ( Donya::OBB::IsHitSphere( playerShieldBox, it ) )
					{
						it.enable		= false;
						wasHitToShield	= true;
						player.SucceededDefence();
						// rival.WasDefended(); // Unnecessary.
					}
					if ( !wasHitToShield && Donya::OBB::IsHitSphere( playerBodyBox, it ) )
					{
						player.ReceiveImpact();
					}

					wasHitToShield = false;
				}
			}
		}
		
		// PlayerAttack VS BossBodies
		{
			Donya::OBB wsRivalBody{};
			{
				Donya::AABB rivalAABB	= rival.GetBodyHitBox();
				wsRivalBody.pos			= rivalAABB.pos;
				wsRivalBody.size		= rivalAABB.size;
				wsRivalBody.orientation	= Donya::Quaternion::Identity();
				wsRivalBody.exist		= rivalAABB.exist;
				wsRivalBody.enable		= rivalAABB.enable;
			}
			if ( Donya::OBB::IsHitOBB( playerAttackBox, wsRivalBody ) )
			{
				rival.ReceiveImpact();
			}
		}
	}
public:
	void LoadParameter(bool isBinary = true)
	{
		Donya::Serializer::Extension ext = (isBinary)
			? Donya::Serializer::Extension::BINARY
			: Donya::Serializer::Extension::JSON;
		std::string filePath = GenerateSerializePath(SERIAL_ID, ext);

		Donya::Serializer seria;
		seria.Load(ext, filePath.c_str(), SERIAL_ID, *this);
	}

	void SaveParameter()
	{
		Donya::Serializer::Extension bin = Donya::Serializer::Extension::BINARY;
		Donya::Serializer::Extension json = Donya::Serializer::Extension::JSON;
		std::string binPath = GenerateSerializePath(SERIAL_ID, bin);
		std::string jsonPath = GenerateSerializePath(SERIAL_ID, json);

		Donya::Serializer seria;
		seria.Save(bin, binPath.c_str(), SERIAL_ID, *this);
		seria.Save(json, jsonPath.c_str(), SERIAL_ID, *this);
	}

	void UseImGui()
	{
#if USE_IMGUI

		if ( ImGui::BeginIfAllowed() )
		{
			if ( ImGui::Button( "GotoTitle" ) )
			{
				using namespace StorageForScene;
				Storage storage{};
				storage.stageNo = 0;
				SetStorage( storage );

				pSceneManager->setNextScene( new sceneTitle(), false );
			}
			ImGui::Text( "" );

			if ( ImGui::TreeNode( "Game" ) )
			{
				ImGui::Text( "Show.Press \'ALT\' and \'I\' or \'T'\'key : Toggle ImGui ON/OFF" );
				ImGui::Text( "Show.Press \'ALT\' and \'H\'key : Toggle Collision ON/OFF" );
				ImGui::Text( "" );

			#if DEBUG_MODE

				ImGui::Text( "Is the Rival defeated ? : %d", ( rival.IsDefeated() ) ? 1 : 0 );
				ImGui::Text( "" );

			#endif // DEBUG_MODE

				if ( ImGui::TreeNode( "Sound test" ) )
				{
					if ( ImGui::Button( "BGM.Play"   ) ) { Donya::Sound::Play  ( BGM_ID ); }
					if ( ImGui::Button( "BGM.Pause"  ) ) { Donya::Sound::Pause ( BGM_ID ); }
					if ( ImGui::Button( "BGM.Resume" ) ) { Donya::Sound::Resume( BGM_ID ); }
					if ( ImGui::Button( "BGM.Stop"   ) ) { Donya::Sound::Stop  ( BGM_ID ); }
					ImGui::Text( "" );
					if ( ImGui::Button( "SE.Play"    ) ) { Donya::Sound::Play  ( SE_ID  ); }
					if ( ImGui::Button( "SE.Pause"   ) ) { Donya::Sound::Pause ( SE_ID  ); }
					if ( ImGui::Button( "SE.Resume"  ) ) { Donya::Sound::Resume( SE_ID  ); }
					if ( ImGui::Button( "SE.Stop"    ) ) { Donya::Sound::Stop  ( SE_ID  ); }

					ImGui::TreePop();
				}

				if ( ImGui::TreeNode( "Configuration" ) )
				{
					ImGui::DragFloat( "Field.Radius", &fieldRadius ); player.SetFieldRadius( fieldRadius );
					ImGui::Text( "" );

					ImGui::DragFloat ( "Camera.Position.Y",		&cameraPos.y );
					ImGui::DragFloat ( "Camera.LeaveDistance",	&cameraLeaveDistance );
					ImGui::DragFloat3( "Camera.FocusOffset",	&cameraFocusOffset.x );

					ImGui::TreePop();
				}

				if ( ImGui::TreeNode( "Light" ) )
				{
					if ( ImGui::TreeNode( "DirectionalLight" ) )
					{
						ImGui::ColorEdit4( "Color", &lights.direction.color.x );
						ImGui::SliderFloat3( "Direction", &lights.direction.direction.x, -8.0f, 8.0f );

						getLineLight().setLineLight(lights.direction.position, lights.direction.direction, lights.direction.color );

						ImGui::TreePop();
					}

					if ( ImGui::TreeNode( "PointLight" ) )
					{
						constexpr size_t MAX_COUNT = 4U;
						bool canAppend  = ( lights.points.size() < MAX_COUNT ) ? true : false;
						bool canPopBack = ( 1U <= lights.points.size() ) ? true : false;
						if ( canAppend  && ImGui::Button( "Append" ) )
						{
							PointLight add{};
							add.pos   = { 0.0f, 0.0f, 0.0f, 1.0f };
							add.color = { 1.0f, 1.0f, 1.0f, 1.0f };
							lights.points.push_back( add );
							setPointLight( add );
						}
						if ( canPopBack && ImGui::Button( "PopBack" ) )
						{
							lights.points.pop_back();
							removePointLight( scast<int>( lights.points.size() ) );
						}

						const size_t COUNT = lights.points.size();
						for ( size_t i = 0; i < COUNT; ++i )
						{
							auto& point = lights.points[i];
							ImGui::DragFloat3( std::string( "Position" + std::to_string( i ) ).c_str(), &point.pos.x ); point.pos.w = 1.0f;
							ImGui::ColorEdit4( std::string( "Color" + std::to_string( i ) ).c_str(), &point.color.x );
							ImGui::SliderFloat4( std::string( "Attenuate" + std::to_string( i ) ).c_str(), &point.attenuate.x, 0.0f, 1.0f );

							setPointlightInfo( scast<int>( i ), point );
						}

						ImGui::TreePop();
					}

					ImGui::TreePop();
				}

			#if DEBUG_MODE

				if ( ImGui::TreeNode( "Player's Wall Collisions" ) )
				{
					if ( ImGui::Button( "Assign to player" ) )
					{
						player.ReplaceWallCollisions( wallCollisions );
					}

					bool canPopBack = ( 1U <= wallCollisions.size() ) ? true : false;
					if ( ImGui::Button( "Append" ) )
					{
						Donya::Box append{};
						append.cx = append.cy = 0.0f;
						append.w  = append.h  = 1.0f;
						append.exist = true;
						wallCollisions.push_back( append  );
					}
					if ( canPopBack && ImGui::Button( "PopBack" ) )
					{
						wallCollisions.pop_back();
					}

					const size_t COUNT = wallCollisions.size();
					for ( size_t i = 0; i < COUNT; ++i )
					{
						auto &wall = wallCollisions[i];
						ImGui::DragFloat2( ( "Position.XZ." + std::to_string( i ) ).c_str(), &wall.cx );
						ImGui::DragFloat2( ( "Size.XZ." + std::to_string( i ) ).c_str(), &wall.w );
						ImGui::Text( "" );
					}

					ImGui::TreePop();
				}

			#endif // DEBUG_MODE

				if ( ImGui::TreeNode( "File.I/O" ) )
				{
					static bool isBinary = true;
					if ( ImGui::RadioButton( "Binary", isBinary ) ) { isBinary = true; }
					if ( ImGui::RadioButton( "JSON", !isBinary ) ) { isBinary = false; }
					std::string loadStr{ "Load " };
					loadStr += (isBinary) ? "Binary" : "JSON";

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

#endif // USE_IMGUI
	}
};
CEREAL_CLASS_VERSION( SceneGame::Impl, 3 )

SceneGame::SceneGame() : baseScene(), pImpl(std::make_unique<Impl>())
{}
SceneGame::~SceneGame()
{
	pImpl.reset( nullptr );
}

void SceneGame::init()
{
	isStack = false; // Is this necessary ?

	pImpl->Init();
}
void SceneGame::uninit()
{
	pImpl->Uninit();
}

void SceneGame::update()
{
	pImpl->Update();
}

void SceneGame::render()
{
	pImpl->Draw();
}

void SceneGame::imGui()
{
	pImpl->UseImGui();
}
