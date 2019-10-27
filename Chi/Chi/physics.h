#ifndef PHYSICS_H_
#define PHYSICS_H_

#include <DirectXMath.h>

#define GRAVITY_ACCE 9.80665 //per_square_seconds

class physics
{
public:
	DirectX::XMFLOAT3 position;//位置
	DirectX::XMFLOAT3 angle;//角度(3軸,単位:rad)
	DirectX::XMFLOAT3 scale;//拡大率(3軸)
	DirectX::XMFLOAT3 initial_verocity;//初速度
	DirectX::XMFLOAT3 verocity;//速度
	DirectX::XMFLOAT3 accelerator;//加速度
	double elapsed_time;//経過時間
	bool collision;//当たり判定
public:
	physics() {}
	~physics() {}
	virtual void initialize() {}
	virtual void update() {}
	virtual void render() {}
	virtual void imGui() {}
};

#endif // !PHYSICS_H_
