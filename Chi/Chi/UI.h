#pragma once
#include <memory>
#include "static_mesh.h"
#include "GameLibFunctions.h"


class UI_Data
{
public:

	DirectX::XMFLOAT4 pos;			// 座標
	DirectX::XMFLOAT2 scale;		// 中心座標からの半径
	DirectX::XMFLOAT4 color;		// 色
	float angle;					// 角度
	bool exist;						// 存在フラグ

	int type;
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

	static_mesh billboard;
public:
	HP_UI()
	{
		for (int i = 0; i < MAX_SIZE; i++)
		{
			createBillboard(&billboard, L"./Data/Images/UI/hpUI.png", DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1024.0f, 222.0f));
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