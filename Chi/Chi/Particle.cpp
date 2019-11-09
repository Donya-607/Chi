#include "Particle.h"
#include "imgui.h"
#include "GameLibFunctions.h"
#include "Donya/FilePath.h"
#include "Donya/Useful.h"


void Particle::Init( DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _speed, DirectX::XMFLOAT3 _accel, float _scale, bool _exist )
{
	pos = _pos;
	speed = _speed;
	accel = _accel;
	color = DirectX::XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
	exist = _exist;
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
	emitting = true;
	originPos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	cnt = 0;
}

void FlashParticle::ImGuiDataInit()
{
	LoadParameter();

	imguiData.speed[0] = 0;
	imguiData.speed[1] = 0;
	imguiData.speed[2] = 0;
	//imguiData.accel[0] = 0;
	//imguiData.accel[1] = 0;
	//imguiData.accel[2] = 0;
	imguiData.accelStart[0] = 0;
	imguiData.accelStart[1] = 0;
	imguiData.accelStart[2] = 0;
	imguiData.accelStage[0] = 0;
	imguiData.accelStage[1] = 0;
	imguiData.accelStage[2] = 0;
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
	emitting = true;
	originPos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	cnt = 0;
}

void BubbleParticle::ImGuiDataInit()
{
	imguiData.speed[0] = 0;
	imguiData.speed[1] = 0;
	imguiData.speed[2] = 0;
	imguiData.accel[0] = 0;
	imguiData.accel[1] = 0;
	imguiData.accel[2] = 0;
	imguiData.accelStart[0] = 0;
	imguiData.accelStart[1] = 0;
	imguiData.accelStart[2] = 0;
	imguiData.accelStage[0] = 0;
	imguiData.accelStage[1] = 0;
	imguiData.accelStage[2] = 0;
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


void FireFryParticle::Set()
{
	for (int i = 0; i < MAX_SIZE; i++)
	{
		data[i].Init(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), 1.0f, false);
		cnt[i] = 0;
	}
	emitting = true;
	originPos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
}

void FireFryParticle::ImGuiDataInit()
{
	LoadParameter();

	imguiData.speed[0] = 0;
	imguiData.speed[1] = 0;
	imguiData.speed[2] = 0;
	//imguiData.accel[0] = 0;
	//imguiData.accel[1] = 0;
	//imguiData.accel[2] = 0;
	imguiData.accelStart[0] = 0;
	imguiData.accelStart[1] = 0;
	imguiData.accelStart[2] = 0;
	imguiData.accelStage[0] = 0;
	imguiData.accelStage[1] = 0;
	imguiData.accelStage[2] = 0;
}

void FireFryParticle::Emit()
{
	for (int i = 0; i < MAX_SIZE; i++)
	{
		if (!data[i].GetExist())
		{
			data[i].SetExist(true);
			break;
		}
		if (MAX_CNT < cnt[i])
		{
			data[i].SetExist(false);
			data[i].SetPos(originPos);
			cnt[i] = 0;
			continue;
		}
		data[i].Update();
		data[i].SetAlpha(1.0f - (static_cast<float>(cnt[i]) / static_cast<float>(MAX_CNT)));
		cnt[i]++;
	}
}

void FireFryParticle::ImGui()
{
	if (ImGui::TreeNode(u8"FireFry"))
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

void FireFryParticle::LoadParameter(bool isBinary)
{
	Donya::Serializer::Extension ext = (isBinary)
		? Donya::Serializer::Extension::BINARY
		: Donya::Serializer::Extension::JSON;
	std::string filePath = GenerateSerializePath(SERIAL_ID, ext);

	Donya::Serializer seria;
	seria.Load(ext, filePath.c_str(), SERIAL_ID, *this);
}

#if USE_IMGUI

void FireFryParticle::SaveParameter()
{
	Donya::Serializer::Extension bin = Donya::Serializer::Extension::BINARY;
	Donya::Serializer::Extension json = Donya::Serializer::Extension::JSON;
	std::string binPath = GenerateSerializePath(SERIAL_ID, bin);
	std::string jsonPath = GenerateSerializePath(SERIAL_ID, json);

	Donya::Serializer seria;
	seria.Save(bin, binPath.c_str(), SERIAL_ID, *this);
	seria.Save(json, jsonPath.c_str(), SERIAL_ID, *this);
}

void FireFryParticle::UseImGui()
{
	if (ImGui::BeginIfAllowed())
	{
		if (ImGui::TreeNode("FireFryParticle.AdjustData"))
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


void EruptionParticle::Set(DirectX::XMFLOAT3 _pos)
{
	for (int i = 0; i < MAX_SIZE; i++)
	{
		float _angle = static_cast<float>(rand() % 360) * 0.01745f;
		originPos[i] = DirectX::XMFLOAT3(_pos.x + 5.0f * cosf(_angle), _pos.y, _pos.z * sinf(_angle));
		data[i].SetExist(true);
		if (i < MAX_SIZE * 0.8f) // è„Ç…è„Ç™ÇÈÇ‚Ç¬
		{
			float _posY = static_cast<float>(rand() % 10) / 10.0f;
			data[i].Init(originPos[i], DirectX::XMFLOAT3(1.0f, _posY, 1.0f), DirectX::XMFLOAT3(0.0f, 0.05f, 0.0f), 1.0f, false);
		}
		else // â°Ç…è„Ç™ÇÈÇ‚Ç¬
		{
			float _posY = static_cast<float>(rand() % 5) / 10.0f;
			data[i].Init(originPos[i], DirectX::XMFLOAT3(1.5f, _posY, 1.5f), DirectX::XMFLOAT3(0.0f, 0.05f, 0.0f), 1.0f, false);
		}
		cnt[i] = 0;
	}
	emitting = true;
	state = 0;
	totalCnt = 0;
}

void EruptionParticle::ImGuiDataInit()
{
	LoadParameter();

	imguiData.speed[0] = 0;
	imguiData.speed[1] = 0;
	imguiData.speed[2] = 0;
	//imguiData.accel[0] = 0;
	//imguiData.accel[1] = 0;
	//imguiData.accel[2] = 0;
	imguiData.accelStart[0] = 0;
	imguiData.accelStart[1] = 0;
	imguiData.accelStart[2] = 0;
	imguiData.accelStage[0] = 0;
	imguiData.accelStage[1] = 0;
	imguiData.accelStage[2] = 0;
}

void EruptionParticle::Emit()
{
	if (emitting)
	{
		for (int i = 0; i < MAX_SIZE; i++)
		{
			if (MAX_CNT < cnt[i])
			{
				data[i].SetExist(false);
				cnt[i] = 0;
				continue;
			}

			if (state == 1)
			{
				data[i].AddSpeedY(data[i].GetAccel().y);
				data[i].AddPosY(data[i].GetSpeed().y);
			}

			int whichOne1 = rand() % 2;
			int whichOne2 = rand() % 2;
			if (whichOne1 == 0) data[i].AddPosX(data[i].GetSpeed().x);
			if (whichOne1 == 1) data[i].AddPosX(-data[i].GetSpeed().x);
			if (whichOne2 == 0) data[i].AddPosZ(data[i].GetSpeed().z);
			if (whichOne2 == 1) data[i].AddPosZ(-data[i].GetSpeed().z);

			data[i].SetAlpha(1.0f - (static_cast<float>(cnt[i]) / static_cast<float>(MAX_CNT)));
			cnt[i]++;
		}
		if (state == 0 && MAX_CNT / 2.0f <= totalCnt)
		{
			state = 1;
		}
		else if (MAX_CNT < totalCnt)
		{
			emitting = false;
			totalCnt = -1;
		}

		totalCnt++;
	}
}

void EruptionParticle::ImGui()
{
	if (ImGui::TreeNode(u8"FireFry"))
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

void EruptionParticle::LoadParameter(bool isBinary)
{
	Donya::Serializer::Extension ext = (isBinary)
		? Donya::Serializer::Extension::BINARY
		: Donya::Serializer::Extension::JSON;
	std::string filePath = GenerateSerializePath(SERIAL_ID, ext);

	Donya::Serializer seria;
	seria.Load(ext, filePath.c_str(), SERIAL_ID, *this);
}

#if USE_IMGUI

void EruptionParticle::SaveParameter()
{
	Donya::Serializer::Extension bin = Donya::Serializer::Extension::BINARY;
	Donya::Serializer::Extension json = Donya::Serializer::Extension::JSON;
	std::string binPath = GenerateSerializePath(SERIAL_ID, bin);
	std::string jsonPath = GenerateSerializePath(SERIAL_ID, json);

	Donya::Serializer seria;
	seria.Save(bin, binPath.c_str(), SERIAL_ID, *this);
	seria.Save(json, jsonPath.c_str(), SERIAL_ID, *this);
}

void EruptionParticle::UseImGui()
{
	if (ImGui::BeginIfAllowed())
	{
		if (ImGui::TreeNode("FireFryParticle.AdjustData"))
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