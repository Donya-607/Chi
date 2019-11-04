#include "Particle.h"
#include "imgui.h"
#include "Donya/FilePath.h"
#include "Donya/Useful.h"


void Particle::Init( DirectX::XMFLOAT3 _originPos, DirectX::XMFLOAT3 _speed, DirectX::XMFLOAT3 _accel, float _scale )
{
	originPos = _originPos;
	pos = _originPos;
	speed = _speed;
	accel = _accel;
	color = DirectX::XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
	exist = true;
	scale = _scale;
}

void Particle::Update()
{
	speed.x += accel.x;
	speed.y += accel.y;
	speed.z += accel.z;
	pos.x += speed.x;
	pos.y += speed.y;
	pos.z += speed.z;
}

#if 0
void Particle::Emit()
{
	if (loop)
	{
		for (int i = 0; i < maxSize; i++)
		{
			if (!exists[i])
			{
				exists[i] = true;
				originSpeed[i] = speed[i];
				break;
			}
		}
		for (int i = 0; i < maxSize; i++)
		{
			if (!exists[i]) continue;
			if (maxCnt < cnts[i])
			{
				cnts[i] = 0;
				exists[i] = false;
				pos[i] = originPos;
				float randSpeedX = static_cast<float>(rand() % flashImGuiData.speed[0]) / 100.0f - (flashImGuiData.speed[0] / 2.0f / 100.0f);
				float randSpeedY = static_cast<float>(rand() % flashImGuiData.speed[1]) / 100.0f;
				float randSpeedZ = static_cast<float>(rand() % flashImGuiData.speed[2]) / 100.0f - (flashImGuiData.speed[2] / 2.0f / 100.0f);
				float randAccelX = static_cast<float>(rand() % flashImGuiData.accel[0]) / 1000.0f - (flashImGuiData.accel[0] / 2.0f / 1000.0f);
				float randAccelY = static_cast<float>(rand() % flashImGuiData.accel[1]) / 1000.0f - (flashImGuiData.accel[1] / 2.0f / 1000.0f);
				float randAccelZ = static_cast<float>(rand() % flashImGuiData.accel[2]) / 1000.0f - (flashImGuiData.accel[2] / 2.0f / 1000.0f); 
				speed[i] = DirectX::XMFLOAT3(randSpeedX, randSpeedY, randSpeedZ);
				accel[i] = DirectX::XMFLOAT3(randAccelX, -randAccelY, randAccelZ);
				color[i].w = 1.0f;
				continue;
			}

			speed[i].x += accel[i].x;
			speed[i].y += accel[i].y;
			speed[i].z += accel[i].z;
			pos[i].x += speed[i].x;
			pos[i].y += speed[i].y;
			pos[i].z += speed[i].z;

			color[i].w = 1.0f - (static_cast<float>(cnts[i]) / static_cast<float>(maxCnt));

			cnts[i]++;
		}
	}
	else if (emitting)
	{
		for (int i = 0; i < maxSize; i++)
		{
			if (!exists[i])
			{
				continue;
			}
			if (maxCnt < cnt)
			{
				exists[i] = false;
				continue;
			}

			speed[i].x += accel[i].x;
			speed[i].y += accel[i].y;
			speed[i].z += accel[i].z;
			pos[i].x += speed[i].x;
			pos[i].y += speed[i].y;
			pos[i].z += speed[i].z;

			color[i].w = 1.0f - (static_cast<float>(cnt) / static_cast<float>(maxCnt));
		}

		if (maxCnt < cnt)
		{
			emitting = false;
			cnt = -1;
		}

		cnt++;
	}
}
#endif


void FlashParticle::Set()
{
	for (int i = 0; i < MAX_SIZE; i++)
	{
		data[i].Init(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), 1.0f);
	}
}

void FlashParticle::ImGuiDataInit()
{
	LoadParameter();

	imguiData.speed[0] = 0; // 0.0f;
	imguiData.speed[1] = 0; // 0.0f;
	imguiData.speed[2] = 0; // 0.0f;
	//imguiData.accel[0] = 0.0f;
	//imguiData.accel[1] = 0.0f;
	//imguiData.accel[2] = 0.0f;
	imguiData.accelStart[0] = 0; // 0.0f;
	imguiData.accelStart[1] = 0; // 0.0f;
	imguiData.accelStart[2] = 0; // 0.0f;
	imguiData.accelStage[0] = 0; // 0.0f;
	imguiData.accelStage[1] = 0; // 0.0f;
	imguiData.accelStage[2] = 0; // 0.0f;
}

void FlashParticle::Emit()
{
	for (int i = 0; i < MAX_SIZE; i++)
	{
		if (!data[i].GetExist())
		{
			continue;
		}
		if (MAX_CNT < cnt)
		{
			data[i].SetExist(false);
			continue;
		}
		data[i].Update();
		data[i].SetAlpha(1.0f - (static_cast<float>(cnt) / static_cast<float>(MAX_CNT)));
	}

	if (MAX_CNT < cnt)
	{
		cnt = -1;
	}
	cnt++;
}

void FlashParticle::ImGui()
{
	if (ImGui::TreeNode(u8"Flash"))
	{
#if 0
		ImGui::InputInt3("speed", imguiData.speed);
		//ImGui::InputInt3("accel", imguiData.accel);
		ImGui::InputInt3("accelStart", imguiData.accelStart);
		ImGui::InputInt3("accelStage", imguiData.accelStage);
#else
		ImGui::DragInt3("speed", imguiData.speed);
		ImGui::DragInt3("accelStart", imguiData.accelStart);
		ImGui::DragInt3("accelStage", imguiData.accelStage);
#endif
		ImGui::TreePop();
	}
}

void FlashParticle::LoadParameter(bool isBinary)
{
	Donya::Serializer::Extension ext = (isBinary)
		? Donya::Serializer::Extension::BINARY
		: Donya::Serializer::Extension::JSON;
	std::string filePath = GenerateSerializePath(SERIAL_ID, ext);

	Donya::Serializer seria;
	seria.Load(ext, filePath.c_str(), SERIAL_ID, *this);
}


#if USE_IMGUI

void FlashParticle::SaveParameter()
{
	Donya::Serializer::Extension bin = Donya::Serializer::Extension::BINARY;
	Donya::Serializer::Extension json = Donya::Serializer::Extension::JSON;
	std::string binPath = GenerateSerializePath(SERIAL_ID, bin);
	std::string jsonPath = GenerateSerializePath(SERIAL_ID, json);

	Donya::Serializer seria;
	seria.Save(bin, binPath.c_str(), SERIAL_ID, *this);
	seria.Save(json, jsonPath.c_str(), SERIAL_ID, *this);
}

void FlashParticle::UseImGui()
{
	if (ImGui::BeginIfAllowed())
	{
		if (ImGui::TreeNode("FlashParticle.AdjustData"))
		{
			/*ImGui::SliderFloat("Scale", &scale, 0.0f, 8.0f);
			ImGui::DragFloat("Running Speed", &runSpeed);
			ImGui::SliderFloat("SlerpPercent of Rotation", &rotSlerpFactor, 0.05f, 1.0f);*/

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

		ImGui::End();
	}
}

#endif // USE_IMGUI


void BubbleParticle::Set()
{
	for (int i = 0; i < MAX_SIZE; i++)
	{
		data[i].Init(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), 1.0f);
	}
}

void BubbleParticle::ImGuiDataInit()
{
	imguiData.speed[0] = 0; // 0.0f;
	imguiData.speed[1] = 0; // 0.0f;
	imguiData.speed[2] = 0; // 0.0f;
	imguiData.accel[0] = 0; // 0.0f;
	imguiData.accel[1] = 0; // 0.0f;
	imguiData.accel[2] = 0; // 0.0f;
	imguiData.accelStart[0] = 0; // 0.0f;
	imguiData.accelStart[1] = 0; // 0.0f;
	imguiData.accelStart[2] = 0; // 0.0f;
	imguiData.accelStage[0] = 0; // 0.0f;
	imguiData.accelStage[1] = 0; // 0.0f;
	imguiData.accelStage[2] = 0; // 0.0f;
}

void BubbleParticle::Emit()
{
	for (int i = 0; i < MAX_SIZE; i++)
	{
		if (!data[i].GetExist())
		{
			continue;
		}
		if (MAX_CNT < cnt)
		{
			data[i].SetExist(false);
			continue;
		}
		data[i].Update();
		data[i].SetAlpha(1.0f - (static_cast<float>(cnt) / static_cast<float>(MAX_CNT)));
	}

	if (MAX_CNT < cnt)
	{
		cnt = -1;
	}
	cnt++;
}

void BubbleParticle::ImGui()
{
	if (ImGui::TreeNode(u8"Bubble"))
	{
#if 0
		ImGui::InputInt3("speed", imguiData.speed);
		ImGui::InputInt3("accel", imguiData.accel);
		//ImGui::InputInt3("accelStart", imguiData.accelStart);
		//ImGui::InputInt3("accelStage", imguiData.accelStage);
#else
		ImGui::DragInt3("speed", imguiData.speed);
		ImGui::DragInt3("accel", imguiData.accel);
#endif
		ImGui::TreePop();
	}
}