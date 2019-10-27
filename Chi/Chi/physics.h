#ifndef PHYSICS_H_
#define PHYSICS_H_

#include <DirectXMath.h>

#define GRAVITY_ACCE 9.80665 //per_square_seconds

class physics
{
public:
	DirectX::XMFLOAT3 position;//�ʒu
	DirectX::XMFLOAT3 angle;//�p�x(3��,�P��:rad)
	DirectX::XMFLOAT3 scale;//�g�嗦(3��)
	DirectX::XMFLOAT3 initial_verocity;//�����x
	DirectX::XMFLOAT3 verocity;//���x
	DirectX::XMFLOAT3 accelerator;//�����x
	double elapsed_time;//�o�ߎ���
	bool collision;//�����蔻��
public:
	physics() {}
	~physics() {}
	virtual void initialize() {}
	virtual void update() {}
	virtual void render() {}
	virtual void imGui() {}
};

#endif // !PHYSICS_H_
