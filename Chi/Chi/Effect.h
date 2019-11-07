#pragma once
#include <vector>

#include "skinned_mesh.h"
#include "Donya/Vector.h"
#include "Particle.h"
#include "Donya/Collision.h"

enum class EffectModel
{
	ERUPTION,
	SHIELD_DEVELOPMENT,
};

std::string GetEffectModelPath(EffectModel effectModel);


class EffectData
{
protected:
	DirectX::XMFLOAT3 pos;			// 座標
	DirectX::XMFLOAT3 angle;		// 角度
	DirectX::XMFLOAT4 color;		// 色
	bool exist;						// 存在フラグ
	float scale;					// 中心座標からの半径
	int time;						// 出現までの時間

public:
	EffectData()
		: pos(0.0f, 0.0f, 0.0f), angle(0.0f, 0.0f, 0.0f), color(1.0f, 1.0f, 1.0f, 1.0f), exist(false), scale(1.0f), time(0)
	{}
	~EffectData() {}

public: // Get関数
	DirectX::XMFLOAT3 GetPos() { return pos; }
	DirectX::XMFLOAT3 GetAngle() { return angle; }
	DirectX::XMFLOAT4 GetColor() { return color; }
	bool GetExist() { return exist; }
	float GetScale() { return scale; }
	int GetTime() { return time; }

public: // Set関数
	void SetPos(DirectX::XMFLOAT3 _pos) { pos = _pos; }
	void SetPosX(float _posX) { pos.x = _posX; }
	void SetPosY(float _posY) { pos.y = _posY; }
	void SetPosZ(float _posZ) { pos.z = _posZ; }
	void Setangle(DirectX::XMFLOAT3 _angle) { angle = _angle; }
	void SetColor(DirectX::XMFLOAT4 _color) { color = _color; }
	void SetAlpha(float _alpha) { color.w = _alpha; }
	void SetExist(bool _exist) { exist = _exist; }
	void SetScale(float _scale) { scale = _scale; }
	void SetTime(int _time) { time = _time; }
};

class EruptionEffect
{
public:
	static const int MAX_SIZE = 20;
	//	Donya::Sphere hitSphere[ORIGIN_MAX_SIZE];

private:
	static const int MAX_ANIM_FRAME = 20; // TODO : 仮の経過時間

	std::shared_ptr<skinned_mesh> pModel[MAX_SIZE];
	EffectData data[MAX_SIZE];
	std::vector<Donya::Sphere> hitSphere;

	int lengthData[MAX_SIZE];
	int angleData[MAX_SIZE];

	int state;
	DirectX::XMFLOAT3 pos;			// 出現位置
	bool activated;					// 出現中フラグ
	int timer;						// 経過時間
	int startTimer;					// 出現し始める時間
	int aliveNum;					// 出現中の番号

public:
	int startFrame;

public:
	EruptionEffect();
	~EruptionEffect() {}

public:
	void Init();
	void UnInit()
	{
		// no
	}

	// _pos : 出現位置, _startTimer : 更新され始める時間
	void SetData(Donya::Vector3 _pos, int _startTimer = 0);

	void Update();
	void Render( fbx_shader &HLSL );

	bool GetActivated() { return activated; }
	std::vector<Donya::Sphere> GetHitSphereVector() { return hitSphere; }

private:
	bool CollideToWall(Donya::Vector3& pos);
};

//class EffectReader
//{
//private:
//
//public:
//	EffectReader() {}
//	virtual ~EffectReader() {}
//
//	virtual void Init() {}
//	virtual void UnInit() {}
//
//	// pos : 出現位置, _startTimer : 更新され始める時間
//	virtual void SetData(Donya::Vector3 pos, int _startTimer = 0) {}
//	virtual void Set() {}
//	virtual void ReSet() {}
//
//	virtual void Update() {}
//	virtual void Update(Donya::Vector3 pos) {}
//	virtual void Render() {}
//
//	virtual void SetMAX_SIZE(int maxSize) {}
//
//	virtual std::vector<EruptionEffect> GetVector() {}
//	virtual bool GetAlive() {}
//};

class EruptionEffectManager/* : public EffectReader*/
{
private:
	static const int MAX_SIZE = 5;
	//	EruptionEffect eruptionEffect[MAX_SIZE];

	std::vector<EruptionEffect> eruptionEffect;

public:
	EruptionEffectManager()
	{
		eruptionEffect.resize(MAX_SIZE);
	}
	~EruptionEffectManager() {}

public:
	void Init() {}
	void UnInit() {}

	// _pos : 出現位置, _startTimer : 更新され始める時間
	void SetData(Donya::Vector3 _pos, int _startTimer = 0);

	// _bossPos : ボス座標, _playerPos : プレイヤー座標,
	void Update();
	void Render( fbx_shader &HLSL );

	std::vector<EruptionEffect> GetEruptionEffectVector() { return eruptionEffect; }
};

class ShieldDevelopment/* : public EffectReader*/
{
private:
	std::shared_ptr<skinned_mesh> pModel;
	EffectData data;

public:
	ShieldDevelopment();
	~ShieldDevelopment() {}

	void Init();
	void UnInit() {}

	void Set();
	void ReSet();

	void Update(Donya::Vector3 pos);
	void Render( fbx_shader &HLSL );
};

class EffectManager
{
private:
	static const int MAX_SIZE = 2;

	//std::vector<std::unique_ptr<EffectReader>> effect;
	EruptionEffectManager	eruqtionEffectManager;
	ShieldDevelopment		shieldDevelopment;

public:
	enum EffectType
	{
		ERUPTION, // 岩噴火
		SHIELD_DEVELOPMENT, // 盾ガード中
	};

public:
	EffectManager() {}
	~EffectManager() {}

	static EffectManager* GetInstance()
	{
		static EffectManager instance;
		return &instance;
	}

public:
	void Init();
	void UnInit() {}

	void Update();
	void Update(Donya::Vector3 pos);
	void Render( fbx_shader &HLSL );

	// effectType : エフェクト番号(EffectType参照), pos : 出現位置, startTime : 関数がコールされてから動き始める時間
	void Set(EffectType effectType, Donya::Vector3 pos, int startTime = 0);

	// effectType : エフェクト番号(EffectType参照)
	void Set(EffectType effectType);
	void ReSet(EffectType effectType);

	std::vector<EruptionEffect> GetEruptionEffectVector()
	{
		return eruqtionEffectManager.GetEruptionEffectVector();
	}
};