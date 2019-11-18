#include "Particle.h"
#include "imgui.h"
#include "GameLibFunctions.h"
#include "Donya/FilePath.h"
#include "Donya/Useful.h"

//#define USE_ACCEL


void Particle::Init(DirectX::XMFLOAT4 _pos, DirectX::XMFLOAT3 _speed, DirectX::XMFLOAT3 _accel, DirectX::XMFLOAT2 _scale, int _type, bool _exist)
{
	pos = _pos;
	speed = _speed;
	accel = _accel;
	scale = _scale;
	exist = _exist;

	type = _type;

	color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
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

void FlashParticle::Render()
{
	DirectX::XMFLOAT4X4 view_projection;
	DirectX::XMStoreFloat4x4(&view_projection, getViewMatrix() * getProjectionMatrix());
	for (int i = 0; i < MAX_SIZE; i++)
	{
	}
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
		cnt[i] = 0;
	}
	emitting = true;
	originPos = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
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
		originPos[i] = DirectX::XMFLOAT4(_pos.x + 50.0f * cosf(_angle), _pos.y, _pos.z + 50.0f * sinf(_angle), 1.0f);
		DirectX::XMFLOAT3 vec;
		vec.x = originPos[i].x - _pos.x;
		vec.y = originPos[i].y - _pos.y;
		vec.z = originPos[i].z - _pos.z;
		DirectX::XMVECTOR _vec = DirectX::XMLoadFloat3(&vec);
		_vec = DirectX::XMVector3Normalize(_vec);
		DirectX::XMStoreFloat3(&vec, _vec);

		if (i < MAX_SIZE * 0.8f) // è„Ç…è„Ç™ÇÈÇ‚Ç¬
		{
			float _posY = static_cast<float>(rand() % 360) / 10.0f;
			float _posX = vec.x * (static_cast<float>(rand() % 40) / 10.0f);
			float _posZ = vec.z * (static_cast<float>(rand() % 40) / 10.0f);

			//if (rand() % 2 == 0) _posX *= -1;
			//if (rand() % 2 == 0) _posZ *= -1;

			data[i].Init(originPos[i], DirectX::XMFLOAT3(_posX, _posY, _posZ), DirectX::XMFLOAT3(0.0f, -0.5f, 0.0f),
				DirectX::XMFLOAT2(7.5f, 7.5f), 0, true);
		}
		else // â°Ç…è„Ç™ÇÈÇ‚Ç¬
		{
			float _posY = static_cast<float>(rand() % 40) / 10.0f;
			float _posX = static_cast<float>(10 + rand() % 160) / 10.0f;
			float _posZ = static_cast<float>(10 + rand() % 160) / 10.0f;

			if (rand() % 2 == 0) _posX *= -1;
			if (rand() % 2 == 0) _posZ *= -1;

			data[i].Init(originPos[i], DirectX::XMFLOAT3(_posX, _posY, _posZ), DirectX::XMFLOAT3(0.0f, -0.05f, 0.0f),
				DirectX::XMFLOAT2(7.5f, 7.5f), 1, true);
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

			data[i].AddSpeedY(data[i].GetAccel().y);
			data[i].AddPosY(data[i].GetSpeed().y);

			/*int whichOne1 = rand() % 2;
			int whichOne2 = rand() % 2;
			if (whichOne1 == 0) data[i].AddPosX(data[i].GetSpeed().x);
			if (whichOne1 == 1) data[i].AddPosX(-data[i].GetSpeed().x);
			if (whichOne2 == 0) data[i].AddPosZ(data[i].GetSpeed().z);
			if (whichOne2 == 1) data[i].AddPosZ(-data[i].GetSpeed().z);*/

			data[i].AddPosX(data[i].GetSpeed().x);
			data[i].AddPosZ(data[i].GetSpeed().z);

			data[i].SetAlpha(1.0f - (static_cast<float>(cnt[i]) / static_cast<float>(MAX_CNT)));
			cnt[i]++;
		}

		if (MAX_CNT < totalCnt)
		{
			emitting = false;
			totalCnt = -1;
		}

		totalCnt++;
	}
}

void EruptionParticle::Draw()
{
	DirectX::XMFLOAT4X4 viewProjection;
	DirectX::XMStoreFloat4x4(&viewProjection, getViewMatrix() * getProjectionMatrix());
	for (int i = 0; i < MAX_SIZE; i++)
	{
		if (data[i].GetExist())
		{
			billboardRender(&billboard[data[i].GetType()], viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos());
		}
	}
}

void EruptionParticle::bloom_Draw()
{
	DirectX::XMFLOAT4X4 viewProjection;
	DirectX::XMStoreFloat4x4(&viewProjection, getViewMatrix() * getProjectionMatrix());
	for (int i = 0; i < MAX_SIZE; i++)
	{
		if (data[i].GetExist())
		{
			billboard_bloom_Render(&billboard[data[i].GetType()], viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos());
		}
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


void AbsorptionParticle::Init()
{
	ImGuiDataInit();
	emitting = true;
	end = false;
	sw = 0;
}

void AbsorptionParticle::Set(int type, DirectX::XMFLOAT3 _pos)
{
	ImGuiDataInit();

	for (int i = 0; i < MAX_SIZE; i++)
	{
		if (type == 0) // player
		{
			data[i].Init(DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
				DirectX::XMFLOAT2(7.5f, 7.5f), 0, false);
		}
		else if (type == 1) // boss2
		{
			data[i].Init(DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
				DirectX::XMFLOAT2(7.5f, 7.5f), 1, false);
		}

#if 0
		data[i].SetPos(DirectX::XMFLOAT4(_pos.x + imguiData.radius * cosf(((360.0f / MAX_SIZE) * i) * 0.01745f),
			_pos.y + (imguiData.radius * 2.0f) * (cosf(((360.0f / MAX_SIZE) * i) * 0.01745f) * sinf(((360.0f / MAX_SIZE) * i) * 0.01745f)),
			_pos.z + imguiData.radius * sinf(((360.0f / MAX_SIZE) * i) * 0.01745f), 1.0f));
#else
		DirectX::XMFLOAT3 _dir;
		_dir.x = (-100 + rand() % 200) / 100.0f;
		_dir.y = (-100 + rand() % 200) / 100.0f;
		_dir.z = (-100 + rand() % 200) / 100.0f;
		data[i].SetPos(DirectX::XMFLOAT4(_pos.x + imguiData.radius * _dir.x, _pos.y + imguiData.radius * _dir.y, _pos.z + imguiData.radius * _dir.z, 1.0f));
#endif
		originPos[i] = data[i].GetPos();
		DirectX::XMFLOAT3 vec;
		vec.x = _pos.x - data[i].GetPos().x;
		vec.y = _pos.y - data[i].GetPos().y;
		vec.z = _pos.z - data[i].GetPos().z;

#ifdef USE_ACCEL
		data[i].SetAccel(DirectX::XMFLOAT3(vec.x, vec.y, vec.z));
#else
		data[i].SetSpeed(DirectX::XMFLOAT3(vec.x / MAX_CNT, vec.y / MAX_CNT, vec.z / MAX_CNT));
#endif
		cnt[i] = 0;
	}
	emitting = true;
	end = false;
	sw = 0;
}

void AbsorptionParticle::ReSet()
{
	end = true;
}

void AbsorptionParticle::ImGuiDataInit()
{
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
	imguiData.radius = 75.0f;
}

void AbsorptionParticle::Emit()
{
	if (emitting)
	{
		if (!end)
		{
			if (sw)
			{
				int numCnt = 0;
				while (numCnt < 2)
				{
					int num = rand() % MAX_SIZE;
					if (!data[num].GetExist())
					{
						data[num].SetExist(true);
						break;
					}
					numCnt++;
				}
			}
			sw = 1 - sw;
		}
		int aliveCnt = 0;
		for (int i = 0; i < MAX_SIZE; i++)
		{
			if (data[i].GetExist())
			{
				if (MAX_CNT < cnt[i])
				{
					data[i].SetExist(false);
					data[i].SetPos(originPos[i]);
					cnt[i] = 0;
					continue;
				}

				data[i].AddPosX(data[i].GetSpeed().x);
				data[i].AddPosY(data[i].GetSpeed().y);
				data[i].AddPosZ(data[i].GetSpeed().z);

				data[i].SetAlpha(1.0f - (static_cast<float>(cnt[i]) / static_cast<float>(MAX_CNT)));
				cnt[i]++;
				aliveCnt++;
			}
		}
		if (end && aliveCnt == 0)
		{
			emitting = false;
		}
	}
}

void AbsorptionParticle::Draw()
{
	if (emitting)
	{
		DirectX::XMFLOAT4X4 viewProjection;
		DirectX::XMStoreFloat4x4(&viewProjection, getViewMatrix() * getProjectionMatrix());
		for (int i = 0; i < MAX_SIZE; i++)
		{
			if (data[i].GetExist())
			{
				//	setBlendMode_ADD(1.0f);
				billboardRender(&billboard[data[i].GetType()], viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos());
				//	billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
				//	setBlendMode_ALPHA(1.0f);
			}
		}
	}
}

void AbsorptionParticle::bloom_Draw()
{
	if (emitting)
	{
		DirectX::XMFLOAT4X4 viewProjection;
		DirectX::XMStoreFloat4x4(&viewProjection, getViewMatrix() * getProjectionMatrix());
		for (int i = 0; i < MAX_SIZE; i++)
		{
			if (data[i].GetExist())
			{
				//	setBlendMode_ADD(1.0f);
				billboard_bloom_Render(&billboard[data[i].GetType()], viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos());
				//	billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
				//	setBlendMode_ALPHA(1.0f);
			}
		}
	}
}

void AbsorptionParticle::ImGui()
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


void DustParticle::Set(DirectX::XMFLOAT3 _pos, int stageNum)
{
	for (int i = 0; i < MAX_SIZE; i++)
	{
		float _angle = static_cast<float>(rand() % 360) * 0.01745f;
		originPos[i] = DirectX::XMFLOAT4(_pos.x + 25.0f * cosf(_angle), _pos.y, _pos.z + 25.0f * sinf(_angle), 1.0f);
		DirectX::XMFLOAT3 vec;
		vec.x = originPos[i].x - _pos.x;
		vec.y = originPos[i].y - _pos.y;
		vec.z = originPos[i].z - _pos.z;
		DirectX::XMVECTOR _vec = DirectX::XMLoadFloat3(&vec);
		_vec = DirectX::XMVector3Normalize(_vec);
		DirectX::XMStoreFloat3(&vec, _vec);

		// â°Ç…è„Ç™ÇÈÇ‚Ç¬
		{
			float _posY = static_cast<float>(rand() % 30) / 10.0f;
			float _posX = static_cast<float>(5 + rand() % 30) / 10.0f;
			float _posZ = static_cast<float>(5 + rand() % 30) / 10.0f;

			if (rand() % 2 == 0) _posX *= -1;
			if (rand() % 2 == 0) _posZ *= -1;

			if (stageNum == 0 || stageNum == 1) // stage1, 2
			{
				int texPosRand = rand() % 3;
				switch (texPosRand)
				{
				case 0:
					data[i].Init(originPos[i], DirectX::XMFLOAT3(_posX, _posY, _posZ), DirectX::XMFLOAT3(-(_posX / 100.0f), -0.05f, -(_posZ / 100.0f)),
						DirectX::XMFLOAT2(7.5f, 7.5f), 0, true);
					break;
				case 1:
					data[i].Init(originPos[i], DirectX::XMFLOAT3(_posX, _posY, _posZ), DirectX::XMFLOAT3(-(_posX / 100.0f), -0.05f, -(_posZ / 100.0f)),
						DirectX::XMFLOAT2(7.5f, 7.5f), 1, true);
					break;
				case 2:
					data[i].Init(originPos[i], DirectX::XMFLOAT3(_posX, _posY, _posZ), DirectX::XMFLOAT3(-(_posX / 100.0f), -0.05f, -(_posZ / 100.0f)),
						DirectX::XMFLOAT2(7.5f, 7.5f), 2, true);
					break;
				}
			}
			else if (stageNum == 2) // stage3
			{
				int texPosRand = rand() % 3;
				switch (texPosRand)
				{
				case 0:
					data[i].Init(originPos[i], DirectX::XMFLOAT3(_posX, _posY, _posZ), DirectX::XMFLOAT3(-(_posX / 100.0f), -0.05f, -(_posZ / 100.0f)),
						DirectX::XMFLOAT2(7.5f, 7.5f), 3, true);
					break;
				case 1:
					data[i].Init(originPos[i], DirectX::XMFLOAT3(_posX, _posY, _posZ), DirectX::XMFLOAT3(-(_posX / 100.0f), -0.05f, -(_posZ / 100.0f)),
						DirectX::XMFLOAT2(7.5f, 7.5f), 4, true);
					break;
				case 2:
					data[i].Init(originPos[i], DirectX::XMFLOAT3(_posX, _posY, _posZ), DirectX::XMFLOAT3(-(_posX / 100.0f), -0.05f, -(_posZ / 100.0f)),
						DirectX::XMFLOAT2(7.5f, 7.5f), 5, true);
					break;
				}
			}
		}
		cnt[i] = 0;
	}
	emitting = true;
	state = 0;
	totalCnt = 0;
}

void DustParticle::ImGuiDataInit()
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

void DustParticle::Emit()
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


			data[i].AddSpeedX(data[i].GetAccel().x);
			data[i].AddSpeedY(data[i].GetAccel().y);
			data[i].AddSpeedZ(data[i].GetAccel().z);

			data[i].AddPosX(data[i].GetSpeed().x);
			data[i].AddPosY(data[i].GetSpeed().y);
			data[i].AddPosZ(data[i].GetSpeed().z);

			data[i].SetAlpha(1.0f - (static_cast<float>(cnt[i]) / static_cast<float>(MAX_CNT)));
			cnt[i]++;
		}

		if (MAX_CNT < totalCnt)
		{
			emitting = false;
			totalCnt = -1;
		}

		totalCnt++;
	}
}

void DustParticle::Draw()
{
	DirectX::XMFLOAT4X4 viewProjection;
	DirectX::XMStoreFloat4x4(&viewProjection, getViewMatrix() * getProjectionMatrix());
	for (int i = 0; i < MAX_SIZE; i++)
	{
		if (data[i].GetExist())
		{
			setBlendMode_ALPHA(data[i].GetColor().w);
			billboardRender(&billboard[data[i].GetType()], viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetColor().w);
			setBlendMode_ALPHA(1.0f);
		}
	}
}

void DustParticle::bloom_Draw()
{
	DirectX::XMFLOAT4X4 viewProjection;
	DirectX::XMStoreFloat4x4(&viewProjection, getViewMatrix() * getProjectionMatrix());
	for (int i = 0; i < MAX_SIZE; i++)
	{
		if (data[i].GetExist())
		{
			setBlendMode_ALPHA(data[i].GetColor().w);
			billboard_bloom_Render(&billboard[data[i].GetType()], viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), { 1.0f,1.0f,1.0f,1.0f }, data[i].GetColor().w);
			setBlendMode_ALPHA(1.0f);
		}
	}
}

void DustParticle::ImGui()
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

void DustParticle::LoadParameter(bool isBinary)
{
	Donya::Serializer::Extension ext = (isBinary)
		? Donya::Serializer::Extension::BINARY
		: Donya::Serializer::Extension::JSON;
	std::string filePath = GenerateSerializePath(SERIAL_ID, ext);

	Donya::Serializer seria;
	seria.Load(ext, filePath.c_str(), SERIAL_ID, *this);
}

#if USE_IMGUI

void DustParticle::SaveParameter()
{
	Donya::Serializer::Extension bin = Donya::Serializer::Extension::BINARY;
	Donya::Serializer::Extension json = Donya::Serializer::Extension::JSON;
	std::string binPath = GenerateSerializePath(SERIAL_ID, bin);
	std::string jsonPath = GenerateSerializePath(SERIAL_ID, json);

	Donya::Serializer seria;
	seria.Save(bin, binPath.c_str(), SERIAL_ID, *this);
	seria.Save(json, jsonPath.c_str(), SERIAL_ID, *this);
}

void DustParticle::UseImGui()
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


void SparkParticle::Set(DirectX::XMFLOAT3 _pos)
{
	ImGuiDataInit();

	for (int i = 0; i < MAX_SIZE; i++)
	{
		data[i].Init(DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
			DirectX::XMFLOAT2(7.5f, 7.5f), 0, true);

		data[i].SetPos(DirectX::XMFLOAT4(_pos.x, _pos.y, _pos.z, 1.0f));
		originPos[i] = data[i].GetPos();

		float radiusRand = (65 + rand() % 35) / 2.0f;
		DirectX::XMFLOAT3 nextPos = DirectX::XMFLOAT3(_pos.x + radiusRand * cosf(((360.0f / MAX_SIZE) * i) * 0.01745f),
			_pos.y + (rand() % 150) / 2.0f,
			_pos.z + radiusRand * sinf(((360.0f / MAX_SIZE) * i) * 0.01745f));

		DirectX::XMFLOAT3 vec;
		vec.x = nextPos.x - _pos.x;
		vec.y = nextPos.y - _pos.y;
		vec.z = nextPos.z - _pos.z;

#ifdef USE_ACCEL
		data[i].SetAccel(DirectX::XMFLOAT3(vec.x, vec.y, vec.z));
#else
		data[i].SetSpeed(DirectX::XMFLOAT3(vec.x / MAX_CNT * 2.0f, vec.y / MAX_CNT * 2.0f, vec.z / MAX_CNT * 2.0f));
		data[i].SetAccel(DirectX::XMFLOAT3(-(vec.x / MAX_CNT / 100000.0f), -0.2f, -(vec.x / MAX_CNT / 100000.0f)));

		originSpeed[i] = data[i].GetSpeed();
#endif
		cnt[i] = 0;
		//	alive[i] = false;
	}
	emitting = true;
}

void SparkParticle::ImGuiDataInit()
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
	//	imguiData.radius = 100.0f;
}

void SparkParticle::Emit()
{
	if (emitting)
	{
		/*int numCnt = 0;
		while (numCnt < 2)
		{
			int num = rand() % MAX_SIZE;
			if (!data[num].GetExist() && !alive[num])
			{
				data[num].SetExist(true);
				alive[num] = true;
				break;
			}
			numCnt++;
		}*/

		bool end = false;
		for (int i = 0; i < MAX_SIZE; i++)
		{
			if (data[i].GetExist())
			{
				if (MAX_CNT < cnt[i])
				{
					data[i].SetExist(false);
					data[i].SetPos(originPos[i]);
					data[i].SetSpeed(originSpeed[i]);
					cnt[i] = 0;
					continue;
				}

				data[i].AddSpeedX(data[i].GetAccel().x);
				data[i].AddSpeedY(data[i].GetAccel().y);
				data[i].AddSpeedZ(data[i].GetAccel().z);
				data[i].AddPosX(data[i].GetSpeed().x);
				data[i].AddPosY(data[i].GetSpeed().y);
				data[i].AddPosZ(data[i].GetSpeed().z);

				data[i].SetAlpha(1.0f - (static_cast<float>(cnt[i]) / static_cast<float>(MAX_CNT)));

				end = true;
				cnt[i]++;
			}
		}

		if (!end)
		{
			emitting = false;
		}
	}
}

void SparkParticle::Draw()
{
	if (emitting)
	{
		setBlendMode_ADD(1.0f);
		DirectX::XMFLOAT4X4 viewProjection;
		DirectX::XMStoreFloat4x4(&viewProjection, getViewMatrix() * getProjectionMatrix());
		for (int i = 0; i < MAX_SIZE; i++)
		{
			if (data[i].GetExist())
			{
				billboardRender(&billboard, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetColor().w);
				//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
				//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
				//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
			}
		}
		setBlendMode_ALPHA(1.0f);
	}
}

void SparkParticle::bloom_Draw()
{
	if (emitting)
	{
		//setBlendMode_ADD(1.0f);
		DirectX::XMFLOAT4X4 viewProjection;
		DirectX::XMStoreFloat4x4(&viewProjection, getViewMatrix() * getProjectionMatrix());
		for (int i = 0; i < MAX_SIZE; i++)
		{
			if (data[i].GetExist())
			{
				billboard_bloom_Render(&billboard, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), { 1.0f,1.0f,1.0f,1.0f }, data[i].GetColor().w);
				//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
				//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
				//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
			}
		}
		//setBlendMode_ALPHA(1.0f);
	}
}

void SparkParticle::ImGui()
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

void SparkParticle::LoadParameter(bool isBinary)
{
	Donya::Serializer::Extension ext = (isBinary)
		? Donya::Serializer::Extension::BINARY
		: Donya::Serializer::Extension::JSON;
	std::string filePath = GenerateSerializePath(SERIAL_ID, ext);

	Donya::Serializer seria;
	seria.Load(ext, filePath.c_str(), SERIAL_ID, *this);
}

#if USE_IMGUI

void SparkParticle::SaveParameter()
{
	Donya::Serializer::Extension bin = Donya::Serializer::Extension::BINARY;
	Donya::Serializer::Extension json = Donya::Serializer::Extension::JSON;
	std::string binPath = GenerateSerializePath(SERIAL_ID, bin);
	std::string jsonPath = GenerateSerializePath(SERIAL_ID, json);

	Donya::Serializer seria;
	seria.Save(bin, binPath.c_str(), SERIAL_ID, *this);
	seria.Save(json, jsonPath.c_str(), SERIAL_ID, *this);
}

void SparkParticle::UseImGui()
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


void LocusParticle::Set(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _dir)
{
	ImGuiDataInit();

	DirectX::XMFLOAT3 originVec(0.0f, 0.0f, 1.0f);
	DirectX::XMFLOAT3 boss_Player_Vec = _dir;
	DirectX::XMFLOAT3 _boss_Player_Vec = boss_Player_Vec;

	float l1, l2;
	l1 = sqrtf((originVec.x * originVec.x) + (originVec.y * originVec.y) + (originVec.z * originVec.z));
	l2 = sqrtf((boss_Player_Vec.x * boss_Player_Vec.x) + 0.0f + (boss_Player_Vec.z * boss_Player_Vec.z));
	boss_Player_Vec.x /= l2;
	boss_Player_Vec.y /= l2;
	boss_Player_Vec.z /= l2;

	float dot = 0.0f;
	float angle = 0.0f;
	l2 = sqrtf((boss_Player_Vec.x * boss_Player_Vec.x) + 0.0f + (boss_Player_Vec.z * boss_Player_Vec.z));
	dot = (originVec.x * boss_Player_Vec.x) + 0.0f + (originVec.z * boss_Player_Vec.z);
	angle = acos(dot / (l1 * l2)) / (3.141592f / 180.0f);

	if (_boss_Player_Vec.x < 0)
	{
		angle *= -1;
	}

	angle += 90;
	for (int i = 0; i < MAX_SIZE; i++)
	{
		data[i].Init(DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
			DirectX::XMFLOAT2(7.5f, 7.5f), 0, true);

		data[i].SetPos(DirectX::XMFLOAT4(_pos.x, _pos.y, _pos.z, 1.0f));
		originPos[i] = data[i].GetPos();

		int radiusRand = 65 + rand() % 35;
		float angleRand = static_cast<float>(-10 + rand() % 20);
		DirectX::XMFLOAT3 nextPos = DirectX::XMFLOAT3(_pos.x + radiusRand * cosf((angle + angleRand) * 0.01745f),
			_pos.y + (-50 + rand() % 100),
			_pos.z + radiusRand * sinf((angle + angleRand) * 0.01745f));

		DirectX::XMFLOAT3 vec;
		vec.x = nextPos.x - _pos.x;
		vec.y = nextPos.y - _pos.y;
		vec.z = nextPos.z - _pos.z;

		data[i].SetSpeed(DirectX::XMFLOAT3(vec.x / MAX_CNT * 2.0f, vec.y / MAX_CNT * 2.0f, vec.z / MAX_CNT * 2.0f));
		data[i].SetAccel(DirectX::XMFLOAT3(vec.x / MAX_CNT / 100000.0f, vec.y / MAX_CNT / 100000.0f, vec.x / MAX_CNT / 100000.0f));

		originSpeed[i] = data[i].GetSpeed();

		cnt[i] = 0;
		//	alive[i] = false;
	}
	emitting = true;
}

void LocusParticle::ImGuiDataInit()
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
	//	imguiData.radius = 100.0f;
}

void LocusParticle::Emit()
{
	if (emitting)
	{
		/*int numCnt = 0;
		while (numCnt < 2)
		{
			int num = rand() % MAX_SIZE;
			if (!data[num].GetExist() && !alive[num])
			{
				data[num].SetExist(true);
				alive[num] = true;
				break;
			}
			numCnt++;
		}*/

		bool end = false;
		for (int i = 0; i < MAX_SIZE; i++)
		{
			if (data[i].GetExist())
			{
				if (MAX_CNT < cnt[i])
				{
					data[i].SetExist(false);
					data[i].SetPos(originPos[i]);
					data[i].SetSpeed(originSpeed[i]);
					cnt[i] = 0;
					continue;
				}
				data[i].AddSpeedX(data[i].GetAccel().x);
				data[i].AddSpeedY(data[i].GetAccel().y);
				data[i].AddSpeedZ(data[i].GetAccel().z);
				data[i].AddPosX(data[i].GetSpeed().x);
				data[i].AddPosY(data[i].GetSpeed().y);
				data[i].AddPosZ(data[i].GetSpeed().z);

				data[i].SetAlpha(1.0f - (static_cast<float>(cnt[i]) / static_cast<float>(MAX_CNT)));

				end = true;
				cnt[i]++;
			}
		}

		if (!end)
		{
			emitting = false;
		}
	}
}

void LocusParticle::Draw()
{
	if (emitting)
	{
		setBlendMode_ADD(1.0f);
		DirectX::XMFLOAT4X4 viewProjection;
		DirectX::XMStoreFloat4x4(&viewProjection, getViewMatrix() * getProjectionMatrix());
		for (int i = 0; i < MAX_SIZE; i++)
		{
			if (data[i].GetExist())
			{
				billboardRender(&billboard, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetColor().w);
				//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
				//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
				//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
			}
		}
		setBlendMode_ALPHA(1.0f);
	}
}

void LocusParticle::bloom_Draw()
{
	if (emitting)
	{
		//setBlendMode_ADD(1.0f);
		DirectX::XMFLOAT4X4 viewProjection;
		DirectX::XMStoreFloat4x4(&viewProjection, getViewMatrix() * getProjectionMatrix());
		for (int i = 0; i < MAX_SIZE; i++)
		{
			if (data[i].GetExist())
			{
				billboard_bloom_Render(&billboard, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), { 1.0f,1.0f,1.0f,1.0f }, data[i].GetColor().w);
				//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
				//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
				//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
			}
		}
		//setBlendMode_ALPHA(1.0f);
	}

}

void LocusParticle::ImGui()
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

void LocusParticle::LoadParameter(bool isBinary)
{
	Donya::Serializer::Extension ext = (isBinary)
		? Donya::Serializer::Extension::BINARY
		: Donya::Serializer::Extension::JSON;
	std::string filePath = GenerateSerializePath(SERIAL_ID, ext);

	Donya::Serializer seria;
	seria.Load(ext, filePath.c_str(), SERIAL_ID, *this);
}

#if USE_IMGUI

void LocusParticle::SaveParameter()
{
	Donya::Serializer::Extension bin = Donya::Serializer::Extension::BINARY;
	Donya::Serializer::Extension json = Donya::Serializer::Extension::JSON;
	std::string binPath = GenerateSerializePath(SERIAL_ID, bin);
	std::string jsonPath = GenerateSerializePath(SERIAL_ID, json);

	Donya::Serializer seria;
	seria.Save(bin, binPath.c_str(), SERIAL_ID, *this);
	seria.Save(json, jsonPath.c_str(), SERIAL_ID, *this);
}

void LocusParticle::UseImGui()
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


void AccelParticle::Set()
{
	ImGuiDataInit();

	for (int i = 0; i < MAX_SIZE; i++)
	{
		originPos[i] = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		originSpeed[i] = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		cnt[i] = 0;
		alive[i] = false;
	}
	emitting = true;
	end = false;
}

void AccelParticle::ReSet()
{
	end = true;
}

void AccelParticle::ImGuiDataInit()
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
	//	imguiData.radius = 100.0f;
}

void AccelParticle::Emit(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _dir)
{
	if (emitting)
	{
		if (!end)
		{
			for (int i = 0; i < MAX_SIZE; i++)
			{
				if (!data[i].GetExist())
				{
					DirectX::XMFLOAT4 _posFloat4 = DirectX::XMFLOAT4(_pos.x, _pos.y, _pos.z, 1.0f);
					DirectX::XMFLOAT3 _speed = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
					DirectX::XMFLOAT3 _accel = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
					DirectX::XMFLOAT2 _scale = DirectX::XMFLOAT2(5.0f, 5.0f);

					_posFloat4.x += static_cast<float>(-10 + rand() % 20);
					_posFloat4.y += static_cast<float>(-10 + rand() % 20);
					_posFloat4.z += static_cast<float>(-10 + rand() % 20);

					float randSpeedX = (-25000 + static_cast<float>(rand() % 50000)) / 100;
					float randSpeedY = static_cast<float>(rand() % 10000) / 100;
					float randSpeedZ = (-25000 + static_cast<float>(rand() % 50000)) / 100;

					float randDirX = (-100 + static_cast<float>(rand() % 200)) / 100;
					float randDirZ = (-100 + static_cast<float>(rand() % 200)) / 100;

					bool minusX = false;
					bool minusZ = false;

					float _dirX = -_dir.x;
					float _dirZ = -_dir.z;

					if (_dir.x < 0)
					{
						_dirX *= -1;
						if (0 < randDirX)
						{
							randDirX *= -1;
						}
						minusX = true;
					}
					if (_dir.z < 0)
					{
						_dirZ *= -1;
						if (0 < randDirZ)
						{
							randDirZ *= -1;
						}
						minusZ = true;
					}

					if (0 < _dir.x)
					{
						if (randDirX < 0)
						{
							randDirX *= -1;
						}
					}
					if (0 < _dir.z)
					{
						if (randDirZ < 0)
						{
							randDirZ *= -1;
						}
					}

					_speed.x = randSpeedX * (_dirX + randDirX) / MAX_CNT;
					_speed.y = randSpeedY / MAX_CNT;
					_speed.z = randSpeedZ * (_dirX + randDirZ) / MAX_CNT;

					if (minusX) _speed.x *= -1;
					if (minusZ) _speed.z *= -1;

					_accel.x = _speed.x / MAX_CNT * -1;
					_accel.z = _speed.z / MAX_CNT * -1;
					_accel.y = _speed.y / (MAX_CNT * 4.0f);
					if (_accel.y < 0) _accel.y *= -1;

					data[i].Init(_posFloat4, _speed, _accel, _scale, 0);
					break;
				}
			}
		}

		int aliveCnt = 0;
		for (int i = 0; i < MAX_SIZE; i++)
		{
			if (data[i].GetExist())
			{
				if (MAX_CNT <= cnt[i]++)
				{
					cnt[i] = 0;
					data[i].SetExist(false);
					continue;
				}
				data[i].AddSpeedX(data[i].GetAccel().x);
				data[i].AddSpeedY(data[i].GetAccel().y);
				data[i].AddSpeedZ(data[i].GetAccel().z);

				data[i].AddPosX(data[i].GetSpeed().x);
				data[i].AddPosY(data[i].GetSpeed().y);
				data[i].AddPosZ(data[i].GetSpeed().z);
				aliveCnt++;
			}
		}

		if (end && aliveCnt == 0)
		{
			emitting = false;
		}
	}
}

void AccelParticle::Draw()
{
	if (emitting)
	{
		setBlendMode_ADD(1.0f);
		DirectX::XMFLOAT4X4 viewProjection;
		DirectX::XMStoreFloat4x4(&viewProjection, getViewMatrix() * getProjectionMatrix());
		for (int i = 0; i < MAX_SIZE; i++)
		{
			if (data[i].GetExist())
			{
				billboardRender(&billboard, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetColor().w);
				//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
				//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
				//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
			}
		}
		setBlendMode_ALPHA(1.0f);
	}
}

void AccelParticle::bloom_Draw()
{
	if (emitting)
	{
		setBlendMode_ADD(1.0f);
		DirectX::XMFLOAT4X4 viewProjection;
		DirectX::XMStoreFloat4x4(&viewProjection, getViewMatrix() * getProjectionMatrix());
		for (int i = 0; i < MAX_SIZE; i++)
		{
			if (data[i].GetExist())
			{
				billboard_bloom_Render(&billboard, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), { 1.0f,1.0f,1.0f,1.0f }, data[i].GetColor().w);
				//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
				//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
				//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
			}
		}
		setBlendMode_ALPHA(1.0f);
	}
}

void AccelParticle::ImGui()
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

void AccelParticle::LoadParameter(bool isBinary)
{
	Donya::Serializer::Extension ext = (isBinary)
		? Donya::Serializer::Extension::BINARY
		: Donya::Serializer::Extension::JSON;
	std::string filePath = GenerateSerializePath(SERIAL_ID, ext);

	Donya::Serializer seria;
	seria.Load(ext, filePath.c_str(), SERIAL_ID, *this);
}

#if USE_IMGUI

void AccelParticle::SaveParameter()
{
	Donya::Serializer::Extension bin = Donya::Serializer::Extension::BINARY;
	Donya::Serializer::Extension json = Donya::Serializer::Extension::JSON;
	std::string binPath = GenerateSerializePath(SERIAL_ID, bin);
	std::string jsonPath = GenerateSerializePath(SERIAL_ID, json);

	Donya::Serializer seria;
	seria.Save(bin, binPath.c_str(), SERIAL_ID, *this);
	seria.Save(json, jsonPath.c_str(), SERIAL_ID, *this);
}

void AccelParticle::UseImGui()
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

void StoneBreakParticle::Set(DirectX::XMFLOAT3 _pos)
{
	ImGuiDataInit();

	for (int i = 0; i < MAX_SIZE; i++)
	{
		data[i].Init(DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
			DirectX::XMFLOAT2(30.0f, 30.0f), rand() % 3, true);

		data[i].SetPos(DirectX::XMFLOAT4(_pos.x, _pos.y, _pos.z, 1.0f));
		originPos[i] = data[i].GetPos();

		float radiusRand = (65 + rand() % 35) / 2.0f;
		DirectX::XMFLOAT3 nextPos = DirectX::XMFLOAT3(_pos.x + radiusRand * cosf(((360.0f / MAX_SIZE) * i) * 0.01745f),
			_pos.y + (rand() % 150) / 2.0f,
			_pos.z + radiusRand * sinf(((360.0f / MAX_SIZE) * i) * 0.01745f));

		DirectX::XMFLOAT3 vec;
		vec.x = nextPos.x - _pos.x;
		vec.y = nextPos.y - _pos.y;
		vec.z = nextPos.z - _pos.z;

#ifdef USE_ACCEL
		data[i].SetAccel(DirectX::XMFLOAT3(vec.x, vec.y, vec.z));
#else
		data[i].SetSpeed(DirectX::XMFLOAT3(vec.x / MAX_CNT * 4.0f, vec.y / MAX_CNT * 4.0f, vec.z / MAX_CNT * 4.0f));
		data[i].SetAccel(DirectX::XMFLOAT3(-(vec.x / MAX_CNT / 100000.0f), -0.2f, -(vec.x / MAX_CNT / 100000.0f)));
		data[i].SetAlpha(1.0f);

		originSpeed[i] = data[i].GetSpeed();
#endif
		cnt[i] = 0;
		//	alive[i] = false;
	}
	emitting = true;
}

void StoneBreakParticle::ImGuiDataInit()
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
	//	imguiData.radius = 100.0f;
}

void StoneBreakParticle::Emit()
{
	if (emitting)
	{
		/*int numCnt = 0;
		while (numCnt < 2)
		{
			int num = rand() % MAX_SIZE;
			if (!data[num].GetExist() && !alive[num])
			{
				data[num].SetExist(true);
				alive[num] = true;
				break;
			}
			numCnt++;
		}*/

		bool end = false;
		for (int i = 0; i < MAX_SIZE; i++)
		{
			if (data[i].GetExist())
			{
				if (MAX_CNT * 2.0f < cnt[i])
				{
					data[i].SetExist(false);
					data[i].SetPos(originPos[i]);
					data[i].SetSpeed(originSpeed[i]);
					cnt[i] = 0;
					continue;
				}

				data[i].AddSpeedX(data[i].GetAccel().x);
				data[i].AddSpeedY(data[i].GetAccel().y);
				data[i].AddSpeedZ(data[i].GetAccel().z);
				data[i].AddPosX(data[i].GetSpeed().x);
				data[i].AddPosY(data[i].GetSpeed().y);
				data[i].AddPosZ(data[i].GetSpeed().z);

				data[i].SetAlpha(1.0f - (static_cast<float>(cnt[i]) / static_cast<float>(MAX_CNT * 2.0f)));

				end = true;
				cnt[i]++;
			}
		}

		if (!end)
		{
			emitting = false;
		}
	}
}

void StoneBreakParticle::Draw()
{
	if (emitting)
	{
		//setBlendMode_ADD(1.0f);
		DirectX::XMFLOAT4X4 viewProjection;
		DirectX::XMStoreFloat4x4(&viewProjection, getViewMatrix() * getProjectionMatrix());
		for (int i = 0; i < MAX_SIZE; i++)
		{
			if (data[i].GetExist())
			{
				billboardRender(&billboard[data[i].GetType()], viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetColor().w);
				//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
				//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
				//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
			}
		}
		//setBlendMode_ALPHA(1.0f);
	}
}

void StoneBreakParticle::bloom_Draw()
{
	if (emitting)
	{
		//setBlendMode_ADD(1.0f);
		DirectX::XMFLOAT4X4 viewProjection;
		DirectX::XMStoreFloat4x4(&viewProjection, getViewMatrix() * getProjectionMatrix());
		for (int i = 0; i < MAX_SIZE; i++)
		{
			if (data[i].GetExist())
			{
				billboard_bloom_Render(&billboard[data[i].GetType()], viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), { 1.0f,1.0f,1.0f,1.0f }, data[i].GetColor().w);
				//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
				//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
				//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
			}
		}
		//setBlendMode_ALPHA(1.0f);
	}
}

void StoneBreakParticle::ImGui()
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

void StoneBreakParticle::LoadParameter(bool isBinary)
{
	Donya::Serializer::Extension ext = (isBinary)
		? Donya::Serializer::Extension::BINARY
		: Donya::Serializer::Extension::JSON;
	std::string filePath = GenerateSerializePath(SERIAL_ID, ext);

	Donya::Serializer seria;
	seria.Load(ext, filePath.c_str(), SERIAL_ID, *this);
}

#if USE_IMGUI

void StoneBreakParticle::SaveParameter()
{
	Donya::Serializer::Extension bin = Donya::Serializer::Extension::BINARY;
	Donya::Serializer::Extension json = Donya::Serializer::Extension::JSON;
	std::string binPath = GenerateSerializePath(SERIAL_ID, bin);
	std::string jsonPath = GenerateSerializePath(SERIAL_ID, json);

	Donya::Serializer seria;
	seria.Save(bin, binPath.c_str(), SERIAL_ID, *this);
	seria.Save(json, jsonPath.c_str(), SERIAL_ID, *this);
}

void StoneBreakParticle::UseImGui()
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

void CatapultBreakParticle::Set(DirectX::XMFLOAT3 _pos)
{
	ImGuiDataInit();

	for (int i = 0; i < MAX_SIZE; i++)
	{
		data[i].Init(DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
			DirectX::XMFLOAT2(50.0f, 50.0f), rand()%3, true);

		data[i].SetPos(DirectX::XMFLOAT4(_pos.x, _pos.y, _pos.z, 1.0f));
		originPos[i] = data[i].GetPos();

		float radiusRand = (65 + rand() % 35) / 2.0f;
		DirectX::XMFLOAT3 nextPos = DirectX::XMFLOAT3(_pos.x + radiusRand * cosf(((360.0f / MAX_SIZE) * i) * 0.01745f),
			_pos.y + (rand() % 150) / 2.0f,
			_pos.z + radiusRand * sinf(((360.0f / MAX_SIZE) * i) * 0.01745f));

		DirectX::XMFLOAT3 vec;
		vec.x = nextPos.x - _pos.x;
		vec.y = nextPos.y - _pos.y;
		vec.z = nextPos.z - _pos.z;

#ifdef USE_ACCEL
		data[i].SetAccel(DirectX::XMFLOAT3(vec.x, vec.y, vec.z));
#else
		data[i].SetSpeed(DirectX::XMFLOAT3(vec.x / MAX_CNT * 8.0f, vec.y / MAX_CNT * 8.0f, vec.z / MAX_CNT * 8.0f));
		data[i].SetAccel(DirectX::XMFLOAT3(-(vec.x / MAX_CNT / 100000.0f), -0.2f, -(vec.x / MAX_CNT / 100000.0f)));
		data[i].SetAlpha(1.0f);

		originSpeed[i] = data[i].GetSpeed();
#endif
		cnt[i] = 0;
		//	alive[i] = false;
	}
	emitting = true;
}

void CatapultBreakParticle::ImGuiDataInit()
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
	//	imguiData.radius = 100.0f;
}

void CatapultBreakParticle::Emit()
{
	if (emitting)
	{
		/*int numCnt = 0;
		while (numCnt < 2)
		{
			int num = rand() % MAX_SIZE;
			if (!data[num].GetExist() && !alive[num])
			{
				data[num].SetExist(true);
				alive[num] = true;
				break;
			}
			numCnt++;
		}*/

		bool end = false;
		for (int i = 0; i < MAX_SIZE; i++)
		{
			if (data[i].GetExist())
			{
				if (MAX_CNT * 4.0f < cnt[i])
				{
					data[i].SetExist(false);
					data[i].SetPos(originPos[i]);
					data[i].SetSpeed(originSpeed[i]);
					cnt[i] = 0;
					continue;
				}

				data[i].AddSpeedX(data[i].GetAccel().x);
				data[i].AddSpeedY(data[i].GetAccel().y);
				data[i].AddSpeedZ(data[i].GetAccel().z);
				data[i].AddPosX(data[i].GetSpeed().x);
				data[i].AddPosY(data[i].GetSpeed().y);
				data[i].AddPosZ(data[i].GetSpeed().z);

				data[i].SetAlpha(1.0f - (static_cast<float>(cnt[i]) / static_cast<float>(MAX_CNT * 4.0f)));

				end = true;
				cnt[i]++;
			}
		}

		if (!end)
		{
			emitting = false;
		}
	}
}

void CatapultBreakParticle::Draw()
{
	if (emitting)
	{
		//setBlendMode_ADD(1.0f);
		DirectX::XMFLOAT4X4 viewProjection;
		DirectX::XMStoreFloat4x4(&viewProjection, getViewMatrix() * getProjectionMatrix());
		for (int i = 0; i < MAX_SIZE; i++)
		{
			if (data[i].GetExist())
			{
				billboardRender(&billboard[data[i].GetType()], viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetColor().w);
				//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
				//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
				//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
			}
		}
		//setBlendMode_ALPHA(1.0f);
	}
}

void CatapultBreakParticle::bloom_Draw()
{
	if (emitting)
	{
		//setBlendMode_ADD(1.0f);
		DirectX::XMFLOAT4X4 viewProjection;
		DirectX::XMStoreFloat4x4(&viewProjection, getViewMatrix() * getProjectionMatrix());
		for (int i = 0; i < MAX_SIZE; i++)
		{
			if (data[i].GetExist())
			{
				billboard_bloom_Render(&billboard[data[i].GetType()], viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), { 1.0f,1.0f,1.0f,1.0f }, data[i].GetColor().w);
				//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
				//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
				//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
			}
		}
		//setBlendMode_ALPHA(1.0f);
	}
}

void CatapultBreakParticle::ImGui()
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

void CatapultBreakParticle::LoadParameter(bool isBinary)
{
	Donya::Serializer::Extension ext = (isBinary)
		? Donya::Serializer::Extension::BINARY
		: Donya::Serializer::Extension::JSON;
	std::string filePath = GenerateSerializePath(SERIAL_ID, ext);

	Donya::Serializer seria;
	seria.Load(ext, filePath.c_str(), SERIAL_ID, *this);
}

#if USE_IMGUI

void CatapultBreakParticle::SaveParameter()
{
	Donya::Serializer::Extension bin = Donya::Serializer::Extension::BINARY;
	Donya::Serializer::Extension json = Donya::Serializer::Extension::JSON;
	std::string binPath = GenerateSerializePath(SERIAL_ID, bin);
	std::string jsonPath = GenerateSerializePath(SERIAL_ID, json);

	Donya::Serializer seria;
	seria.Save(bin, binPath.c_str(), SERIAL_ID, *this);
	seria.Save(json, jsonPath.c_str(), SERIAL_ID, *this);
}

void CatapultBreakParticle::UseImGui()
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


void ShieldParticle::Set(DirectX::XMFLOAT3 _pos)
{
	ImGuiDataInit();

	for (int i = 0; i < MAX_SIZE; i++)
	{
		DirectX::XMFLOAT4 _posFloat4 = DirectX::XMFLOAT4(_pos.x, _pos.y, _pos.z, 1.0f);
		DirectX::XMFLOAT3 _speed = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		DirectX::XMFLOAT3 _accel = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		DirectX::XMFLOAT2 _scale = DirectX::XMFLOAT2(7.5f, 7.5f);
		DirectX::XMFLOAT2 _texPos = DirectX::XMFLOAT2(0.0f, 630.0f);
		DirectX::XMFLOAT2 _texSize = DirectX::XMFLOAT2(142.0f, 142.0f);

		DirectX::XMFLOAT3 _dir;
		_dir.x = (-100 + rand() % 200) / 100.0f;
		_dir.y = (-100 + rand() % 200) / 100.0f;
		_dir.z = (-100 + rand() % 200) / 100.0f;
		_speed = DirectX::XMFLOAT3(imguiData.radius * _dir.x, imguiData.radius * _dir.y, imguiData.radius * _dir.z);

		DirectX::XMFLOAT3 vec;
		vec.x = _pos.x - data[i].GetPos().x;
		vec.y = _pos.y - data[i].GetPos().y;
		vec.z = _pos.z - data[i].GetPos().z;

		_accel = DirectX::XMFLOAT3(_speed.x / MAX_CNT * -1, _speed.y / MAX_CNT * -1, _speed.z / MAX_CNT * -1);

		originScale[i] = _scale;

		data[i].Init(_posFloat4, _speed, _accel, _scale, _texPos, _texSize);
	}
	emitting = true;
	cnt = -1;
}

void ShieldParticle::ImGuiDataInit()
{
	//LoadParameter();

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
	imguiData.radius = 10.0f;
}

void ShieldParticle::Emit()
{
	if (emitting)
	{
		if (MAX_CNT <= cnt++)
		{
			cnt = -1;
			emitting = false;
		}
		else
		{
			for (int i = 0; i < MAX_SIZE; i++)
			{
				data[i].AddSpeedX(data[i].GetAccel().x);
				data[i].AddSpeedY(data[i].GetAccel().y);
				data[i].AddSpeedZ(data[i].GetAccel().z);

				data[i].AddPosX(data[i].GetSpeed().x);
				data[i].AddPosY(data[i].GetSpeed().y);
				data[i].AddPosZ(data[i].GetSpeed().z);

				data[i].SetScale(originScale[i] - (static_cast<float>(cnt) / static_cast<float>(MAX_CNT)));
				data[i].SetAlpha(1.0f - (static_cast<float>(cnt) / static_cast<float>(MAX_CNT)));
			}
		}
	}
}

void ShieldParticle::Draw()
{
	if (emitting)
	{
		//setBlendMode_ADD(1.0f);
		DirectX::XMFLOAT4X4 viewProjection;
		DirectX::XMStoreFloat4x4(&viewProjection, getViewMatrix() * getProjectionMatrix());
		for (int i = 0; i < MAX_SIZE; i++)
		{
			if (data[i].GetExist())
			{
				billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize(), data[i].GetColor().w);
				//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
				//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
				//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
			}
		}
		//setBlendMode_ALPHA(1.0f);
	}
}

void ShieldParticle::ImGui()
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

void ShieldParticle::LoadParameter(bool isBinary)
{
	Donya::Serializer::Extension ext = (isBinary)
		? Donya::Serializer::Extension::BINARY
		: Donya::Serializer::Extension::JSON;
	std::string filePath = GenerateSerializePath(SERIAL_ID, ext);

	Donya::Serializer seria;
	seria.Load(ext, filePath.c_str(), SERIAL_ID, *this);
}

#if USE_IMGUI

void ShieldParticle::SaveParameter()
{
	Donya::Serializer::Extension bin = Donya::Serializer::Extension::BINARY;
	Donya::Serializer::Extension json = Donya::Serializer::Extension::JSON;
	std::string binPath = GenerateSerializePath(SERIAL_ID, bin);
	std::string jsonPath = GenerateSerializePath(SERIAL_ID, json);

	Donya::Serializer seria;
	seria.Save(bin, binPath.c_str(), SERIAL_ID, *this);
	seria.Save(json, jsonPath.c_str(), SERIAL_ID, *this);
}

void ShieldParticle::UseImGui()
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


void DisappearanceParticle::Set(DirectX::XMFLOAT3 _pos)
{
	ImGuiDataInit();

	for (int i = 0; i < MAX_SIZE; i++)
	{
		DirectX::XMFLOAT4 _posFloat4 = DirectX::XMFLOAT4(_pos.x, _pos.y, _pos.z, 1.0f);
		DirectX::XMFLOAT3 _speed = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		DirectX::XMFLOAT3 _accel = DirectX::XMFLOAT3(0.0f, 0.25f, 0.0f);
		DirectX::XMFLOAT2 _scale = DirectX::XMFLOAT2(7.5f, 7.5f);
		DirectX::XMFLOAT2 _texPos = DirectX::XMFLOAT2(0.0f, 1420.0f);
		DirectX::XMFLOAT2 _texSize = DirectX::XMFLOAT2(142.0f, 142.0f);

		data[i].Init(_posFloat4, _speed, _accel, _scale, _texPos, _texSize, false);

		radius[i] = 10.0f + static_cast<float>(rand() % 10);
		angle[i] = static_cast<float>(rand() % 360);
		cnt[i] = 0;
	}
	originPos = _pos;
	emitting = true;
}

void DisappearanceParticle::ImGuiDataInit()
{
	//LoadParameter();

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
	imguiData.radius = 10.0f;
}

void DisappearanceParticle::Emit()
{
	if (emitting)
	{
		for (int i = 0; i < MAX_SIZE; i++)
		{
			if (!data[i].GetExist())
			{
				data[i].SetExist(true);
				break;
			}
		}

		bool exist = false;
		for (int i = 0; i < MAX_SIZE; i++)
		{
			if (MAX_CNT <= cnt[i])
			{
				data[i].SetExist(false);
			}
			if (data[i].GetExist())
			{
				data[i].SetPosX(originPos.x + radius[i] * cosf(angle[i] * 0.01745f));
				data[i].SetPosZ(originPos.z + radius[i] * sinf(angle[i] * 0.01745f));

				data[i].AddSpeedY(data[i].GetAccel().y);
				data[i].AddPosY(data[i].GetSpeed().y);

				radius[i] += 1.5f;
				angle[i] += 1.0f;
				exist = true;
				cnt[i]++;
			}
		}
		if (!exist)
		{
			emitting = false;
		}
	}
}

void DisappearanceParticle::Draw()
{
	if (emitting)
	{
		//setBlendMode_ADD(1.0f);
		DirectX::XMFLOAT4X4 viewProjection;
		DirectX::XMStoreFloat4x4(&viewProjection, getViewMatrix() * getProjectionMatrix());
		for (int i = 0; i < MAX_SIZE; i++)
		{
			if (data[i].GetExist())
			{
				billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize(), data[i].GetColor().w);
				//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
				//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
				//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
			}
		}
		//setBlendMode_ALPHA(1.0f);
	}
}

void DisappearanceParticle::ImGui()
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

void DisappearanceParticle::LoadParameter(bool isBinary)
{
	Donya::Serializer::Extension ext = (isBinary)
		? Donya::Serializer::Extension::BINARY
		: Donya::Serializer::Extension::JSON;
	std::string filePath = GenerateSerializePath(SERIAL_ID, ext);

	Donya::Serializer seria;
	seria.Load(ext, filePath.c_str(), SERIAL_ID, *this);
}

#if USE_IMGUI

void DisappearanceParticle::SaveParameter()
{
	Donya::Serializer::Extension bin = Donya::Serializer::Extension::BINARY;
	Donya::Serializer::Extension json = Donya::Serializer::Extension::JSON;
	std::string binPath = GenerateSerializePath(SERIAL_ID, bin);
	std::string jsonPath = GenerateSerializePath(SERIAL_ID, json);

	Donya::Serializer seria;
	seria.Save(bin, binPath.c_str(), SERIAL_ID, *this);
	seria.Save(json, jsonPath.c_str(), SERIAL_ID, *this);
}

void DisappearanceParticle::UseImGui()
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