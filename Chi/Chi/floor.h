#ifndef FLOOR_H_
#define FLOOR_H_

#include "GameLibFunctions.h"
#include "physics.h"
#include "imgui.h"

class Floor : public physics
{
private:
	static_mesh OBJ;
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX S, R, T;
	DirectX::XMFLOAT4X4 world_view_projection;
	DirectX::XMFLOAT4X4 World;
	DirectX::XMFLOAT4 color;
	bool wireFlg;
public:
	Floor() {}
	~Floor() {}
	void initialize(const float r,const float g,const float b, const float a);
	void initialize(const DirectX::XMFLOAT4& _color);
	void update();
	void render();
	void imGui();
	void saveData();
	void loadData();
};

class Floor2 : public physics
{
private:
	static_mesh OBJ;
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX S, R, T;
	DirectX::XMFLOAT4X4 world_view_projection;
	DirectX::XMFLOAT4X4 World;
	DirectX::XMFLOAT4 color;
	bool wireFlg;
public:
	Floor2() {}
	~Floor2() {}
	void initialize(const float r, const float g, const float b, const float a);
	void initialize(const DirectX::XMFLOAT4& _color);
	void update();
	void render1(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _direct);
	void render2(ID3D11ShaderResourceView* _resouce,DirectX::XMFLOAT4 _direct);
	void imGui();
	void saveData();
	void loadData();
};


#endif // !FLOOR_H_
