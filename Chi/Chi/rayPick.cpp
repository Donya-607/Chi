#include "rayPick.h"

float det(XMFLOAT3 vecA, XMFLOAT3 vecB, XMFLOAT3 vecC) {
	return ((vecA.x * vecB.y * vecC.z)
		+ (vecA.y * vecB.z * vecC.x)
		+ (vecA.z * vecB.x * vecC.y)
		- (vecA.x * vecB.z * vecC.y)
		- (vecA.y * vecB.x * vecC.z)
		- (vecA.z * vecB.y * vecC.x));
}


XMFLOAT3 convertModelSpace(const XMFLOAT3& _wm, const XMMATRIX& _worldMatrix)
{
	XMFLOAT4 _pos = { _wm.x, _wm.y, _wm.z ,1.0f };
	XMFLOAT4 _result = { 0,0,0,0 };
	XMVECTOR test;

	XMMATRIX _inverse_world = XMMatrixInverse(nullptr, _worldMatrix);
	XMFLOAT4X4 _inverse;
	XMStoreFloat4x4(&_inverse, _inverse_world);

	_result = { _pos.x*_inverse._11 + _pos.y*_inverse._21 + _pos.z*_inverse._31 + _pos.w*_inverse._41, _pos.x*_inverse._12 + _pos.y*_inverse._22 + _pos.z*_inverse._32 + _pos.w*_inverse._42, _pos.x*_inverse._13 + _pos.y*_inverse._23 + _pos.z*_inverse._33 + _pos.w*_inverse._43, _pos.x*_inverse._14 + _pos.y*_inverse._24 + _pos.z*_inverse._34 + _pos.w*_inverse._44 };

	//XMFLOAT4X4 _world;
	//XMStoreFloat4x4(&_world, _worldMatrix);

	//_result = { _pos.x*_world._11 + _pos.y*_world._21 + _pos.z*_world._31 + _pos.w*_world._41, _pos.x*_world._12 + _pos.y*_world._22 + _pos.z*_world._32 + _pos.w*_world._42, _pos.x*_world._13 + _pos.y*_world._23 + _pos.z*_world._33 + _pos.w*_world._43, _pos.x*_world._14 + _pos.y*_world._24 + _pos.z*_world._34 + _pos.w*_world._44 };


	return{ _result.x,_result.y,_result.z };
}


RayPic_Result RayPick(Ray _ray, static_mesh* _mesh, XMMATRIX& _world_matrix)
{
	RayPic_Result result;
	result.hit = false;
	result.hit_pos = { 0,0,0 };
	XMFLOAT3 vertex_pos[3];
	XMFLOAT3 vec[2];
	XMFLOAT3 d;
	XMFLOAT4 _model_pos;
	float distance = -1;
	_ray.pos = convertModelSpace(_ray.pos, _world_matrix);


	for (auto& p : _mesh->subsets)
	{
		for (std::vector<static_mesh::vertex>::iterator it = p.m_vertexes.begin(); it < p.m_vertexes.end(); it++)
		{
			vertex_pos[0] = it->position;
			it++;
			vertex_pos[1] = it->position;
			it++;
			vertex_pos[2] = it->position;

			vec[0] = makeVector(vertex_pos[0], vertex_pos[1]);

			vec[1] = makeVector(vertex_pos[0], vertex_pos[2]);

			float denominator = det(vec[0], vec[1], _ray.invers_direction);

			//ïΩçsÇ»ÇÁìñÇΩÇ¡ÇƒÇ»Ç¢
			if (denominator <= 0)
			{
				continue;
			}

			d = makeVector(vertex_pos[0], _ray.pos);

			float u, v, t;
			u = det(d, vec[1], _ray.invers_direction) / denominator;
			if ((u >= 0) && (u <= 1)) {
				v = det(vec[0], d, _ray.invers_direction) / denominator;
				if ((v >= 0) && (u + v <= 1)) {
					t = det(vec[0], vec[1], d) / denominator;

					if (t < 0) {
						continue;
					}
					else
					{
						result.hit = true;

						XMFLOAT4 _hit_pos = { _ray.pos.x + _ray.direction.x*t,_ray.pos.y + _ray.direction.y*t, _ray.pos.z + _ray.direction.z*t,1.0 };

						XMFLOAT3 check = { _hit_pos.x - _ray.pos.x, _hit_pos.y - _ray.pos.y, _hit_pos.z - _ray.pos.z };
						float dist = sqrtf(check.x*check.x + check.y*check.y + check.z*check.z);
						if (distance < 0)
						{
							_model_pos = _hit_pos;
							distance = dist;
						}
						else if (distance > dist)
						{
							_model_pos = _hit_pos;
							distance = dist;
						}

					}
				}

			}
		}
	}
	XMFLOAT4X4 world_matrix;
	XMStoreFloat4x4(&world_matrix, _world_matrix);
	XMFLOAT4 _world_pos = {
		_model_pos.x*world_matrix._11 + _model_pos.y*world_matrix._21 + _model_pos.z*world_matrix._31 + _model_pos.w*world_matrix._41, _model_pos.x*world_matrix._12 + _model_pos.y*world_matrix._22 + _model_pos.z*world_matrix._32 + _model_pos.w*world_matrix._42, _model_pos.x*world_matrix._13 + _model_pos.y*world_matrix._23 + _model_pos.z*world_matrix._33 + _model_pos.w*world_matrix._43, _model_pos.x*world_matrix._14 + _model_pos.y*world_matrix._24 + _model_pos.z*world_matrix._34 + _model_pos.w*world_matrix._44
	};


	result.hit_pos = { _world_pos.x / _world_pos.w,_world_pos.y / _world_pos.w,_world_pos.z / _world_pos.w };
	return result;

}

XMFLOAT3 makeVector(XMFLOAT3& _start, XMFLOAT3& _end)
{
	return{ _end.x - _start.x,_end.y - _start.y,_end.z - _start.z };
}

void normalizeVector(XMFLOAT3& _export, XMFLOAT3& _inport)
{
	float size = sqrtf(_inport.x*_inport.x + _inport.y*_inport.y + _inport.z*_inport.z);
	_export = { _inport.x / size, _inport.y / size, _inport.z / size };
}