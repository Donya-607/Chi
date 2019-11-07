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
	DirectX::XMFLOAT3 pos;			// ���W
	DirectX::XMFLOAT3 angle;		// �p�x
	DirectX::XMFLOAT4 color;		// �F
	bool exist;						// ���݃t���O
	float scale;					// ���S���W����̔��a
	int time;						// �o���܂ł̎���

public:
	EffectData()
		: pos(0.0f, 0.0f, 0.0f), angle(0.0f, 0.0f, 0.0f), color(1.0f, 1.0f, 1.0f, 1.0f), exist(false), scale(1.0f), time(0)
	{}
	~EffectData() {}

public: // Get�֐�
	DirectX::XMFLOAT3 GetPos() { return pos; }
	DirectX::XMFLOAT3 GetAngle() { return angle; }
	DirectX::XMFLOAT4 GetColor() { return color; }
	bool GetExist() { return exist; }
	float GetScale() { return scale; }
	int GetTime() { return time; }

public: // Set�֐�
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
	static const int MAX_ANIM_FRAME = 20; // TODO : ���̌o�ߎ���

	std::shared_ptr<skinned_mesh> pModel[MAX_SIZE];
	EffectData data[MAX_SIZE];
	std::vector<Donya::Sphere> hitSphere;

	int lengthData[MAX_SIZE];
	int angleData[MAX_SIZE];

	int state;
	DirectX::XMFLOAT3 pos;			// �o���ʒu
	bool activated;					// �o�����t���O
	int timer;						// �o�ߎ���
	int startTimer;					// �o�����n�߂鎞��
	int aliveNum;					// �o�����̔ԍ�

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

	// _pos : �o���ʒu, _startTimer : �X�V����n�߂鎞��
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
//	// pos : �o���ʒu, _startTimer : �X�V����n�߂鎞��
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

	// _pos : �o���ʒu, _startTimer : �X�V����n�߂鎞��
	void SetData(Donya::Vector3 _pos, int _startTimer = 0);

	// _bossPos : �{�X���W, _playerPos : �v���C���[���W,
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
		ERUPTION, // �╬��
		SHIELD_DEVELOPMENT, // ���K�[�h��
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

	// effectType : �G�t�F�N�g�ԍ�(EffectType�Q��), pos : �o���ʒu, startTime : �֐����R�[������Ă��瓮���n�߂鎞��
	void Set(EffectType effectType, Donya::Vector3 pos, int startTime = 0);

	// effectType : �G�t�F�N�g�ԍ�(EffectType�Q��)
	void Set(EffectType effectType);
	void ReSet(EffectType effectType);

	std::vector<EruptionEffect> GetEruptionEffectVector()
	{
		return eruqtionEffectManager.GetEruptionEffectVector();
	}
};