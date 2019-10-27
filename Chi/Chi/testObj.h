#ifndef TESTOBJ_H_
#define TESTOBJ_H_

#include "physics.h"
#include "GameLibFunctions.h"
#include "imgui.h"

class testOBJ : public physics
{
public:
	static_mesh model;
	line_light lineLight;
	point_light pointLight;
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX S, R, T;
	DirectX::XMFLOAT4X4 world_view_projection;
	DirectX::XMFLOAT4X4 World;
	DirectX::XMFLOAT4 color;
	bool wireFlg;
	int useShaderNumber;
public:
	testOBJ() {}
	~testOBJ() {}
	void initialize(DirectX::XMFLOAT4 _color);
	void initialize(float r, float g, float b, float a);
	void update();
	void render();
	void imGui();
	static_mesh* getAddress()
	{
		return &model;
	}
	void saveData();
	void loadData();
};

class testOBJ2 : public physics
{
private:
	static_mesh model;
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX S, R, T;
	DirectX::XMFLOAT4X4 world_view_projection;
	DirectX::XMFLOAT4X4 World;
	DirectX::XMFLOAT4 color;
	bool wireFlg;
public:
	testOBJ2() {}
	~testOBJ2() {}
	void initialize(DirectX::XMFLOAT4 _color);
	void initialize(float r, float g, float b, float a);
	void update();
	void render1(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _direct);
	void render2(ID3D11ShaderResourceView* _resouce, DirectX::XMFLOAT4 _direct);
	void imGui();
	static_mesh* getAddress()
	{
		return &model;
	}
};

#endif // !TESTOBJ_H_
