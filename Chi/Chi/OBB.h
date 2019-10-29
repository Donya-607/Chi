#pragma once

#include "Donya/Vector.h"
#include "Donya/Quaternion.h"
#include "vector.h"


class OBB
{
public:
	Donya::Vector3 pos;
	Donya::Vector3 scale;
	Donya::Quaternion orientation;
	// bool judge;

public:
	OBB() {}
	~OBB() {}

public:
	bool JudgeOBB(const OBB* obb);

private:
	float LenSegOnSepAxis(DirectX::XMFLOAT3* sep, DirectX::XMFLOAT3* v1, DirectX::XMFLOAT3* v2, DirectX::XMFLOAT3* v3 = 0)
	{
		// 3�̓��ς̐�Βl�̘a�œ��e���������v�Z
		// ������Sep�͕W��������Ă��邱��
		float dot1 = ((sep->x * v1->x) + (sep->y * v1->y) + (sep->z * v1->z));
		float dot2 = ((sep->x * v2->x) + (sep->y * v2->y) + (sep->z * v2->z));
		float dot3 = 0.0f;
		if (!v3)
		{
			dot3 = 0.0f;
		}
		else
		{
			dot3 = ((sep->x * v3->x) + (sep->y * v3->y) + (sep->z * v3->z));
		}

		float r1 = fabs(dot1);
		float r2 = fabs(dot2);
		float r3 = v3 ? (fabs(dot3)) : 0;

		return r1 + r2 + r3;
	}
};