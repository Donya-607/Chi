#pragma once
#include <vector>

#include "skinned_mesh.h"
#include "Donya/Vector.h"
#include "Particle.h"
#include "Donya/Collision.h"


enum class EffectModel
{
	ERUPTION,
	SHIELD_OPEN,
	SHIELD_DEVELOPMENT,
	SHIELD_IMPACT,
	LONG_ATTACK
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
	std::shared_ptr<skinned_mesh> pModel[MAX_SIZE];

private:
	static const int MAX_ANIM_FRAME = 20; // TODO : 仮の経過時間

	EffectData data[MAX_SIZE];
	EruptionParticle eruqtionParticle[MAX_SIZE];
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
	void Render(fbx_shader& HLSL);
	void z_Render(fbx_shader& HLSL);
	void bloom_Render(fbx_shader& HLSL);

	bool GetActivated() const { return activated; }
	std::vector<Donya::Sphere> &GetHitSphereVector() { return hitSphere; }
	void SetEnable(int num, bool _enable = false)
	{
		if (MAX_SIZE <= num) return;

		hitSphere[num].enable = _enable;
	}
	void SetExist(int num, bool _exist = false)
	{
		if (MAX_SIZE <= num) return;

		hitSphere[num].exist = _exist;
	}
	void SetDataExist(int num, bool _exist)
	{
		if (MAX_SIZE <= num) return;

		data[num].SetExist(_exist);
		eruqtionParticle[num].ReSet();
	}

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

		auto GenerateFBX = []()->std::shared_ptr<skinned_mesh>
		{
			std::shared_ptr<skinned_mesh> tmpFBX = std::make_shared<skinned_mesh>();
			loadFBX(tmpFBX.get(), GetEffectModelPath(EffectModel::ERUPTION));
			return tmpFBX;
		};
		for(int i = 0; i < static_cast<int>( eruptionEffect.size() );i++)
		{
			for (int j = 0; j < eruptionEffect[i].MAX_SIZE; j++)
			{
				eruptionEffect[i].pModel[j] = std::make_unique<skinned_mesh>();
				eruptionEffect[i].pModel[j] = GenerateFBX();
			}
		}
	}
	~EruptionEffectManager() {}

public:
	void Init() {}
	void UnInit() {}

	// _pos : 出現位置, _startTimer : 更新され始める時間
	void SetData(Donya::Vector3 _pos, int _startTimer = 0);

	void AllReSetExist()
	{
		for (int i = 0; i < MAX_SIZE; i++)
		{
			for (int j = 0; j < eruptionEffect[i].MAX_SIZE; j++)
			{
				eruptionEffect[i].SetDataExist(j, false);
			}
		}
	}
	void AllReSetCollision()
	{
		for (int i = 0; i < MAX_SIZE; i++)
		{
			for (int j = 0; j < eruptionEffect[i].MAX_SIZE; j++)
			{
				eruptionEffect[i].SetExist(j, false);
				eruptionEffect[i].SetEnable(j, false);
			}
		}
	}

	// _bossPos : ボス座標, _playerPos : プレイヤー座標,
	void Update();
	void Render(fbx_shader& HLSL);
	void z_Render(fbx_shader& HLSL);
	void bloom_Render(fbx_shader& HLSL);

	std::vector<EruptionEffect> &GetEruptionEffectVector() { return eruptionEffect; }
};

class LongAttackEffect
{
public:
	static const int MAX_SIZE = 12;
	//	Donya::Sphere hitSphere[ORIGIN_MAX_SIZE];
	std::shared_ptr<skinned_mesh> pModel[MAX_SIZE];

private:
	static const int MAX_ANIM_FRAME = 20; // TODO : 仮の経過時間
	static constexpr float SIZE = 275.0f;

	EffectData data[MAX_SIZE];
	std::vector<Donya::Sphere> hitSphere;


	int state;
	DirectX::XMFLOAT3 pos;			// 出現位置
	bool activated;					// 出現中フラグ
	int timer;						// 経過時間
	int startTimer;					// 出現し始める時間
	int aliveMaxNum;				// 出現する最大数
	int aliveMaxReductionNum;		// 出現する最大数から壁に当たって出さない数
	int aliveNum;					// 出現中の番号

public:
	LongAttackEffect();
	~LongAttackEffect() {}

public:
	void Init();
	void UnInit()
	{
		// no
	}

	// _pos : 出現位置, _startTimer : 更新され始める時間
	void SetData(Donya::Vector3 _playerPos, Donya::Vector3 _bossPos, int _startTimer = 0);

	void Update();
	void Render(fbx_shader& HLSL);
	void z_Render(fbx_shader& HLSL);
	void bloom_Render(fbx_shader& HLSL);

	void SetEnable(int num, bool _enable = false)
	{
		if (MAX_SIZE <= num) return;

		hitSphere[num].enable = _enable;
	}
	void SetExist(int num, bool _exist = false)
	{
		if (MAX_SIZE <= num) return;

		hitSphere[num].exist = _exist;
	}
	void SetDataExist(int num, bool _exist)
	{
		if (MAX_SIZE <= num) return;

		data[num].SetExist(_exist);
	}

	bool GetActivated() const { return activated; }
	std::vector<Donya::Sphere>& GetHitSphereVector() { return hitSphere; }

private:
	bool CollideToWall(Donya::Vector3 pos);
};

class LongAttackEffectManager/* : public EffectReader*/
{
private:
	static const int MAX_SIZE = 5;
	//	EruptionEffect eruptionEffect[MAX_SIZE];

	std::vector<LongAttackEffect> longAttackEffect;

public:
	LongAttackEffectManager()
	{
		longAttackEffect.resize(MAX_SIZE);

		auto GenerateFBX = []()->std::shared_ptr<skinned_mesh>
		{
			std::shared_ptr<skinned_mesh> tmpFBX = std::make_shared<skinned_mesh>();
			loadFBX(tmpFBX.get(), GetEffectModelPath(EffectModel::LONG_ATTACK));
			return tmpFBX;
		};
		for (size_t i = 0; i < longAttackEffect.size(); i++)
		{
			for (int j = 0; j < longAttackEffect[i].MAX_SIZE; j++)
			{
				longAttackEffect[i].pModel[j] = std::make_shared<skinned_mesh>();
				longAttackEffect[i].pModel[j] = GenerateFBX();
			}
		}


		/*std::shared_ptr<skinned_mesh> tmpFBX = std::make_shared<skinned_mesh>();
		loadFBX(tmpFBX.get(), GetEffectModelPath(EffectModel::LONG_ATTACK));

		SetModels(tmpFBX);*/
	}
	~LongAttackEffectManager() {}

public:
	void Init() {}
	void UnInit() {}

	void SetModels(std::shared_ptr<skinned_mesh>& _pModel)
	{
		for (size_t i = 0; i < longAttackEffect.size(); i++)
		{
			for (int j = 0; j < longAttackEffect[i].MAX_SIZE; j++)
			{
				longAttackEffect[i].pModel[j] = std::make_shared<skinned_mesh>();
				longAttackEffect[i].pModel[j] = _pModel;
			}
		}
	}

	void AllReSetExist()
	{
		for (int i = 0; i < MAX_SIZE; i++)
		{
			for (int j = 0; j < longAttackEffect[i].MAX_SIZE; j++)
			{
				longAttackEffect[i].SetDataExist(j, false);
			}
		}
	}
	void AllReSetCollision()
	{
		for (int i = 0; i < MAX_SIZE; i++)
		{
			for (int j = 0; j < longAttackEffect[i].MAX_SIZE; j++)
			{
				longAttackEffect[i].SetExist(j, false);
				longAttackEffect[i].SetEnable(j, false);
			}
		}
	}

	// _pos : 出現位置, _startTimer : 更新され始める時間
	void SetData(Donya::Vector3 _playerPos, Donya::Vector3 _bossPos, int _startTimer = 0);

	// _bossPos : ボス座標, _playerPos : プレイヤー座標,
	void Update();
	void Render(fbx_shader& HLSL);
	void z_Render(fbx_shader& HLSL);
	void bloom_Render(fbx_shader& HLSL);

	std::vector<LongAttackEffect>& GetLongAttackEffectVector() { return longAttackEffect; }
};

class DustEffect
{
private:
	static const int MAX_SIZE = 10;

	DustParticle dustParticle[MAX_SIZE];

public:
	DustEffect() {}
	~DustEffect() {}

public:
	void Init() {}
	void UnInit() {}

	void SetData(Donya::Vector3 _pos, int _stageNo);

	void Update();
	void Render(fbx_shader& HLSL);
	void bloom_Render(fbx_shader& HLSL);
};

class SparkEffect
{
private:
	static const int MAX_SIZE = 10;

	SparkParticle sparkParticle[MAX_SIZE];

public:
	SparkEffect() {}
	~SparkEffect() {}

public:
	void Init() {}
	void UnInit() {}

	// _pos : 出現位置
	void SetData(Donya::Vector3 _pos);

	void Update();
	void Render(fbx_shader& HLSL);
	void bloom_Render(fbx_shader& HLSL);
};

class JumpEffect
{
private:
	EruptionParticle		eruqtionParticle;
	DustParticle			dustParticle;

public:
	JumpEffect() {}
	~JumpEffect() {}

	void Init() {}
	void UnInit() {}

	void SetData(Donya::Vector3 _pos, int _stageNo);

	void Update();
	void Render(fbx_shader& HLSL);
	void bloom_Render(fbx_shader& HLSL);
};

class BossAttackMomentEffect
{
private:
	Particle data;
	static constexpr float len = 50.0f;
	int aliveFrameMAX;
	int cnt;
	static_mesh billboard[3];

public:
	BossAttackMomentEffect()
	{
		createBillboard(&billboard[0], L"./Data/Images/UI/CheckPoint.png", DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1480.0f, 1150.0f));
		createBillboard(&billboard[1], L"./Data/Images/UI/CheckPoint.png",DirectX::XMFLOAT2(0.0f, 1150.0f), DirectX::XMFLOAT2(1480.0f, 1150.0f));
		createBillboard(&billboard[2], L"./Data/Images/UI/CheckPoint.png", DirectX::XMFLOAT2(0.0f, 2300.0f), DirectX::XMFLOAT2(1480.0f, 1150.0f));
	}
	~BossAttackMomentEffect() {}

	void Set(int aliveFrameMax, int bossNo)
	{
		DirectX::XMFLOAT4 _posFloat4 = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		DirectX::XMFLOAT3 _speed = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		DirectX::XMFLOAT3 _accel = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		DirectX::XMFLOAT2 _scale = DirectX::XMFLOAT2(100.0f, 100.0f);

		data.Init(_posFloat4, _speed, _accel, _scale, bossNo);
		data.SetColor(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		aliveFrameMAX = aliveFrameMax;
		cnt = 0;
	}

	void Update(Donya::Vector3 bossPos, Donya::Vector3 playerPos)
	{
		if (aliveFrameMAX <= cnt)
		{
			data.SetExist(false);
		}
		if (data.GetExist())
		{
			Donya::Vector3 vec = playerPos - bossPos;
			vec.Normalize();

			data.SetPos(Donya::Vector4(vec.x * len, 50.0f, vec.z * len, 1.0f));
			data.SetAlpha(1.0f - (static_cast<float>(cnt) / static_cast<float>(aliveFrameMAX)));
			cnt++;
		}
	}

	void Draw()
	{
		setBlendMode_ADD(1.0f);
		DirectX::XMFLOAT4X4 viewProjection;
		DirectX::XMStoreFloat4x4(&viewProjection, getViewMatrix() * getProjectionMatrix());
		if (data.GetExist())
		{
			billboardRender(&billboard[data.GetType()], viewProjection, data.GetPos(), data.GetScale(), data.GetAngle(), getCamPos(), data.GetColor().w);
			//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
			//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
			//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
		}
		setBlendMode_ALPHA(1.0f);
	}
	void bloom_Draw()
	{
		setBlendMode_ADD(1.0f);
		DirectX::XMFLOAT4X4 viewProjection;
		DirectX::XMStoreFloat4x4(&viewProjection, getViewMatrix() * getProjectionMatrix());
		if (data.GetExist())
		{
			billboard_bloom_Render(&billboard[data.GetType()], viewProjection, data.GetPos(), data.GetScale(), data.GetAngle(), getCamPos(), {1.0f,1.0f,1.0f,1.0f}, data.GetColor().w);
			//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
			//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
			//billboardRender(&data[i].pMesh, viewProjection, data[i].GetPos(), data[i].GetScale(), data[i].GetAngle(), getCamPos(), data[i].GetTexPos(), data[i].GetTexSize());
		}
		setBlendMode_ALPHA(1.0f);
	}

};

class ShieldEffect
{
private:
	static const int MAX_SIZE = 5;
	ShieldParticle shieldParticle[MAX_SIZE];

public:
	ShieldEffect() {}
	~ShieldEffect() {}


	void Set(Donya::Vector3 _pos)
	{
		for (int i = 0; i < MAX_SIZE; i++)
		{
			if (!shieldParticle[i].GetEmitting())
			{
				shieldParticle[i].Set(_pos);
				break;
			}
		}
	}
	void Update()
	{
		for (int i = 0; i < MAX_SIZE; i++)
		{
			shieldParticle[i].Emit();
		}
	}
	void Draw()
	{
		for (int i = 0; i < MAX_SIZE; i++)
		{
			shieldParticle[i].Draw();
		}
	}
};

class EffectManager
{
private:
	static const int MAX_SIZE = 2;

	EruptionEffectManager	eruptionEffectManager;
	LongAttackEffectManager longAttackEffectManager;
	DustEffect				dustEffect;
	SparkEffect				sparkEffect;
	AbsorptionParticle		playerAbsorptionParticle;
	AbsorptionParticle		bossAbsorptionParticle;
	LocusParticle			locusParticle;
	JumpEffect				jumpEffect;
	AccelParticle			accelParticle;
	BossAttackMomentEffect	bossAttackMomentEffect;
	StoneBreakParticle		stoneBreakParticle;
	CatapultBreakParticle	catapultBreakParticle;
	ShieldEffect			shieldEffect;
	DisappearanceParticle	disappearanceParticle;

public:
	EffectManager() {}
	~EffectManager() {}

	static EffectManager* GetInstance()
	{
		static EffectManager instance;
		return &instance;
	}

public:
	void Init() {}
	void UnInit()
	{
		EruptionEffectReSetCollision();
		LongAttackEffectReSetCollision();
	}

	void Update();
	// pos : ボスの座標, dir : 方向ベクトル
	void AccelEffectUpdate(Donya::Vector3 pos, Donya::Vector3 dir);
	void BossAttackMomentEffectUpdate(Donya::Vector3 bossPos, Donya::Vector3 playerPos);
	void Render(fbx_shader& HLSL);
	void z_Render(fbx_shader& HLSL);
	void bloom_Render(fbx_shader& HLSL);

	//// effectType : エフェクト番号(EffectType参照), pos : 出現位置, startTime : 関数がコールされてから動き始める時間
	//void Set(EffectType effectType, Donya::Vector3 pos, int startTime = 0);
	//void Set(EffectType effectType, Donya::Vector3 pos, int stageNum, int startTime);
	//void Set(EffectType effectType, Donya::Vector3 playerPos, Donya::Vector3 bossPos);

	// 岩噴火
	void EruptionEffectSet(Donya::Vector3 pos)
	{
		eruptionEffectManager.SetData(pos);
	}
	void EruptionEffectReSetExist() // 表示OFF
	{
		eruptionEffectManager.AllReSetExist();
	}
	void EruptionEffectReSetCollision() // 当たり判定OFF
	{
		eruptionEffectManager.AllReSetCollision();
	}

	// 火花
	void SparkEffectSet(Donya::Vector3 pos)
	{
		sparkEffect.SetData(pos);
	}

	// 攻撃溜め
	void PlayerAbsorptionEffectSet(Donya::Vector3 pos)
	{
		playerAbsorptionParticle.Set(0, pos);
	}
	void PlayerAbsorptionEffectReSet()
	{
		playerAbsorptionParticle.ReSet();
	}

	// 爆破前の溜め
	void BossAbsorptionEffectSet(Donya::Vector3 pos)
	{
		bossAbsorptionParticle.Set(1, pos);
	}
	void BossAbsorptionEffectReSet()
	{
		bossAbsorptionParticle.ReSet();
	}

	// プレイヤー攻撃時のエフェクトとセット (Donya::Vector3 pos : 座標, Donya::Vector3 dir : 放出方向)
	void LocusParticleSet(Donya::Vector3 pos, Donya::Vector3 dir)
	{
		locusParticle.Set(pos, dir);
	}

	// 砂埃 (Donya::Vector3 pos : 座標, int stageNo : ステージ番号)
	void DustEffectSet(Donya::Vector3 pos, int stageNo)
	{
		dustEffect.SetData(pos, stageNo);
	}

	// BOSS3 遠距離攻撃
	void LongAttackEffectSet(Donya::Vector3 playerPos, Donya::Vector3 bossPos)
	{
		longAttackEffectManager.SetData(playerPos, bossPos);
	}
	void LongAttackEffectReSetExist() // 表示OFF
	{
		longAttackEffectManager.AllReSetExist();
	}
	void LongAttackEffectReSetCollision() // 当たり判定OFF
	{
		longAttackEffectManager.AllReSetCollision();
	}

	// BOSS1 ジャンプ攻撃
	void JumpEffectSet(Donya::Vector3 pos)
	{
		jumpEffect.SetData(pos, 0);
	}

	// BOSS3 突進攻撃
	void AccelEffectSet()
	{
		accelParticle.Set();
	}
	void AccelEffectReSet()
	{
		accelParticle.ReSet();
	}

	// BOSS 攻撃瞬間
	void BossAttackMomentEffectSet(int aliveFrameMax, int bossNo)
	{
		bossAttackMomentEffect.Set(aliveFrameMax, bossNo);
	}

	// チュートリアル 石破壊
	void StoneBreakEffectSet(Donya::Vector3 pos)
	{
		stoneBreakParticle.Set(pos);
	}
	
	// チュートリアル カタパルト破壊
	void CatapultBreakEffectSet(Donya::Vector3 pos)
	{
		catapultBreakParticle.Set(pos);
	}

	// プレイヤーシールド展開
	void ShieldEffectSet(Donya::Vector3 pos)
	{
		shieldEffect.Set(pos);
	}

	// ボス死亡パーティクル
	void DisappearanceEffectSet(Donya::Vector3 pos)
	{
		disappearanceParticle.Set(pos);
	}

	std::vector<EruptionEffect> &GetEruptionEffectVector()
	{
		return eruptionEffectManager.GetEruptionEffectVector();
	}
	std::vector<LongAttackEffect> &GetLongAttackEffectVector()
	{
		return longAttackEffectManager.GetLongAttackEffectVector();
	}

#ifdef USE_SHILED
	ShieldDevelopment &GetShieldDevelopment()
	{
		return shieldDevelopment;
	}
#endif
};