#pragma once
#include <memory>
#include "static_mesh.h"
#include "GameLibFunctions.h"


class UI_Data
{
public:
	static_mesh pMesh;

	DirectX::XMFLOAT4 pos;			// ���W
	DirectX::XMFLOAT2 scale;		// ���S���W����̔��a
	DirectX::XMFLOAT4 color;		// �F
	float angle;					// �p�x
	bool exist;						// ���݃t���O

	DirectX::XMFLOAT2 texPos;
	DirectX::XMFLOAT2 texSize;
};

class HP_UI
{
private:
	static const int MAX_SIZE = 5;

	UI_Data data[MAX_SIZE];
	bool exist;
	float addY;
	float widthY;
	int timer;
	int aliveDataNum;
	int state;

public:
	HP_UI()
	{
		for (int i = 0; i < MAX_SIZE; i++)
		{
			createBillboard(&data[i].pMesh, L"./Data/Images/UI/hpUI.png");
		}
	}
	~HP_UI() {}

	void Init();
	void UnInit();
	void Update(Donya::Vector3 _bossPos);
	void Draw();
	void Imgui();
};

class UIManager
{
private:
	HP_UI hpUI;

public:
	UIManager() {}
	~UIManager() {}

	static UIManager* GetInstance()
	{
		static UIManager instance;
		return &instance;
	}

	void Init()
	{
		hpUI.Init();
	}
	void UnInit()
	{
		hpUI.UnInit();
	}
	void Update(Donya::Vector3 _bossPos)
	{
		hpUI.Update(_bossPos);
	}
	void Draw()
	{
		hpUI.Draw();
	}
	void Imgui()
	{
		hpUI.Imgui();
	}
};