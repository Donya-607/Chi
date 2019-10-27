#pragma once

#include "GameLibFunctions.h"

using namespace DirectX;
struct Ray
{
	XMFLOAT3 pos;
	XMFLOAT3 direction;
	XMFLOAT3 invers_direction;
};

struct RayPic_Result
{
	bool hit;
	XMFLOAT3 hit_pos;
};

extern float det(XMFLOAT3 vecA, XMFLOAT3 vecB, XMFLOAT3 vecC);

extern XMFLOAT3 makeVector(XMFLOAT3& _start, XMFLOAT3& _end);

extern void normalizeVector(XMFLOAT3& _export, XMFLOAT3& _inport);

extern XMFLOAT3 convertModelSpace(const XMFLOAT3& _wm,const XMMATRIX& _inverse_worldMatrix);

extern RayPic_Result RayPick(Ray _ray, static_mesh* _mesh,XMMATRIX& _world_matrix);