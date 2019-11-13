#include "UI.h"
#include "imgui.h"


void HP_UI::Init()
{
	for (int i = 0; i < MAX_SIZE; i++)
	{
		data[i].scale = DirectX::XMFLOAT2(100.0f, 10.0f);
		data[i].angle = 0.0f;
		data[i].color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		data[i].exist = false;

		data[i].texPos = DirectX::XMFLOAT2(0.0f, 0.0f);
		data[i].texSize = DirectX::XMFLOAT2(1024.0f, 222.0f);
	}

	exist = true;
	addY = 100.0f;
	widthY = 15.0f;
	timer = 0;
	aliveDataNum = 0;
	state = 0;
}
void HP_UI::UnInit()
{

}
void HP_UI::Update(Donya::Vector3 _bossPos)
{
	if (exist)
	{
		for (int i = 0; i < MAX_SIZE; i++)
		{
			data[i].pos.x = _bossPos.x + 100.0f;
			data[i].pos.y = (_bossPos.y + addY) - (widthY * i);
			data[i].pos.z = _bossPos.z;
		}

		switch (state)
		{
		case 0:
			if (timer++ % 20 == 0 && aliveDataNum < MAX_SIZE)
			{
				data[aliveDataNum].exist = true;
				aliveDataNum++;
			}
			if (aliveDataNum == MAX_SIZE) state++;
			break;
		case 1:
			if (timer++ % 10 == 0 && -1 < aliveDataNum)
			{
				data[aliveDataNum].exist = false;
				aliveDataNum--;
			}
			if (aliveDataNum == -1)
			{
				exist = false;
			}
			break;
		}
	}
}
void HP_UI::Draw()
{
	DirectX::XMFLOAT4X4 viewProjection;
	DirectX::XMStoreFloat4x4(&viewProjection, getViewMatrix() * getProjectionMatrix());
	for (int i = 0; i < MAX_SIZE; i++)
	{
		if (data[i].exist)
		{
			billboardRender(&data[i].pMesh, viewProjection, data[i].pos, data[i].scale, data[i].angle, getCamPos(), data[i].texPos, data[i].texSize, data[i].color.w);
		}
	}
}
void HP_UI::Imgui()
{
	ImGui::Begin("HP_UI");
	ImGui::InputFloat("addY", &addY);
	ImGui::InputFloat("widthY", &widthY);
	ImGui::End();
}