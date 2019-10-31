#include "scene.h"
#include "sceneManager.h"

#include "Donya/Constant.h"	// Use DEBUG_MODE macro.
#include "Donya/Keyboard.h"
#include "Donya/Sound.h"
#include "Donya/UseImGui.h"	// Use helper functions of ImGui.
#include "Donya/Vector.h"

#include "gameLib.h"

#include "Player.h"

#if DEBUG_MODE
constexpr int BGM_ID = 'BGM';
constexpr int SE_ID = 'SE';
#endif // DEBUG_MODE

struct SceneGame::Impl
{
public:
	Donya::Vector3 cameraPos;
	Player player;
	static_mesh testCube;
	skinned_mesh animTest;
public:
	Impl() :
		cameraPos(),
		player()
	{}
	~Impl() = default;
public:
	void Init()
	{
#if DEBUG_MODE
		Donya::Sound::Load(BGM_ID, "./Data/Sounds/Test/BGM.wav", true);
		Donya::Sound::Load(SE_ID, "./Data/Sounds/Test/SE.wav", false);
#endif // DEBUG_MODE

		cameraPos = Donya::Vector3{ 0.0f, 256.0f, -512.0f };
		createCube(&testCube);
		loadFBX(&animTest, "./Data/MDL_Player_Attack.fbx");
		player.Init();
	}
	void Uninit()
	{
		player.Uninit();
	}

	void Update()
	{
		GameLib::camera::setPos(cameraPos);

		auto MakePlayerInput = []()->Player::Input
		{
			Player::Input input{};

			if (Donya::Keyboard::Press(VK_UP)) { input.moveVector.z = +1.0f; }
			if (Donya::Keyboard::Press(VK_DOWN)) { input.moveVector.z = -1.0f; }
			if (Donya::Keyboard::Press(VK_LEFT)) { input.moveVector.x = -1.0f; }
			if (Donya::Keyboard::Press(VK_RIGHT)) { input.moveVector.x = +1.0f; }

			if (Donya::Keyboard::Press('Z')) { input.doDefend = true; }
			if (Donya::Keyboard::Press('X')) { input.doAttack = true; }

			return input;
		};
		player.Update(MakePlayerInput());

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
			setStopAnimation(&animTest, false);
	}

	void Draw()
	{
		clearWindow(0.1f, 0.1f, 0.1f, 1.0f);

		Donya::Vector4x4 V = Donya::Vector4x4::FromMatrix(GameLib::camera::GetViewMatrix());
		Donya::Vector4x4 P = Donya::Vector4x4::FromMatrix(GameLib::camera::GetProjectionMatrix());

		//player.Draw( V, P );

		XMFLOAT4X4 World, world_view_projection;
		DirectX::XMMATRIX worldM;
		DirectX::XMMATRIX worldcubeM;
		DirectX::XMMATRIX S, R, Rx, Ry, Rz, T;
		worldM = DirectX::XMMatrixIdentity();

		//	Šg‘åEk¬
		S = DirectX::XMMatrixScaling(0.5f, 0.5f, 0.5f);

		//	‰ñ“]
		Rx = DirectX::XMMatrixRotationX(0);				//	XŽ²‚ðŠî€‚Æ‚µ‚½‰ñ“]s—ñ
		Ry = DirectX::XMMatrixRotationY(0);				//	YŽ²‚ðŠî€‚Æ‚µ‚½‰ñ“]s—ñ
		Rz = DirectX::XMMatrixRotationZ(0);				//	ZŽ²‚ðŠî€‚Æ‚µ‚½‰ñ“]s—ñ
		R = Rz * Rx * Ry;

		//	•½sˆÚ“®
		T = DirectX::XMMatrixTranslation(0, 0, 0);

		//	ƒ[ƒ‹ƒh•ÏŠ·s—ñ
		worldM = S * R * T;

		//	Matrix -> Float4x4 •ÏŠ·
		DirectX::XMStoreFloat4x4(&world_view_projection, worldM * getViewMatrix() * getProjectionMatrix());
		DirectX::XMStoreFloat4x4(&World, worldM);
		setBlendMode_ALPHA(255);
		FBXRender(&animTest, world_view_projection, World);
		DirectX::XMFLOAT3 pos = { -10,10,100 };
		worldcubeM = DirectX::XMMatrixIdentity();

		//	Šg‘åEk¬
		S = DirectX::XMMatrixScaling(10,10,10);

		//	‰ñ“]
		Rx = DirectX::XMMatrixRotationX(0);				//	XŽ²‚ðŠî€‚Æ‚µ‚½‰ñ“]s—ñ
		Ry = DirectX::XMMatrixRotationY(0);				//	YŽ²‚ðŠî€‚Æ‚µ‚½‰ñ“]s—ñ
		Rz = DirectX::XMMatrixRotationZ(0);				//	ZŽ²‚ðŠî€‚Æ‚µ‚½‰ñ“]s—ñ
		R = Rz * Rx * Ry;

		//	•½sˆÚ“®
		calcTransformedPosBySpecifyMesh(&animTest, pos, "_04_MDL_yoroi1:_02_MDL_yoroi:spear");
		T = DirectX::XMMatrixTranslation(pos.x,pos.y,pos.z);

		//	ƒ[ƒ‹ƒh•ÏŠ·s—ñ
		worldcubeM = S * R * T * worldM;

		//	Matrix -> Float4x4 •ÏŠ·
		DirectX::XMStoreFloat4x4(&world_view_projection, worldcubeM * getViewMatrix() * getProjectionMatrix());
		DirectX::XMStoreFloat4x4(&World, worldcubeM);

		OBJRender(&testCube, world_view_projection, World);
	}

public:
	void UseImGui()
	{
#if USE_IMGUI

		if (ImGui::BeginIfAllowed())
		{
			if (ImGui::TreeNode(u8"Sound test"))
			{
				if (ImGui::Button(u8"BGM.Play")) { Donya::Sound::Play(BGM_ID); }
				if (ImGui::Button(u8"BGM.Pause")) { Donya::Sound::Pause(BGM_ID); }
				if (ImGui::Button(u8"BGM.Resume")) { Donya::Sound::Resume(BGM_ID); }
				if (ImGui::Button(u8"BGM.Stop")) { Donya::Sound::Stop(BGM_ID); }
				ImGui::Text("");
				if (ImGui::Button(u8"SE.Play")) { Donya::Sound::Play(SE_ID); }
				if (ImGui::Button(u8"SE.Pause")) { Donya::Sound::Pause(SE_ID); }
				if (ImGui::Button(u8"SE.Resume")) { Donya::Sound::Resume(SE_ID); }
				if (ImGui::Button(u8"SE.Stop")) { Donya::Sound::Stop(SE_ID); }

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Configuration"))
			{
				ImGui::DragFloat3("Camera.Pos", &cameraPos.x);

				ImGui::TreePop();
			}

			if (ImGui::Button("GotoTitle"))
			{
				pSceneManager->setNextScene(new sceneTitle(), false);
			}

			ImGui::End();
		}

#endif // USE_IMGUI
	}
};

SceneGame::SceneGame() : baseScene(), pImpl(std::make_unique<Impl>())
{}
SceneGame::~SceneGame()
{
	pImpl.reset(nullptr);
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
