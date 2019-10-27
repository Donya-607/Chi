#include "scene.h"
#include "sceneManager.h"

#include "Donya/Constant.h"	// Use DEBUG_MODE macro.
#include "Donya/Sound.h"
#include "Donya/UseImGui.h"	// Use helper functions of ImGui.

#if DEBUG_MODE
constexpr int BGM_ID = 'BGM';
constexpr int SE_ID  = 'SE';
#endif // DEBUG_MODE

struct SceneGame::Impl
{
public:

public:
	Impl()
	{

	}
	~Impl() = default;
public:
	void Init()
	{
	#if DEBUG_MODE
		Donya::Sound::Load( BGM_ID, "./Data/Sounds/Test/BGM.wav", true  );
		Donya::Sound::Load( SE_ID,  "./Data/Sounds/Test/SE.wav",  false );
	#endif // DEBUG_MODE
	}
	void Uninit()
	{

	}

	void Update()
	{

	}

	void Draw() const
	{
		clearWindow( 0.5f, 0.5f, 0.5f, 1.0f );

	}

public:
	void UseImGui()
	{
	#if USE_IMGUI

		if ( ImGui::BeginIfAllowed() )
		{
			if ( ImGui::TreeNode( u8"ÉTÉEÉìÉhÉeÉXÉg" ) )
			{
				if ( ImGui::Button( u8"ÇaÇfÇlÅEÇ»ÇÁÇ∑"   ) ) { Donya::Sound::Play  ( BGM_ID ); }
				if ( ImGui::Button( u8"ÇaÇfÇlÅEÇ∆ÇﬂÇÈ"   ) ) { Donya::Sound::Pause ( BGM_ID ); }
				if ( ImGui::Button( u8"ÇaÇfÇlÅEÇ≥Ç¢Ç©Ç¢" ) ) { Donya::Sound::Resume( BGM_ID ); }
				if ( ImGui::Button( u8"ÇaÇfÇlÅEÇ∑Ç∆Ç¡Ç’" ) ) { Donya::Sound::Stop  ( BGM_ID ); }
				ImGui::Text( "" );
				if ( ImGui::Button( u8"ÇrÇdÅEÇ»ÇÁÇ∑"     ) ) { Donya::Sound::Play  ( SE_ID  ); }
				if ( ImGui::Button( u8"ÇrÇdÅEÇ∆ÇﬂÇÈ"     ) ) { Donya::Sound::Pause ( SE_ID  ); }
				if ( ImGui::Button( u8"ÇrÇdÅEÇ≥Ç¢Ç©Ç¢"   ) ) { Donya::Sound::Resume( SE_ID  ); }
				if ( ImGui::Button( u8"ÇrÇdÅEÇ∑Ç∆Ç¡Ç’"   ) ) { Donya::Sound::Stop  ( SE_ID  ); }

				ImGui::TreePop();
			}

			ImGui::End();
		}

	#endif // USE_IMGUI
	}
};

SceneGame::SceneGame() : pImpl( std::make_unique<Impl>() )
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
