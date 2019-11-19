#pragma once

#include <memory>

#include "Donya/Vector.h"

class skinned_mesh;	// With pointer. because I'm not want include this at header.
struct fbx_shader;
class Stage
{
private:
	int stageNo;	// 1-based.
	std::unique_ptr<skinned_mesh> pModel;
	bool anim_flg;
public:
	Stage();
	~Stage();
public:
	void Init( int stageNo );
	void Uninit();
	void setAnimFlg(bool flg)
	{
		anim_flg = flg;
	}
	void Update();

	void Draw(fbx_shader& hlsl, const Donya::Vector4x4& matView, const Donya::Vector4x4& matProjection);
	void z_Draw(fbx_shader& hlsl, const Donya::Vector4x4& matView, const Donya::Vector4x4& matProjection);
	void bloom_Draw(fbx_shader& hlsl, const Donya::Vector4x4& matView, const Donya::Vector4x4& matProjection);
private:
	void LoadModel();
};
