#include "Particle.h"
#include "imgui.h"
#include "GameLibFunctions.h"
#include "Donya/FilePath.h"
#include "Donya/Useful.h"
#include "easing.h"

//#define USE_ACCEL


void Particle::Init( DirectX::XMFLOAT4 _pos, DirectX::XMFLOAT3 _speed, DirectX::XMFLOAT3 _accel, DirectX::XMFLOAT2 _scale, DirectX::XMFLOAT2 _texPos, DirectX::XMFLOAT2 _texSize, bool _exist )
{
	pos = _pos;
	speed = _speed;
	accel = _accel;
	scale = _scale;
	exist = _exist;

	texPos = _texPos;
	texSize = _texSize;

	color = DirectX::XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
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
		data[i].Init(DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
			DirectX::XMFLOAT2(1.0f, 1.0f), DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f), 1.0f);
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
		billboardRender(&data[i].pMesh, view_projection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
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
		data[i].Init(DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
			DirectX::XMFLOAT2(1.0f, 1.0f), DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f), 1.0f);
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
		data[i].Init(DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
			DirectX::XMFLOAT2(1.0f, 1.0f), DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f), false);
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
		originPos[i] = DirectX::XMFLOAT4(_pos.x + 25.0f * cosf(_angle), _pos.y, _pos.z + 25.0f * sinf(_angle), 1.0f);
		DirectX::XMFLOAT3 vec;
		vec.x = originPos[i].x - _pos.x;
		vec.y = originPos[i].y - _pos.y;
		vec.z = originPos[i].z - _pos.z;
		DirectX::XMVECTOR _vec = DirectX::XMLoadFloat3(&vec);
		_vec = DirectX::XMVector3Normalize(_vec);
		DirectX::XMStoreFloat3(&vec, _vec);

		if (i < MAX_SIZE * 0.8f) // è„Ç…è„Ç™ÇÈÇ‚Ç¬
		{
			float _posY = static_cast<float>(rand() % 180) / 10.0f;
			float _posX = vec.x * (static_cast<float>(rand() % 20) / 10.0f);
			float _posZ = vec.z * (static_cast<float>(rand() % 20) / 10.0f);

			//if (rand() % 2 == 0) _posX *= -1;
			//if (rand() % 2 == 0) _posZ *= -1;

			data[i].Init(originPos[i], DirectX::XMFLOAT3(_posX, _posY, _posZ), DirectX::XMFLOAT3(0.0f, -0.5f, 0.0f),
				DirectX::XMFLOAT2(7.5f, 7.5f), DirectX::XMFLOAT2(0.0f, 240.0f), DirectX::XMFLOAT2(142.0f, 142.0f), true);
		}
		else // â°Ç…è„Ç™ÇÈÇ‚Ç¬
		{
			float _posY = static_cast<float>(rand() % 20) / 10.0f;
			float _posX = static_cast<float>(10 + rand() % 80) / 10.0f;
			float _posZ = static_cast<float>(10 + rand() % 80) / 10.0f;

			if (rand() % 2 == 0) _posX *= -1;
			if (rand() % 2 == 0) _posZ *= -1;

			data[i].Init(originPos[i], DirectX::XMFLOAT3(_posX, _posY, _posZ), DirectX::XMFLOAT3(0.0f, -0.05f, 0.0f),
				DirectX::XMFLOAT2(7.5f, 7.5f), DirectX::XMFLOAT2(0.0f, 240.0f), DirectX::XMFLOAT2(142.0f, 142.0f), true);
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
			billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
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


void AbsorptionParticle::Set(DirectX::XMFLOAT3 _pos)
{
	ImGuiDataInit();

	for (int i = 0; i < MAX_SIZE; i++)
	{
#if 0 // player
		data[i].Init(DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
			DirectX::XMFLOAT2(7.5f, 7.5f), DirectX::XMFLOAT2(0.0f, 630.0f), DirectX::XMFLOAT2(142.0f, 142.0f), false);
#else // boss2
		data[i].Init(DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
			DirectX::XMFLOAT2(7.5f, 7.5f), DirectX::XMFLOAT2(0.0f, 1420.0f), DirectX::XMFLOAT2(142.0f, 142.0f), false);
#endif
		data[i].SetPos(DirectX::XMFLOAT4(_pos.x + imguiData.radius * cosf(((360.0f / MAX_SIZE) * i) * 0.01745f), 
			_pos.y + (imguiData.radius * 2.0f) * (cosf(((360.0f / MAX_SIZE) * i) * 0.01745f) * sinf(((360.0f / MAX_SIZE) * i) * 0.01745f)), 
			_pos.z + imguiData.radius * sinf(((360.0f / MAX_SIZE) * i) * 0.01745f), 1.0f));
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
	sw = 0;
}

void AbsorptionParticle::ReSet()
{
	emitting = false;
}

void AbsorptionParticle::ImGuiDataInit()
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
	imguiData.radius = 75.0f;
}

void AbsorptionParticle::Emit()
{
	if (emitting)
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
		for (int i = 0; i < MAX_SIZE; i++)
		{
			if (data[i].GetExist())
			{
				if (MAX_CNT < cnt[i])
				{
					data[i].SetExist(false);
					data[i].SetPos(originPos[i]);
#ifdef USE_ACCEL
					data[i].SetSpeed(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
#endif
					cnt[i] = 0;
					continue;
				}
#ifdef USE_ACCEL
				DirectX::XMFLOAT3 _speed;
				_speed.x = easing::InQuart(static_cast<float>(cnt[i]), MAX_CNT * 2.0f, data[i].GetAccel().x, 0.0f);
				_speed.y = easing::InQuart(static_cast<float>(cnt[i]), MAX_CNT * 2.0f, data[i].GetAccel().y, 0.0f);
				_speed.z = easing::InQuart(static_cast<float>(cnt[i]), MAX_CNT * 2.0f, data[i].GetAccel().z, 0.0f);
				data[i].SetSpeed(_speed);
#endif
				data[i].AddPosX(data[i].GetSpeed().x);
				data[i].AddPosY(data[i].GetSpeed().y);
				data[i].AddPosZ(data[i].GetSpeed().z);

				data[i].SetAlpha(1.0f - (static_cast<float>(cnt[i]) / static_cast<float>(MAX_CNT)));
				cnt[i]++;
			}
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
				billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
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

void AbsorptionParticle::LoadParameter(bool isBinary)
{
	Donya::Serializer::Extension ext = (isBinary)
		? Donya::Serializer::Extension::BINARY
		: Donya::Serializer::Extension::JSON;
	std::string filePath = GenerateSerializePath(SERIAL_ID, ext);

	Donya::Serializer seria;
	seria.Load(ext, filePath.c_str(), SERIAL_ID, *this);
}

#if USE_IMGUI

void AbsorptionParticle::SaveParameter()
{
	Donya::Serializer::Extension bin = Donya::Serializer::Extension::BINARY;
	Donya::Serializer::Extension json = Donya::Serializer::Extension::JSON;
	std::string binPath = GenerateSerializePath(SERIAL_ID, bin);
	std::string jsonPath = GenerateSerializePath(SERIAL_ID, json);

	Donya::Serializer seria;
	seria.Save(bin, binPath.c_str(), SERIAL_ID, *this);
	seria.Save(json, jsonPath.c_str(), SERIAL_ID, *this);
}

void AbsorptionParticle::UseImGui()
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


void DustParticle::Set(DirectX::XMFLOAT3 _pos)
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

#if 0 // stage1
			int texPosRand = rand() % 3;
			switch (texPosRand)
			{
			case 0:
				data[i].Init(originPos[i], DirectX::XMFLOAT3(_posX, _posY, _posZ), DirectX::XMFLOAT3(-(_posX / 100.0f), -0.05f, -(_posZ / 100.0f)),
					DirectX::XMFLOAT2(7.5f, 7.5f), DirectX::XMFLOAT2(0.0f, 1010.0f), DirectX::XMFLOAT2(142.0f, 142.0f), true);
				break;
			case 1:
				data[i].Init(originPos[i], DirectX::XMFLOAT3(_posX, _posY, _posZ), DirectX::XMFLOAT3(-(_posX / 100.0f), -0.05f, -(_posZ / 100.0f)),
					DirectX::XMFLOAT2(7.5f, 7.5f), DirectX::XMFLOAT2(0.0f, 1150.0f), DirectX::XMFLOAT2(142.0f, 142.0f), true);
				break;
			case 2:
				data[i].Init(originPos[i], DirectX::XMFLOAT3(_posX, _posY, _posZ), DirectX::XMFLOAT3(-(_posX / 100.0f), -0.05f, -(_posZ / 100.0f)),
					DirectX::XMFLOAT2(7.5f, 7.5f), DirectX::XMFLOAT2(0.0f, 1270.0f), DirectX::XMFLOAT2(142.0f, 142.0f), true);
				break;
			}
#else // stage3
			int texPosRand = rand() % 3;
			switch (texPosRand)
			{
			case 0:
				data[i].Init(originPos[i], DirectX::XMFLOAT3(_posX, _posY, _posZ), DirectX::XMFLOAT3(-(_posX / 100.0f), -0.05f, -(_posZ / 100.0f)),
					DirectX::XMFLOAT2(7.5f, 7.5f), DirectX::XMFLOAT2(0.0f, 1670.0f), DirectX::XMFLOAT2(142.0f, 142.0f), true);
				break;
			case 1:
				data[i].Init(originPos[i], DirectX::XMFLOAT3(_posX, _posY, _posZ), DirectX::XMFLOAT3(-(_posX / 100.0f), -0.05f, -(_posZ / 100.0f)),
					DirectX::XMFLOAT2(7.5f, 7.5f), DirectX::XMFLOAT2(0.0f, 1800.0f), DirectX::XMFLOAT2(142.0f, 142.0f), true);
				break;
			case 2:
				data[i].Init(originPos[i], DirectX::XMFLOAT3(_posX, _posY, _posZ), DirectX::XMFLOAT3(-(_posX / 100.0f), -0.05f, -(_posZ / 100.0f)),
					DirectX::XMFLOAT2(7.5f, 7.5f), DirectX::XMFLOAT2(0.0f, 1920.0f), DirectX::XMFLOAT2(142.0f, 142.0f), true);
				break;
			}
#endif
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
			billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
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
			DirectX::XMFLOAT2(7.5f, 7.5f), DirectX::XMFLOAT2(0.0f, 900.0f), DirectX::XMFLOAT2(142.0f, 142.0f), true);

		data[i].SetPos(DirectX::XMFLOAT4(_pos.x , _pos.y, _pos.z, 1.0f));
		originPos[i] = data[i].GetPos();

		float radiusRand = (65 + rand() % 35) / 2.0f;
		DirectX::XMFLOAT3 nextPos = DirectX::XMFLOAT3(_pos.x + radiusRand * cosf(((360.0f / MAX_SIZE) * i) * 0.01745f),
			_pos.y + (rand() % 150)  / 2.0f,
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
#ifdef USE_ACCEL
					data[i].SetSpeed(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
#endif
					cnt[i] = 0;
					continue;
				}
#ifdef USE_ACCEL
				DirectX::XMFLOAT3 _speed;
				_speed.x = easing::InQuart(static_cast<float>(cnt[i]), MAX_CNT * 2.0f, data[i].GetAccel().x, 0.0f);
				_speed.y = easing::InQuart(static_cast<float>(cnt[i]), MAX_CNT * 2.0f, data[i].GetAccel().y, 0.0f);
				_speed.z = easing::InQuart(static_cast<float>(cnt[i]), MAX_CNT * 2.0f, data[i].GetAccel().z, 0.0f);
				data[i].SetSpeed(_speed);
#endif
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
				billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
				//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
				//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
				//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
			}
		}
		setBlendMode_ALPHA(1.0f);
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
			DirectX::XMFLOAT2(7.5f, 7.5f), DirectX::XMFLOAT2(0.0f, 780.0f), DirectX::XMFLOAT2(142.0f, 142.0f), true);

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
#ifdef USE_ACCEL
					data[i].SetSpeed(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
#endif
					cnt[i] = 0;
					continue;
				}
#ifdef USE_ACCEL
				DirectX::XMFLOAT3 _speed;
				_speed.x = easing::InQuart(static_cast<float>(cnt[i]), MAX_CNT * 2.0f, data[i].GetAccel().x, 0.0f);
				_speed.y = easing::InQuart(static_cast<float>(cnt[i]), MAX_CNT * 2.0f, data[i].GetAccel().y, 0.0f);
				_speed.z = easing::InQuart(static_cast<float>(cnt[i]), MAX_CNT * 2.0f, data[i].GetAccel().z, 0.0f);
				data[i].SetSpeed(_speed);
#endif
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
				billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
				//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
				//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
				//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
			}
		}
		setBlendMode_ALPHA(1.0f);
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