#include "scene.h"
#include "sceneManager.h"

#include "Donya/Constant.h"	// Use DEBUG_MODE macro.
#include "Donya/Keyboard.h"
#include "Donya/Sound.h"
#include "Donya/Useful.h"	// Use show collision state.
#include "Donya/UseImGui.h"	// Use helper functions of ImGui.
#include "Donya/Vector.h"

#include "gameLib.h"

#include "Player.h"
#include "Stage.h"

#if DEBUG_MODE
constexpr int BGM_ID = 'BGM';
constexpr int SE_ID  = 'SE';
#endif // DEBUG_MODE

struct SceneGame::Impl
{
public:
	Donya::Vector3 cameraPos;
	Player player;
	Stage  stage;
public:
	Impl() :
		cameraPos(),
		player(),
		stage()
	{}
	~Impl() = default;
public:
	void Init()
	{
	#if DEBUG_MODE
		Donya::Sound::Load( BGM_ID, "./Data/Sounds/Test/BGM.wav", true  );
		Donya::Sound::Load( SE_ID,  "./Data/Sounds/Test/SE.wav",  false );
	#endif // DEBUG_MODE

		cameraPos = Donya::Vector3{ 0.0f, 256.0f, -512.0f };

		stage.Init( NULL );
		player.Init();
	}
	void Uninit()
	{
		stage.Uninit();
		player.Uninit();
	}

	void Update()
	{
	#if DEBUG_MODE

		if ( Donya::Keyboard::Press( 'D' ) && Donya::Keyboard::Trigger( 'C' ) )
		{
			char breakPoint = 0;
		}
		if ( Donya::Keyboard::Trigger( 'H' ) )
		{
			Donya::ToggleShowStateOfImGui();
		}
		if ( Donya::Keyboard::Trigger( 'T' ) )
		{
			Donya::ToggleShowCollision();
		}

	#endif // DEBUG_MODE

		GameLib::camera::setPos( cameraPos );

		stage.Update();

		auto MakePlayerInput = []()->Player::Input
		{
			Player::Input input{};

			if ( Donya::Keyboard::Press( VK_UP    ) ) { input.moveVector.z = +1.0f; }
			if ( Donya::Keyboard::Press( VK_DOWN  ) ) { input.moveVector.z = -1.0f; }
			if ( Donya::Keyboard::Press( VK_LEFT  ) ) { input.moveVector.x = -1.0f; }
			if ( Donya::Keyboard::Press( VK_RIGHT ) ) { input.moveVector.x = +1.0f; }

			if ( Donya::Keyboard::Press( 'Z' ) ) { input.doDefend = true; }
			if ( Donya::Keyboard::Press( 'X' ) ) { input.doAttack = true; }

			return input;
		};
		player.Update( MakePlayerInput() );
	}

	void Draw()
	{
		clearWindow( 0.5f, 0.5f, 0.5f, 1.0f );

		Donya::Vector4x4 V = Donya::Vector4x4::FromMatrix( GameLib::camera::GetViewMatrix() );
		Donya::Vector4x4 P = Donya::Vector4x4::FromMatrix( GameLib::camera::GetProjectionMatrix() );

		stage.Draw( V, P );

		player.Draw( V, P );
	}

public:
	void UseImGui()
	{
	#if USE_IMGUI

		if ( ImGui::BeginIfAllowed() )
		{
			if ( ImGui::TreeNode( u8"Sound test" ) )
			{
				if ( ImGui::Button( u8"BGM.Play"   ) ) { Donya::Sound::Play  ( BGM_ID ); }
				if ( ImGui::Button( u8"BGM.Pause"  ) ) { Donya::Sound::Pause ( BGM_ID ); }
				if ( ImGui::Button( u8"BGM.Resume" ) ) { Donya::Sound::Resume( BGM_ID ); }
				if ( ImGui::Button( u8"BGM.Stop"   ) ) { Donya::Sound::Stop  ( BGM_ID ); }
				ImGui::Text( "" );
				if ( ImGui::Button( u8"SE.Play"    ) ) { Donya::Sound::Play  ( SE_ID  ); }
				if ( ImGui::Button( u8"SE.Pause"   ) ) { Donya::Sound::Pause ( SE_ID  ); }
				if ( ImGui::Button( u8"SE.Resume"  ) ) { Donya::Sound::Resume( SE_ID  ); }
				if ( ImGui::Button( u8"SE.Stop"    ) ) { Donya::Sound::Stop  ( SE_ID  ); }

				ImGui::TreePop();
			}

			if ( ImGui::TreeNode( "Configuration" ) )
			{
				ImGui::DragFloat3( "Camera.Pos", &cameraPos.x );

				ImGui::TreePop();
			}

			if ( ImGui::Button( "GotoTitle" ) )
			{
				pSceneManager->setNextScene( new sceneTitle(), false );
			}

			ImGui::End();
		}

	#endif // USE_IMGUI
	}
};

SceneGame::SceneGame() : baseScene(), pImpl( std::make_unique<Impl>() )
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
