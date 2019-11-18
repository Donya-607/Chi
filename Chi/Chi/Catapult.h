#pragma once

#include "skinned_mesh.h"
#include "Donya/Vector.h"
#include "Donya/Collision.h"
#include "imgui.h"
#include "GameLibFunctions.h"
#include "Donya/Sound.h"

class Stone
{
public:
	std::shared_ptr<skinned_mesh> pModel;
	Donya::Vector3 pos;
	Donya::Vector3 speed;
	Donya::Vector3 accel;
	Donya::Vector3 scale;
	Donya::Vector3 angle;
	bool exist;
//	Donya::Sphere hitSphere;
	Donya::OBB hitOBB;

public:
	Stone() :
		pModel(std::make_shared<skinned_mesh>()),
		pos(),
		speed(),
		accel(),
		scale(),
		angle(),
		exist(false),
		//hitSphere(),
		hitOBB()
	{}
	~Stone() 
	{
		Donya::Sound::Play(ROCK);
	}

	void Init(Donya::Vector3 _pos, Donya::Vector3 _speed, Donya::Vector3 _accel, Donya::Vector3 _scale, Donya::Vector3 _angle);
	void Update();
	void Draw(fbx_shader& HLSL, const Donya::Vector4x4& matView, const Donya::Vector4x4& matProjection);
	void z_Draw(fbx_shader& HLSL, const Donya::Vector4x4& matView, const Donya::Vector4x4& matProjection);
	void bloom_Draw(fbx_shader& HLSL, const Donya::Vector4x4& matView, const Donya::Vector4x4& matProjection);
	void UnInit();
	void ImGui();

private:
	Donya::Vector4x4 CalcWorldMatrix() const
	{
		Donya::Vector4x4 S = Donya::Vector4x4::MakeScaling(scale);
		Donya::Vector4x4 R = Donya::Vector4x4::MakeRotationEuler(angle);
		Donya::Vector4x4 T = Donya::Vector4x4::MakeTranslation(pos);
		return S * R * T;
	}
};

class Catapult
{
private:
	static const int MAX_ATTACK_CNT = 30;

	Donya::Vector3 pos;
	Donya::Vector3 scale;
	Donya::Vector3 angle;
	int attackCnt;


public:
	std::shared_ptr<skinned_mesh> pModel;
	Donya::OBB hitOBB;
	Stone stone;

public:
	Catapult() :
		pModel(std::make_shared<skinned_mesh>()),
		pos(),
		scale(),
		angle(),
		attackCnt(0),
		stone(),
		hitOBB()
	{}
	~Catapult() {}

	void Init(Donya::Vector3 _pos, Donya::Vector3 _scale, Donya::Vector3 _angle);
	void Update(Donya::Vector3 _playerPos);
	void Draw(fbx_shader& HLSL, const Donya::Vector4x4& matView, const Donya::Vector4x4& matProjection);
	void z_Draw(fbx_shader& HLSL, const Donya::Vector4x4& matView, const Donya::Vector4x4& matProjection);
	void bloom_Draw(fbx_shader& HLSL, const Donya::Vector4x4& matView, const Donya::Vector4x4& matProjection);
	void UnInit();
	void ImGui();
	void StartAnim()
	{
		setStopAnimation(pModel.get(), false);
		setAnimFlame(pModel.get(), 0);
	}

public:
	Donya::Vector3 GetPos() const { return pos; }

private:
	Donya::Vector4x4 CalcWorldMatrix() const
	{
		Donya::Vector4x4 S = Donya::Vector4x4::MakeScaling(scale);
		Donya::Vector4x4 R = Donya::Vector4x4::MakeRotationEuler(angle);
		Donya::Vector4x4 T = Donya::Vector4x4::MakeTranslation(pos);
		return S * R * T;
	}
};