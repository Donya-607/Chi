#include "OBB.h"


bool OBB::JudgeOBB(const OBB* obb)
{
	DirectX::XMFLOAT3 Nv1_1;
	DirectX::XMFLOAT3 Nv2_1;
	DirectX::XMFLOAT3 Nv3_1;

	DirectX::XMFLOAT3 Nv1_2;
	DirectX::XMFLOAT3 Nv2_2;
	DirectX::XMFLOAT3 Nv3_2;


	DirectX::XMFLOAT3 v1_1;
	DirectX::XMFLOAT3 v2_1;
	DirectX::XMFLOAT3 v3_1;

	DirectX::XMFLOAT3 v1_2;
	DirectX::XMFLOAT3 v2_2;
	DirectX::XMFLOAT3 v3_2;


	//	‰ñ“]
	DirectX::XMFLOAT4X4 rota, rota2;

	rota = orientation.RequireRotationMatrix();

	Nv1_1.x = rota._11;
	Nv1_1.y = rota._12;
	Nv1_1.z = rota._13;

	Nv2_1.x = rota._21;
	Nv2_1.y = rota._22;
	Nv2_1.z = rota._23;

	Nv3_1.x = rota._31;
	Nv3_1.y = rota._32;
	Nv3_1.z = rota._33;

	rota2 = obb->orientation.RequireRotationMatrix();

	Nv1_2.x = rota2._11;
	Nv1_2.y = rota2._12;
	Nv1_2.z = rota2._13;

	Nv2_2.x = rota2._21;
	Nv2_2.y = rota2._22;
	Nv2_2.z = rota2._23;

	Nv3_2.x = rota2._31;
	Nv3_2.y = rota2._32;
	Nv3_2.z = rota2._33;

	v1_1.x = Nv1_1.x * scale.x / 2.0f;
	v1_1.y = Nv1_1.y * scale.x / 2.0f;
	v1_1.z = Nv1_1.z * scale.x / 2.0f;

	v2_1.x = Nv2_1.x * scale.y / 2.0f;
	v2_1.y = Nv2_1.y * scale.y / 2.0f;
	v2_1.z = Nv2_1.z * scale.y / 2.0f;

	v3_1.x = Nv3_1.x * scale.z / 2.0f;
	v3_1.y = Nv3_1.y * scale.z / 2.0f;
	v3_1.z = Nv3_1.z * scale.z / 2.0f;

	v1_2.x = Nv1_2.x * obb->scale.x / 2.0f;
	v1_2.y = Nv1_2.y * obb->scale.x / 2.0f;
	v1_2.z = Nv1_2.z * obb->scale.x / 2.0f;

	v2_2.x = Nv2_2.x * obb->scale.y / 2.0f;
	v2_2.y = Nv2_2.y * obb->scale.y / 2.0f;
	v2_2.z = Nv2_2.z * obb->scale.y / 2.0f;

	v3_2.x = Nv3_2.x * obb->scale.z / 2.0f;
	v3_2.y = Nv3_2.y * obb->scale.z / 2.0f;
	v3_2.z = Nv3_2.z * obb->scale.z / 2.0f;


	DirectX::XMFLOAT3 interval;

	interval.x = pos.x - obb->pos.x;
	interval.y = pos.y - obb->pos.y;
	interval.z = pos.z - obb->pos.z;

	float r1 = 0.0f;
	float r2 = 0.0f;
	float l = 0.0f;
	float dot = 0.0f;


	// •ª—£² v1_1
	r1 = sqrtf((v1_1.x * v1_1.x) + (v1_1.y * v1_1.y) + (v1_1.z * v1_1.z));
	r2 = LenSegOnSepAxis(&Nv1_1, &v1_2, &v2_2, &v3_2);
	dot = ((interval.x * Nv1_1.x) + (interval.y * Nv1_1.y) + (interval.z * Nv1_1.z));
	l = fabs(dot);
	if (l > r1 + r2)
	{
		return false;
	}

	// •ª—£² v2_1
	r1 = sqrtf((v2_1.x * v2_1.x) + (v2_1.y * v2_1.y) + (v2_1.z * v2_1.z));
	r2 = LenSegOnSepAxis(&Nv2_1, &v1_2, &v2_2, &v3_2);
	dot = ((interval.x * Nv2_1.x) + (interval.y * Nv2_1.y) + (interval.z * Nv2_1.z));
	l = fabs(dot);
	if (l > r1 + r2)
	{
		return false;
	}

	// •ª—£² v3_1
	r1 = sqrtf((v3_1.x * v3_1.x) + (v3_1.y * v3_1.y) + (v3_1.z * v3_1.z));
	r2 = LenSegOnSepAxis(&Nv3_1, &v1_2, &v2_2, &v3_2);
	dot = ((interval.x * Nv3_1.x) + (interval.y * Nv3_1.y) + (interval.z * Nv3_1.z));
	l = fabs(dot);
	if (l > r1 + r2)
	{
		return false;
	}


	// •ª—£² v1_2
	r1 = LenSegOnSepAxis(&Nv1_2, &v1_1, &v2_1, &v3_1);
	r2 = sqrtf((v1_2.x * v1_2.x) + (v1_2.y * v1_2.y) + (v1_2.z * v1_2.z));
	dot = ((interval.x * Nv1_2.x) + (interval.y * Nv1_2.y) + (interval.z * Nv1_2.z));
	l = fabs(dot);
	if (l > r1 + r2)
	{
		return false;
	}

	// •ª—£² v2_2
	r1 = LenSegOnSepAxis(&Nv2_2, &v1_1, &v2_1, &v3_1);
	r2 = sqrtf((v2_2.x * v2_2.x) + (v2_2.y * v2_2.y) + (v2_2.z * v2_2.z));
	dot = ((interval.x * Nv2_2.x) + (interval.y * Nv2_2.y) + (interval.z * Nv2_2.z));
	l = fabs(dot);
	if (l > r1 + r2)
	{
		return false;
	}

	// •ª—£² v3_2
	r1 = LenSegOnSepAxis(&Nv3_2, &v1_1, &v2_1, &v3_1);
	r2 = sqrtf((v3_2.x * v3_2.x) + (v3_2.y * v3_2.y) + (v3_2.z * v3_2.z));
	dot = ((interval.x * Nv3_2.x) + (interval.y * Nv3_2.y) + (interval.z * Nv3_2.z));
	l = fabs(dot);
	if (l > r1 + r2)
	{
		return false;
	}


	DirectX::XMFLOAT3 cross;

	// •ª—£² cross v1_1 + v1_2
	cross.x = v1_1.y * v1_2.z - v1_1.z * v1_2.y;
	cross.y = v1_1.z * v1_2.x - v1_1.x * v1_2.z;
	cross.z = v1_1.x * v1_2.y - v1_1.y * v1_2.x;

	r1 = LenSegOnSepAxis(&cross, &v2_1, &v3_1);
	r2 = LenSegOnSepAxis(&cross, &v2_2, &v3_2);
	dot = ((interval.x * cross.x) + (interval.y * cross.y) + (interval.z * cross.z));
	l = fabs(dot);
	if (l > r1 + r2)
	{
		return false;
	}

	// •ª—£² cross v1_1 + v2_2
	cross.x = v1_1.y * v2_2.z - v1_1.z * v2_2.y;
	cross.y = v1_1.z * v2_2.x - v1_1.x * v2_2.z;
	cross.z = v1_1.x * v2_2.y - v1_1.y * v2_2.x;

	r1 = LenSegOnSepAxis(&cross, &v2_1, &v3_1);
	r2 = LenSegOnSepAxis(&cross, &v1_2, &v3_2);
	dot = ((interval.x * cross.x) + (interval.y * cross.y) + (interval.z * cross.z));
	l = fabs(dot);
	if (l > r1 + r2)
	{
		return false;
	}

	// •ª—£² cross v1_1 + v3_2
	cross.x = v1_1.y * v3_2.z - v1_1.z * v3_2.y;
	cross.y = v1_1.z * v3_2.x - v1_1.x * v3_2.z;
	cross.z = v1_1.x * v3_2.y - v1_1.y * v3_2.x;

	r1 = LenSegOnSepAxis(&cross, &v2_1, &v3_1);
	r2 = LenSegOnSepAxis(&cross, &v1_2, &v2_2);
	dot = ((interval.x * cross.x) + (interval.y * cross.y) + (interval.z * cross.z));
	l = fabs(dot);
	if (l > r1 + r2)
	{
		return false;
	}

	// •ª—£² cross v2_1 + v1_2
	cross.x = v2_1.y * v1_2.z - v2_1.z * v1_2.y;
	cross.y = v2_1.z * v1_2.x - v2_1.x * v1_2.z;
	cross.z = v2_1.x * v1_2.y - v2_1.y * v1_2.x;

	r1 = LenSegOnSepAxis(&cross, &v1_1, &v3_1);
	r2 = LenSegOnSepAxis(&cross, &v2_2, &v3_2);
	dot = ((interval.x * cross.x) + (interval.y * cross.y) + (interval.z * cross.z));
	l = fabs(dot);
	if (l > r1 + r2)
	{
		return false;
	}

	// •ª—£² cross v2_1 + v2_2
	cross.x = v2_1.y * v2_2.z - v2_1.z * v2_2.y;
	cross.y = v2_1.z * v2_2.x - v2_1.x * v2_2.z;
	cross.z = v2_1.x * v2_2.y - v2_1.y * v2_2.x;

	r1 = LenSegOnSepAxis(&cross, &v1_1, &v3_1);
	r2 = LenSegOnSepAxis(&cross, &v1_2, &v3_2);
	dot = ((interval.x * cross.x) + (interval.y * cross.y) + (interval.z * cross.z));
	l = fabs(dot);
	if (l > r1 + r2)
	{
		return false;
	}

	// •ª—£² cross v2_1 + v3_2
	cross.x = v2_1.y * v3_2.z - v2_1.z * v3_2.y;
	cross.y = v2_1.z * v3_2.x - v2_1.x * v3_2.z;
	cross.z = v2_1.x * v3_2.y - v2_1.y * v3_2.x;

	r1 = LenSegOnSepAxis(&cross, &v1_1, &v3_1);
	r2 = LenSegOnSepAxis(&cross, &v1_2, &v2_2);
	dot = ((interval.x * cross.x) + (interval.y * cross.y) + (interval.z * cross.z));
	l = fabs(dot);
	if (l > r1 + r2)
	{
		return false;
	}

	// •ª—£² cross v3_1 + v1_2
	cross.x = v3_1.y * v1_2.z - v3_1.z * v1_2.y;
	cross.y = v3_1.z * v1_2.x - v3_1.x * v1_2.z;
	cross.z = v3_1.x * v1_2.y - v3_1.y * v1_2.x;

	r1 = LenSegOnSepAxis(&cross, &v1_1, &v2_1);
	r2 = LenSegOnSepAxis(&cross, &v2_2, &v3_2);
	dot = ((interval.x * cross.x) + (interval.y * cross.y) + (interval.z * cross.z));
	l = fabs(dot);
	if (l > r1 + r2)
	{
		return false;
	}

	// •ª—£² cross v3_1 + v2_2
	cross.x = v3_1.y * v2_2.z - v3_1.z * v2_2.y;
	cross.y = v3_1.z * v2_2.x - v3_1.x * v2_2.z;
	cross.z = v3_1.x * v2_2.y - v3_1.y * v2_2.x;

	r1 = LenSegOnSepAxis(&cross, &v1_1, &v2_1);
	r2 = LenSegOnSepAxis(&cross, &v1_2, &v3_2);
	dot = ((interval.x * cross.x) + (interval.y * cross.y) + (interval.z * cross.z));
	l = fabs(dot);
	if (l > r1 + r2)
	{
		return false;
	}

	// •ª—£² cross v3_1 + v3_2
	cross.x = v3_1.y * v3_2.z - v3_1.z * v3_2.y;
	cross.y = v3_1.z * v3_2.x - v3_1.x * v3_2.z;
	cross.z = v3_1.x * v3_2.y - v3_1.y * v3_2.x;

	r1 = LenSegOnSepAxis(&cross, &v1_1, &v2_1);
	r2 = LenSegOnSepAxis(&cross, &v1_2, &v2_2);
	dot = ((interval.x * cross.x) + (interval.y * cross.y) + (interval.z * cross.z));
	l = fabs(dot);
	if (l > r1 + r2)
	{
		return false;
	}


	return true;
}