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

#include "Golem.h"
#include "Knight.h"
#include "Player.h"
#include "Stage.h"

#define scast static_cast

#if DEBUG_MODE
constexpr int BGM_ID = 'BGM';
constexpr int SE_ID  = 'SE';
#endif // DEBUG_MODE

struct SceneGame::Impl
{
public:
	float			fieldRadius;
	float			cameraLeaveDistance;	// Leave from player.
	Donya::Vector3	cameraPos;				// The Y will be serialize.
	Donya::Vector3	cameraFocusOffset;
	Lights			lights;
	Player			player;
	Stage			stage;
	// Golem			boss;
	Knight			boss;
public:
	Impl() :
		fieldRadius(), cameraLeaveDistance(),
		cameraPos(), cameraFocusOffset(),
		player(),
		stage(),
		boss(),
		lights()
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
	static constexpr const char *SERIAL_ID = "Game";
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

		getLineLight().setLineLight( lights.direction.direction, lights.direction.color );

		resetPointLight();
		for ( const auto i : lights.points )
		{
			setPointLight( i );
		}

	#if DEBUG_MODE
		// createCube( &testCube );

		constexpr int STAGE_NO = NULL;
	#endif // DEBUG_MODE

		Donya::OutputDebugStr( "Begin Objects initialize.\n" );
		Donya::OutputDebugStr( "Objects count : 3\n" );

		Donya::OutputDebugStr( "No.1 Begin Player::Init.\n" );
		player.Init();
		player.SetFieldRadius( fieldRadius );
		Donya::OutputDebugStr( "No.1 End Player::Init.\n" );

		Donya::OutputDebugStr( "No.2 Begin Stage::Init.\n" );
		stage.Init( STAGE_NO );
		Donya::OutputDebugStr( "No.2 End Stage::Init.\n" );

		Donya::OutputDebugStr( "No.3 Begin Boss::Init.\n" );
		boss.Init( STAGE_NO );
		boss.SetFieldRadius( fieldRadius );
		Donya::OutputDebugStr( "No.3 End Boss::Init.\n" );

		Donya::OutputDebugStr( "End Objects initialize.\n" );

		// Set camera's position and focus.
		CameraUpdate();
	}
	void Uninit()
	{
		stage.Uninit();
		player.Uninit();
		boss.Uninit();
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

		stage.Update();

		auto MakePlayerInput = []( Donya::Vector4x4 viewMat )->Player::Input
		{
			Player::Input input{};

		#if DEBUG_MODE
			
			if ( Donya::Keyboard::Press( VK_UP    ) ) { input.moveVector.z = +1.0f; }
			if ( Donya::Keyboard::Press( VK_DOWN  ) ) { input.moveVector.z = -1.0f; }
			if ( Donya::Keyboard::Press( VK_LEFT  ) ) { input.moveVector.x = -1.0f; }
			if ( Donya::Keyboard::Press( VK_RIGHT ) ) { input.moveVector.x = +1.0f; }

			if ( Donya::Keyboard::Trigger( 'Z' ) ) { input.doDefend = true; }
			if ( Donya::Keyboard::Trigger( 'X' ) ) { input.doAttack = true; }
			
			// XINPUT_GAMEPAD : https://docs.microsoft.com/ja-jp/windows/win32/api/xinput/ns-xinput-xinput_gamepad

			// TODO : コントローラーがあるか判定する
			{
				constexpr int   PAD_NO = 0;
				constexpr float STICK_RANGE_MAX = 32768.0f;
				const auto leftStick = GameLib::input::xInput::getThumbL( PAD_NO );
				if ( leftStick.x != 0 ) { input.moveVector.x = scast<float>( leftStick.x ) / STICK_RANGE_MAX; }
				if ( leftStick.y != 0 ) { input.moveVector.z = scast<float>( leftStick.y ) / STICK_RANGE_MAX; }

				constexpr int TRIGGER_FLAG = 1;
				if ( GameLib::input::xInput::pressedButtons( PAD_NO, XboxPad_Button::RIGHT_THUMB ) == TRIGGER_FLAG ) { input.doDefend = true; }
				if ( GameLib::input::xInput::pressedButtons( PAD_NO, XboxPad_Button::X           ) == TRIGGER_FLAG ) { input.doAttack = true; }
			}

		#endif // DEBUG_MODE

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
		};
		player.Update( MakePlayerInput( Donya::Vector4x4::FromMatrix( GameLib::camera::GetViewMatrix() ) ) );

		// Golem::TargetStatus bossTarget{};
		Knight::TargetStatus bossTarget{};
		bossTarget.pos = player.GetPosition();
		boss.Update( bossTarget );
		
		CameraUpdate();

		ProcessCollision();
		
	#if DEBUG_MODE
		/*
		if (getKeyState(KEY_INPUT_1) == 1)
		{
			setAnimFlame(&animTest, 0);
		}
		if (getKeyState(KEY_INPUT_2) == 1)
		{
			setAnimFlame(&animTest, 20);
		}
		if (getKeyState(KEY_INPUT_SPACE))
		{
			//setStopTime(&animTest,1);
			setStopAnimation(&animTest, true);
		}
		else
		{
			setStopAnimation( &animTest, false );
		}
		*/
	#endif // DEBUG_MODE
	}

	void Draw()
	{
		clearWindow( 0.1f, 0.1f, 0.1f, 1.0f );
		setBlendMode_ALPHA( 1.0f );

		Donya::Vector4x4 V = Donya::Vector4x4::FromMatrix( getViewMatrix() );
		Donya::Vector4x4 P = Donya::Vector4x4::FromMatrix( getProjectionMatrix() );

		stage.Draw( V, P );

		player.Draw( V, P );

		boss.Draw( V, P );

	#if DEBUG_MODE
		/*
		if ( 0 )
		{
			XMFLOAT4X4 World, world_view_projection;
			DirectX::XMMATRIX worldM;
			DirectX::XMMATRIX worldcubeM;
			DirectX::XMMATRIX S, R, Rx, Ry, Rz, T;
			auto plPos = player.GetPosition();
			T = DirectX::XMMatrixTranslation( plPos.x, plPos.y, plPos.z );
			worldM = T;

			//	Matrix -> Float4x4 変換
			DirectX::XMStoreFloat4x4( &world_view_projection, worldM * getViewMatrix() * getProjectionMatrix() );
			DirectX::XMStoreFloat4x4( &World, worldM );
			
			FBXRender( &animTest, world_view_projection, World );

			DirectX::XMFLOAT3 pos = { -10,10,100 };
			worldcubeM = DirectX::XMMatrixIdentity();

			//	拡大・縮小
			S = DirectX::XMMatrixScaling( 10, 10, 10 );

			//	回転
			Rx = DirectX::XMMatrixRotationX( 0 );				//	X軸を基準とした回転行列
			Ry = DirectX::XMMatrixRotationY( 0 );				//	Y軸を基準とした回転行列
			Rz = DirectX::XMMatrixRotationZ( 0 );				//	Z軸を基準とした回転行列
			R = Rz * Rx * Ry;

			//	平行移動
			calcTransformedPosBySpecifyMesh( &animTest, pos, "_04_MDL_yoroi1:_02_MDL_yoroi:spear" );
			T = DirectX::XMMatrixTranslation( pos.x, pos.y, pos.z );

			//	ワールド変換行列
			worldcubeM = S * R * T * worldM;

			//	Matrix -> Float4x4 変換
			DirectX::XMStoreFloat4x4( &world_view_projection, worldcubeM * getViewMatrix() * getProjectionMatrix() );
			DirectX::XMStoreFloat4x4( &World, worldcubeM );

			OBJRender( &testCube, world_view_projection, World );
		}
		*/
	#endif // DEBUG_MODE
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

	void CameraUpdate()
	{
	#if 0
		DirectX::XMFLOAT3 originVec( 0.0f, 0.0f, 1.0f );
		DirectX::XMFLOAT3 boss_Player_Vec( player.GetPosition().x - boss.pos.x, player.GetPosition().y - boss.pos.y, player.GetPosition().z - boss.pos.z );
		DirectX::XMFLOAT3 _boss_Player_Vec( player.GetPosition().x - boss.pos.x, player.GetPosition().y - boss.pos.y, player.GetPosition().z - boss.pos.z );

		float l1, l2;
		l1 = sqrtf( ( originVec.x * originVec.x ) + ( originVec.y * originVec.y ) + ( originVec.z * originVec.z ) );
		l2 = sqrtf( ( boss_Player_Vec.x * boss_Player_Vec.x ) + 0.0f + ( boss_Player_Vec.z * boss_Player_Vec.z ) );
		boss_Player_Vec.x /= l2;
		boss_Player_Vec.y /= l2;
		boss_Player_Vec.z /= l2;

		float dot = 0.0f;
		float angle = 0.0f;
		l2 = sqrtf( ( boss_Player_Vec.x * boss_Player_Vec.x ) + 0.0f + ( boss_Player_Vec.z * boss_Player_Vec.z ) );
		dot = ( originVec.x * boss_Player_Vec.x ) + 0.0f + ( originVec.z * boss_Player_Vec.z );
		angle = acos( dot / ( l1 * l2 ) ) / ( 3.141592f / 180.0f );

		if ( _boss_Player_Vec.x < 0 )
		{
			angle *= -1;
		}
		float distance = 1000.0f;
		DirectX::XMFLOAT3 _player_pos = player.GetPosition();
		cameraPos.x = _player_pos.x + sinf( angle * 0.01745f ) * distance;
		cameraPos.y = 400.0f;
		cameraPos.z = _player_pos.z + cosf( angle * 0.01745f ) * distance;
	#elif 0
		Donya::Vector3 boss_Player_Vec( player.GetPosition().x - boss.pos.x, 0.0f, player.GetPosition().z - boss.pos.z );
		Donya::Vector3 boss_Player_Vec_N = boss_Player_Vec;
		boss_Player_Vec_N.Normalize();

		float add_length = 1000.0f;

		cameraPos.x = boss_Player_Vec_N.x * ( boss_Player_Vec.Length() + add_length );
		cameraPos.y = player.GetPosition().y + add_length;
		cameraPos.z = boss_Player_Vec_N.z * ( boss_Player_Vec.Length() + add_length );
	#else
		Donya::Vector3 playerPos = player.GetPosition();
		Donya::Vector3 targetPos = boss.GetPos();

		// 単位ベクトル取得
		// DirectX::XMFLOAT3 player_to_target_vec = DirectX::XMFLOAT3( targetPos.x - playerPos.x, targetPos.y - playerPos.y, targetPos.z - playerPos.z );
		Donya::Vector3 player_to_target_vec = targetPos - playerPos;
		if ( player_to_target_vec.IsZero() )
		{
			// Prevent NaN when calculate Player's input.
			player_to_target_vec = -Donya::Vector3::Front();
		}
		float distPlayerBoss = player_to_target_vec.Length();
		Donya::Vector3 unitvec_player_to_target = player_to_target_vec.Normalized();

		//カメラ位置取得
		cameraPos = Donya::Vector3
		{
			playerPos.x - unitvec_player_to_target.x * cameraLeaveDistance,
			cameraPos.y,
			playerPos.z - unitvec_player_to_target.z * cameraLeaveDistance
		};

		//注視点取得
	#if 0
		DirectX::XMFLOAT3 cameraTarget = DirectX::XMFLOAT3( targetPos.x + unitvec_player_to_target.x, targetPos.y + unitvec_player_to_target.y, targetPos.z + unitvec_player_to_target.z );
	#else
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
	#endif

	#endif
		GameLib::camera::setPos( cameraPos );
		GameLib::camera::setTarget( cameraTarget + cameraFocusOffset );
	}

	void ProcessCollision()
	{
		const Donya::OBB playerBodyBox   = player.GetHurtBox();
		const Donya::OBB playerShieldBox = player.GetShieldHitBox();
		const Donya::OBB playerAttackBox = player.CalcAttackHitBox();

		//// BossAttacks VS Player(and Player's Shield)
		//{
		//	bool wasHitToShield = false;

		//#if FETCH_BOXES_THEN_JUDGE

		//	const auto attackBoxes = boss.RequireAttackHitBoxesOBB();
		//	for ( const auto &it : attackBoxes )
		//	{
		//		if ( Donya::OBB::IsHitOBB( playerShieldBox, it ) )
		//		{
		//			wasHitToShield = true;
		//			player.SucceededDefence();
		//		}
		//		if ( !wasHitToShield && Donya::OBB::IsHitOBB( playerBodyBox, it ) )
		//		{
		//			player.ReceiveImpact();
		//		}

		//		wasHitToShield = false;
		//	}

		//	const auto attackSpheres = boss.RequireAttackHitBoxesSphere();
		//	for ( const auto &it : attackSpheres )
		//	{
		//		if ( Donya::OBB::IsHitSphere( playerShieldBox, it ) )
		//		{
		//			wasHitToShield = true;
		//			player.SucceededDefence();
		//		}
		//		if ( !wasHitToShield && Donya::OBB::IsHitSphere( playerBodyBox, it ) )
		//		{
		//			player.ReceiveImpact();
		//		}
		//	}

		//#else

		//	bool shieldCollided = boss.IsCollideAttackHitBoxes( playerShieldBox, /* disableCollidingHitBoxes = */ true );
		//	if ( shieldCollided )
		//	{
		//		wasHitToShield = true;
		//		player.SucceededDefence();
		//	}

		//	bool bodyCollided = ( shieldCollided ) ? false : boss.IsCollideAttackHitBoxes( playerBodyBox, /* disableCollidingHitBoxes = */ false );
		//	if ( bodyCollided )
		//	{
		//		player.ReceiveImpact();
		//	}

		//#endif // FETCH_BOXES_THEN_JUDGE
		//}
		//
		//// PlayerAttack VS BossBodies
		//{
		//	const auto pBossBodies = boss.GetBodyHitBoxes();
		//	for ( const auto &it : pBossBodies )
		//	{
		//		if ( Donya::OBB::IsHitOBB( playerAttackBox, it ) )
		//		{
		//			boss.ReceiveImpact();
		//			break;
		//		}
		//	}
		//}
		
	}
public:
	void LoadParameter( bool isBinary = true )
	{
		Donya::Serializer::Extension ext = ( isBinary )
		? Donya::Serializer::Extension::BINARY
		: Donya::Serializer::Extension::JSON;
		std::string filePath = GenerateSerializePath( SERIAL_ID, ext );

		Donya::Serializer seria;
		seria.Load( ext, filePath.c_str(), SERIAL_ID, *this );
	}

	void SaveParameter()
	{
		Donya::Serializer::Extension bin  = Donya::Serializer::Extension::BINARY;
		Donya::Serializer::Extension json = Donya::Serializer::Extension::JSON;
		std::string binPath  = GenerateSerializePath( SERIAL_ID, bin  );
		std::string jsonPath = GenerateSerializePath( SERIAL_ID, json );

		Donya::Serializer seria;
		seria.Save( bin,  binPath.c_str(),  SERIAL_ID, *this );
		seria.Save( json, jsonPath.c_str(), SERIAL_ID, *this );
	}

	void UseImGui()
	{
#if USE_IMGUI

		if ( ImGui::BeginIfAllowed() )
		{
			if ( ImGui::Button( "GotoTitle" ) )
			{
				pSceneManager->setNextScene( new sceneTitle(), false );
			}

			if ( ImGui::TreeNode( "Game" ) )
			{
				ImGui::Text( "Show.Press \'ALT\' and \'I\' or \'T'\'key : Toggle ImGui ON/OFF" );
				ImGui::Text( "Show.Press \'ALT\' and \'H\'key : Toggle Collision ON/OFF" );
				ImGui::Text( "" );

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

						getLineLight().setLineLight( lights.direction.direction, lights.direction.color );

						ImGui::TreePop();
					}
					
					if ( ImGui::TreeNode( "PointLight" ) )
					{
						constexpr size_t MAX_COUNT = 4U;
						bool canAppend  = ( lights.points.size() < MAX_COUNT ) ? true : false;
						bool canPopBack = ( 1U <= lights.points.size() ) ? true : false;
						if ( canAppend && ImGui::Button( "Append" ) )
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
							auto &point = lights.points[i];
							ImGui::DragFloat3( std::string( "Position"  + std::to_string( i ) ).c_str(), &point.pos.x		); point.pos.w = 1.0f;
							ImGui::ColorEdit4( std::string( "Color"     + std::to_string( i ) ).c_str(), &point.color.x		);
							ImGui::SliderFloat4( std::string( "Attenuate" + std::to_string( i ) ).c_str(), &point.attenuate.x, 0.0f, 1.0f );

							setPointlightInfo( scast<int>( i ), point );
						}
						
						ImGui::TreePop();
					}

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
