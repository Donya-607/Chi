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

#include "Boss.h"
#include "Player.h"
#include "Stage.h"

#define scast static_cast

#if DEBUG_MODE
constexpr int BGM_ID = 'BGM';
constexpr int SE_ID = 'SE';
#endif // DEBUG_MODE

struct SceneGame::Impl
{
public:
	float	fieldRadius;
	Donya::Vector3 cameraPos;
	Donya::Vector3 cameraFocusOffset;
	Player	player;
	Stage	stage;
	Boss	boss;
	Lights	lights;
	ID3D11ShaderResourceView* SRV[2];
	ID3D11RenderTargetView* RT[2];
	static_mesh testCube{};
	skinned_mesh animTest{};
	bool bloom_flg = false;
	ImTextureID tex_SRV[2] = { nullptr };
	Donya::Vector4 color{ 1,1,1,1 };
	float blur = 0;
	fbx_shader shader;

public:
	Impl() :
		fieldRadius(),
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
	void serialize(Archive& archive, std::uint32_t version)
	{
		archive
		(
			CEREAL_NVP(fieldRadius)
		);

		if (1 <= version)
		{
			archive(CEREAL_NVP(lights));
		}
		if (2 <= version)
		{
			archive(CEREAL_NVP(cameraFocusOffset));
		}
		if (3 <= version)
		{
			// archive( CEREAL_NVP( x ) );
		}
	}
	static constexpr const char* SERIAL_ID = "Game";
public:
	void Init()
	{
#if DEBUG_MODE
		Donya::Sound::Load(BGM_ID, "./Data/Sounds/Test/BGM.wav", true);
		Donya::Sound::Load(SE_ID, "./Data/Sounds/Test/SE.wav", false);
#endif // DEBUG_MODE

		resetPointLight();

		LoadParameter();

		getLineLight().setLineLight(lights.direction.direction, lights.direction.color);
		//for (const auto i : lights.points)
		//{
		//	setPointLight(i);
		//}

		cameraPos = Donya::Vector3{ 0.0f, 256.0f, -512.0f };
		loadShader(shader, "./Data/shader/skinned_mesh_has_born_vs.cso", "./Data/shader/skinned_mesh_ps.cso", "./Data/shader/skinned_mesh_vs.cso", "./Data/shader/skinned_mesh_no_uv_ps.cso");

#if DEBUG_MODE
		createCube(&testCube);
		tex_SRV[0] = nullptr;
		tex_SRV[1] = nullptr;
		bloom_flg = false;
		constexpr int STAGE_NO = NULL;
#endif // DEBUG_MODE
		createSRV(&SRV[0], &RT[0]);
		createSRV(&SRV[1], &RT[1]);

		stage.Init(STAGE_NO);

		loadFBX(&animTest, "./Data/MDL_Player_Wait.fbx");

		player.Init();
		player.SetFieldRadius(fieldRadius);

		boss.Init(STAGE_NO);
	}
	void Uninit()
	{
		ResourceManager::ReleaseShaderResourceView(SRV[0]);
		ResourceManager::ReleaseShaderResourceView(SRV[1]);
		stage.Uninit();
		player.Uninit();
		boss.Uninit();
	}

	void Update()
	{
#if DEBUG_MODE

		if (Donya::Keyboard::Press(VK_MENU))
		{
			if (Donya::Keyboard::Trigger('C'))
			{
				char breakPoint = 0;
			}
			if (Donya::Keyboard::Trigger('H'))
			{
				Donya::ToggleShowCollision();
			}
			if (Donya::Keyboard::Trigger('T') || Donya::Keyboard::Trigger('I'))
			{
				Donya::ToggleShowStateOfImGui();
			}
		}

#endif // DEBUG_MODE

		stage.Update();

		auto MakePlayerInput = []()->Player::Input
		{
			Player::Input input{};

#if DEBUG_MODE
			if (Donya::Keyboard::Press(VK_UP)) { input.moveVector.z = +1.0f; }
			if (Donya::Keyboard::Press(VK_DOWN)) { input.moveVector.z = -1.0f; }
			if (Donya::Keyboard::Press(VK_LEFT)) { input.moveVector.x = -1.0f; }
			if (Donya::Keyboard::Press(VK_RIGHT)) { input.moveVector.x = +1.0f; }

			if (Donya::Keyboard::Press('Z')) { input.doDefend = true; }
			if (Donya::Keyboard::Trigger('X')) { input.doAttack = true; }
#endif // DEBUG_MODE

			return input;
		};
		player.Update(MakePlayerInput());

		boss.Update();

		GameLib::camera::setPos(cameraPos);
		GameLib::camera::setTarget(player.GetPosition() + cameraFocusOffset);

		ProcessCollision();

#if DEBUG_MODE

		if (getKeyState(KEY_INPUT_1) == 1)
		{
			setAnimFlame(&animTest, 0);
		}
		if (getKeyState(KEY_INPUT_2) == 1)
		{
			setAnimFlame(&animTest, 20);
		}
		if (getKeyState(KEY_INPUT_B) == 1)
		{
			bloom_flg ^= 1;
		}
		if (getKeyState(KEY_INPUT_SPACE))
		{
			//setStopTime(&animTest,1);
			setStopAnimation(&animTest, true);
		}
		else
		{
			setStopAnimation(&animTest, false);
		}

#endif // DEBUG_MODE
	}

	void Draw()
	{
		clearWindow(0.1f, 0.1f, 0.1f, 1.0f);
		setBlendMode_ALPHA(1.0f);

		Donya::Vector4x4 V = Donya::Vector4x4::FromMatrix(getViewMatrix());
		Donya::Vector4x4 P = Donya::Vector4x4::FromMatrix(getProjectionMatrix());

		stage.Draw(shader,V, P);

		player.Draw(shader,V, P);

		boss.Draw(shader,V, P);
		GameLib::clearRT(RT[0], { 0,0,0,0 });
		GameLib::setRenderTarget(&RT[0]);

		stage.Draw(shader,V, P);

		player.Draw(shader,V, P);

		boss.Draw(shader,V, P);

		GameLib::clearRT(RT[1], { 0,0,0,0 });
		GameLib::setRenderTarget(&RT[1]);

		postEffect_Bloom_SRV(&SRV[0], color);

		GameLib::resetRendertarget();

		if (bloom_flg)
		{
			postEffect_Bloom(&SRV[1], blur, color);
			tex_SRV[0] = (void*)SRV[0];
			tex_SRV[1] = (void*)SRV[1];
		}

#if DEBUG_MODE

		if (0)
		{
			XMFLOAT4X4 World, world_view_projection;
			DirectX::XMMATRIX worldM;
			DirectX::XMMATRIX worldcubeM;
			DirectX::XMMATRIX S, R, Rx, Ry, Rz, T;
			auto plPos = player.GetPosition();
			T = DirectX::XMMatrixTranslation(plPos.x, plPos.y, plPos.z);
			worldM = T;

			//	Matrix -> Float4x4 •ÏŠ·
			DirectX::XMStoreFloat4x4(&world_view_projection, worldM * getViewMatrix() * getProjectionMatrix());
			DirectX::XMStoreFloat4x4(&World, worldM);

			FBXRender(&animTest,shader, world_view_projection, World);

			DirectX::XMFLOAT3 pos = { -10,10,100 };
			worldcubeM = DirectX::XMMatrixIdentity();

			//	Šg‘åEk¬
			S = DirectX::XMMatrixScaling(10, 10, 10);

			//	‰ñ“]
			Rx = DirectX::XMMatrixRotationX(0);				//	XŽ²‚ðŠî€‚Æ‚µ‚½‰ñ“]s—ñ
			Ry = DirectX::XMMatrixRotationY(0);				//	YŽ²‚ðŠî€‚Æ‚µ‚½‰ñ“]s—ñ
			Rz = DirectX::XMMatrixRotationZ(0);				//	ZŽ²‚ðŠî€‚Æ‚µ‚½‰ñ“]s—ñ
			R = Rz * Rx * Ry;

			//	•½sˆÚ“®
			calcTransformedPosBySpecifyMesh(&animTest, pos, "_04_MDL_yoroi1:_02_MDL_yoroi:spear");
			T = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);

			//	ƒ[ƒ‹ƒh•ÏŠ·s—ñ
			worldcubeM = S * R * T * worldM;

			//	Matrix -> Float4x4 •ÏŠ·
			DirectX::XMStoreFloat4x4(&world_view_projection, worldcubeM * getViewMatrix() * getProjectionMatrix());
			DirectX::XMStoreFloat4x4(&World, worldcubeM);

			OBJRender(&testCube, world_view_projection, World);

		}


#endif // DEBUG_MODE
	}

public:
	void ProcessCollision()
	{
		Donya::OBB playerBodyBox = player.GetHurtBox();
		Donya::OBB playerShieldBox = player.GetShieldHitBox();

		// Player VS BossAttacks
		{
			bool wasHitToShield = false;
			const auto attackBoxes = boss.GetAttackHitBoxes();
			for (const auto& it : attackBoxes)
			{
				if (Donya::OBB::IsHitOBB(playerShieldBox, it))
				{
					wasHitToShield = true;
					player.SucceededDefence();
				}
				if (!wasHitToShield && Donya::OBB::IsHitOBB(playerShieldBox, it))
				{
					// TODO:Do
				}

				wasHitToShield = false;
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


		if (ImGui::BeginIfAllowed())
		{
			if (ImGui::Button("GotoTitle"))
			{
				pSceneManager->setNextScene(new sceneTitle(), false);
			}

			if (ImGui::TreeNode("Game"))
			{
				ImGui::Text("Show.Press \'ALT\' and \'I\' or \'T'\'key : Toggle ImGui ON/OFF");
				ImGui::Text("Show.Press \'ALT\' and \'H\'key : Toggle Collision ON/OFF");
				ImGui::Text("");

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
					ImGui::DragFloat("Field.Radius", &fieldRadius); player.SetFieldRadius(fieldRadius);
					ImGui::Text("");
					ImGui::DragFloat3("Camera.Pos", &cameraPos.x);
					ImGui::DragFloat3("Camera.FocusOffset", &cameraFocusOffset.x);

					ImGui::TreePop();
				}

				if (ImGui::TreeNode("Light"))
				{
					if (ImGui::TreeNode("DirectionalLight"))
					{
						ImGui::ColorEdit4("Color", &lights.direction.color.x);
						ImGui::SliderFloat3("Direction", &lights.direction.direction.x, -8.0f, 8.0f);

						getLineLight().setLineLight(lights.direction.direction, lights.direction.color);

						ImGui::TreePop();
					}

					if (ImGui::TreeNode("PointLight"))
					{
						constexpr size_t MAX_COUNT = 4U;
						bool canAppend = (lights.points.size() < MAX_COUNT) ? true : false;
						bool canPopBack = (1U <= lights.points.size()) ? true : false;
						if (canAppend && ImGui::Button("Append"))
						{
							PointLight add{};
							add.pos = { 0.0f, 0.0f, 0.0f, 1.0f };
							add.color = { 1.0f, 1.0f, 1.0f, 1.0f };
							lights.points.push_back(add);
							setPointLight(add);
						}
						if (canPopBack && ImGui::Button("PopBack"))
						{
							lights.points.pop_back();
							removePointLight(scast<int>(lights.points.size()));
						}

						const size_t COUNT = lights.points.size();
						for (size_t i = 0; i < COUNT; ++i)
						{
							auto& point = lights.points[i];
							ImGui::DragFloat3(std::string("Position" + std::to_string(i)).c_str(), &point.pos.x); point.pos.w = 1.0f;
							ImGui::ColorEdit4(std::string("Color" + std::to_string(i)).c_str(), &point.color.x);
							ImGui::SliderFloat4(std::string("Attenuate" + std::to_string(i)).c_str(), &point.attenuate.x, 0.0f, 1.0f);

							setPointlightInfo(scast<int>(i), point);
						}

						ImGui::TreePop();
					}

					ImGui::TreePop();
				}

				if (ImGui::TreeNode("File.I/O"))
				{
					static bool isBinary = true;
					if (ImGui::RadioButton("Binary", isBinary)) { isBinary = true; }
					if (ImGui::RadioButton("JSON", !isBinary)) { isBinary = false; }
					std::string loadStr{ "Load " };
					loadStr += (isBinary) ? "Binary" : "JSON";

					if (ImGui::Button("Save"))
					{
						SaveParameter();
					}
					// if ( ImGui::Button( Donya::MultiToUTF8( loadStr ).c_str() ) )
					if (ImGui::Button(loadStr.c_str()))
					{
						LoadParameter(isBinary);
					}

					ImGui::TreePop();
				}

				ImGui::TreePop();
			}
			if (bloom_flg && tex_SRV)
			{
				ImGui::Image(tex_SRV[0], { 192 * 2,108 * 2 });
				ImGui::Image(tex_SRV[1], { 192 * 2,108 * 2 });
			}
			ImGui::Text("Color");
			ImGui::DragFloat("r##color", &color.x, 0.01, 0, 1);
			ImGui::DragFloat("g##color", &color.y, 0.01, 0, 1);
			ImGui::DragFloat("b##color", &color.z, 0.01, 0, 1);

			ImGui::NewLine();
			ImGui::DragFloat("blur", &blur, 0.01, 0);
			ImGui::End();
		}

#endif // USE_IMGUI
	}
};
CEREAL_CLASS_VERSION(SceneGame::Impl, 2)

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
