#pragma once

#include <memory>
#include <vector>
#include <DirectXMath.h>

#include "static_mesh.h"
#include "skinned_mesh.h"
#include "GameLibFunctions.h"
#include "Donya/Serializer.h"
#include "Donya/UseImGui.h"


class Particle
{
public:
	static_mesh pMesh;

protected:
	DirectX::XMFLOAT4 pos;			// 座標
	DirectX::XMFLOAT3 speed;		// 速度
	DirectX::XMFLOAT3 accel;		// 加速度
	DirectX::XMFLOAT2 scale;		// 中心座標からの半径
	DirectX::XMFLOAT4 color;		// 色
	float angle;					// 角度
	bool exist;						// 存在フラグ

	DirectX::XMFLOAT2 texPos;
	DirectX::XMFLOAT2 texSize;

public:
	Particle()
		: pMesh(),
		pos(0.0f, 0.0f, 0.0f, 1.0f), speed(0.0f, 0.0f, 0.0f),
		accel(0.0f, 0.0f, 0.0f), color(1.0f, 1.0f, 1.0f, 1.0f),
		angle(0.0f), exist(false), scale(1.0f, 1.0f),
		texPos(0.0f, 0.0f), texSize(0.0f, 0.0f)
	{}
	~Particle() {}

	void Init(DirectX::XMFLOAT4 _pos, DirectX::XMFLOAT3 _speed, DirectX::XMFLOAT3 _accel, DirectX::XMFLOAT2 _scale, DirectX::XMFLOAT2 _texPos, DirectX::XMFLOAT2 _texSize, bool _exist = true);
	void Update();

public: // Get関数
//	static_mesh GetMesh()					{ return pMesh;		}
	DirectX::XMFLOAT4 GetPos() { return pos; }
	DirectX::XMFLOAT3 GetSpeed() { return speed; }
	DirectX::XMFLOAT3 GetAccel() { return accel; }
	DirectX::XMFLOAT2 GetScale() { return scale; }
	DirectX::XMFLOAT4 GetColor() { return color; }
	float GetAngle() { return angle; }
	bool GetExist() { return exist; }

	DirectX::XMFLOAT2 GetTexPos() { return texPos; }
	DirectX::XMFLOAT2 GetTexSize() { return texSize; }

public: // Set関数
//	void SetMesh( static_mesh _pMesh )					{ pMesh = _pMesh;		}
	void SetPos(DirectX::XMFLOAT4 _pos) { pos = _pos; }
	void SetPosX(float _posX) { pos.x = _posX; }
	void SetPosY(float _posY) { pos.y = _posY; }
	void SetPosZ(float _posZ) { pos.z = _posZ; }
	void SetSpeed(DirectX::XMFLOAT3 _speed) { speed = _speed; }
	void SetAccel(DirectX::XMFLOAT3 _accel) { accel = _accel; }
	void SetScale(DirectX::XMFLOAT2 _scale) { scale = _scale; }
	void SetColor(DirectX::XMFLOAT4 _color) { color = _color; }
	void SetAlpha(float _alpha) { color.w = _alpha; }
	void SetAngle(float _angle) { angle = _angle; }
	void SetExist(bool _exist) { exist = _exist; }

	void SetTexPos(DirectX::XMFLOAT2 _texPos) { texPos = _texPos; }
	void SetTexSize(DirectX::XMFLOAT2 _texSize) { texSize = _texSize; }

public:
	void AddPosX(float _addX) { pos.x += _addX; }
	void AddPosY(float _addY) { pos.y += _addY; }
	void AddPosZ(float _addZ) { pos.z += _addZ; }

	void AddSpeedX(float _addX) { speed.x += _addX; }
	void AddSpeedY(float _addY) { speed.y += _addY; }
	void AddSpeedZ(float _addZ) { speed.z += _addZ; }
};

class FlashParticle
{
private:
	struct ParticleImGuiData
	{
		int speed[3];
		//int accel[3];
		int accelStart[3];
		int accelStage[3];
	};

private:
	static const int MAX_SIZE = 10;
	static const int MAX_CNT = 30;

	bool emitting;
	Particle data[MAX_SIZE];
	ParticleImGuiData imguiData;
	DirectX::XMFLOAT3 originPos;	// 放出源の座標
	int cnt;

private:
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive, std::uint32_t version)
	{
		archive
		(
			CEREAL_NVP(imguiData.speed),
			CEREAL_NVP(imguiData.accelStart),
			CEREAL_NVP(imguiData.accelStage)
		);

		if (1 <= version)
		{
			// archive( CEREAL_NVP( x ) );
		}
	}
	static constexpr const char* SERIAL_ID = "Flash";

	void LoadParameter(bool isBinary = true);

#if USE_IMGUI

	void SaveParameter();

public:
	void UseImGui();

#endif // USE_IMGUI

public:
	void Set();
	void Emit();
	void Render();
	void ImGuiDataInit();
	void ImGui();
};
CEREAL_CLASS_VERSION(FlashParticle, 0)

class BubbleParticle
{
private:
	struct ParticleImGuiData
	{
		int speed[3];
		int accel[3];
		int accelStart[3];
		int accelStage[3];
	};

private:
	static const int MAX_SIZE = 10;
	static const int MAX_CNT = 30;

	bool emitting;
	Particle data[MAX_SIZE];
	ParticleImGuiData imguiData;
	DirectX::XMFLOAT3 originPos;	// 放出源の座標
	int cnt;

public:
	void Set();
	void Emit();
	void ImGuiDataInit();
	void ImGui();
};

class FireFryParticle
{
private:
	struct ParticleImGuiData
	{
		int speed[3];
		//int accel[3];
		int accelStart[3];
		int accelStage[3];
	};

private:
	static const int MAX_SIZE = 100;
	static const int MAX_CNT = 180;

	bool emitting;
	Particle data[MAX_SIZE];
	ParticleImGuiData imguiData;
	DirectX::XMFLOAT4 originPos;	// 放出源の座標
	int cnt[MAX_SIZE];

public:
	FireFryParticle() : emitting(false) {}
	~FireFryParticle() {}

private:
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive, std::uint32_t version)
	{
		archive
		(
			CEREAL_NVP(imguiData.speed),
			CEREAL_NVP(imguiData.accelStart),
			CEREAL_NVP(imguiData.accelStage)
		);

		if (1 <= version)
		{
			// archive( CEREAL_NVP( x ) );
		}
	}
	static constexpr const char* SERIAL_ID = "Flash";

	void LoadParameter(bool isBinary = true);

#if USE_IMGUI

	void SaveParameter();

public:
	void UseImGui();

#endif // USE_IMGUI

public:
	void Set();
	void Emit();
	void Draw();
	void ImGuiDataInit();
	void ImGui();

	// 放出位置の設定関数
	void SetOriginPos(DirectX::XMFLOAT4 _originPos) { originPos = _originPos; }

	// パーティクルの最大サイズ
	int GetMaxSize() { return MAX_SIZE; }
};
CEREAL_CLASS_VERSION(FireFryParticle, 0)

class EruptionParticle
{
private:
	struct ParticleImGuiData
	{
		int speed[3];
		//int accel[3];
		int accelStart[3];
		int accelStage[3];
	};

private:
	static const int MAX_SIZE = 25;
	static const int MAX_CNT = 60;

	bool emitting;
	int state;
	Particle data[MAX_SIZE];
	ParticleImGuiData imguiData;
	DirectX::XMFLOAT4 originPos[MAX_SIZE];	// 放出源の座標
	int cnt[MAX_SIZE];
	int totalCnt;

	std::shared_ptr<static_mesh> billboard;

public:
	EruptionParticle() : emitting(false)
	{
		for (int i = 0; i < MAX_SIZE; i++)
		{
			createBillboard(&data[i].pMesh, L"./Data/Images/UI/particle.png");
		}
	}
	~EruptionParticle() {}

private:
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive, std::uint32_t version)
	{
		archive
		(
			CEREAL_NVP(imguiData.speed),
			CEREAL_NVP(imguiData.accelStart),
			CEREAL_NVP(imguiData.accelStage)
		);

		if (1 <= version)
		{
			// archive( CEREAL_NVP( x ) );
		}
	}
	static constexpr const char* SERIAL_ID = "Flash";

	void LoadParameter(bool isBinary = true);

#if USE_IMGUI

	void SaveParameter();

public:
	void UseImGui();

#endif // USE_IMGUI

public:
	void Set(DirectX::XMFLOAT3 _pos);
	void ReSet() { emitting = false; }
	void Emit();
	void Draw();
	void bloom_Draw();
	void ImGuiDataInit();
	void ImGui();

	// 放出位置の設定関数
//	void SetOriginPos(DirectX::XMFLOAT3 _originPos) { originPos = _originPos; }

	// パーティクルの最大サイズ
	int GetMaxSize() { return MAX_SIZE; }
};
CEREAL_CLASS_VERSION(EruptionParticle, 0)

class AbsorptionParticle
{
private:
	struct ParticleImGuiData
	{
		int speed[3];
		//int accel[3];
		int accelStart[3];
		int accelStage[3];
		float radius;
	};

private:
	static const int MAX_SIZE = 1000;
	static const int MAX_CNT = 30;

	bool emitting;
	bool end;
	int state;
	Particle data[MAX_SIZE];
	ParticleImGuiData imguiData;
	DirectX::XMFLOAT4 originPos[MAX_SIZE];	// 放出源の座標
	int cnt[MAX_SIZE];
	int sw;

public:
	AbsorptionParticle() : emitting(false)
	{
		for (int i = 0; i < MAX_SIZE; i++)
		{
			createBillboard(&data[i].pMesh, L"./Data/Images/UI/particle.png");
		}
	}
	~AbsorptionParticle() {}

public:
	void Init();
	void Set(int type, DirectX::XMFLOAT3 _pos);
	void ReSet();
	void Emit();
	void Draw();
	void bloom_Draw();
	void ImGuiDataInit();
	void ImGui();

	// 放出位置の設定関数
//	void SetOriginPos(DirectX::XMFLOAT3 _originPos) { originPos = _originPos; }

	// パーティクルの最大サイズ
	int GetMaxSize() { return MAX_SIZE; }
};

class DustParticle
{
private:
	struct ParticleImGuiData
	{
		int speed[3];
		//int accel[3];
		int accelStart[3];
		int accelStage[3];
	};

private:
	static const int MAX_SIZE = 25;
	static const int MAX_CNT = 60;

	bool emitting;
	int state;
	Particle data[MAX_SIZE];
	ParticleImGuiData imguiData;
	DirectX::XMFLOAT4 originPos[MAX_SIZE];	// 放出源の座標
	int cnt[MAX_SIZE];
	int totalCnt;

	std::shared_ptr<static_mesh> billboard;

public:
	DustParticle() : emitting(false)
	{
		for (int i = 0; i < MAX_SIZE; i++)
		{
			createBillboard(&data[i].pMesh, L"./Data/Images/UI/particle.png");
		}
	}
	~DustParticle() {}

private:
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive, std::uint32_t version)
	{
		archive
		(
			CEREAL_NVP(imguiData.speed),
			CEREAL_NVP(imguiData.accelStart),
			CEREAL_NVP(imguiData.accelStage)
		);

		if (1 <= version)
		{
			// archive( CEREAL_NVP( x ) );
		}
	}
	static constexpr const char* SERIAL_ID = "Flash";

	void LoadParameter(bool isBinary = true);

#if USE_IMGUI

	void SaveParameter();

public:
	void UseImGui();

#endif // USE_IMGUI

public:
	void Set(DirectX::XMFLOAT3 _pos, int stageNum = 0);
	void Emit();
	void Draw();
	void bloom_Draw();
	void ImGuiDataInit();
	void ImGui();

	bool GetEmitting() { return emitting; }

	// 放出位置の設定関数
//	void SetOriginPos(DirectX::XMFLOAT3 _originPos) { originPos = _originPos; }

	// パーティクルの最大サイズ
	int GetMaxSize() { return MAX_SIZE; }
};
CEREAL_CLASS_VERSION(DustParticle, 0)

class SparkParticle
{
private:
	struct ParticleImGuiData
	{
		int speed[3];
		//int accel[3];
		int accelStart[3];
		int accelStage[3];
		float radius;
	};

private:
	static const int MAX_SIZE = 25;
	static const int MAX_CNT = 30;

	bool emitting;
	int state;
	Particle data[MAX_SIZE];
	ParticleImGuiData imguiData;
	DirectX::XMFLOAT4 originPos[MAX_SIZE];		// 放出源の座標
	DirectX::XMFLOAT3 originSpeed[MAX_SIZE];
	int cnt[MAX_SIZE];
	bool alive[MAX_SIZE];
	//	int totalCnt;

	//	std::shared_ptr<static_mesh> billboard;

public:
	SparkParticle() : emitting(false)
	{
		for (int i = 0; i < MAX_SIZE; i++)
		{
			createBillboard(&data[i].pMesh, L"./Data/Images/UI/particle.png");
		}
	}
	~SparkParticle() {}

private:
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive, std::uint32_t version)
	{
		archive
		(
			CEREAL_NVP(imguiData.speed),
			CEREAL_NVP(imguiData.accelStart),
			CEREAL_NVP(imguiData.accelStage),
			CEREAL_NVP(imguiData.radius)
		);

		if (1 <= version)
		{
			// archive( CEREAL_NVP( x ) );
		}
	}
	static constexpr const char* SERIAL_ID = "Absorption";

	void LoadParameter(bool isBinary = true);

#if USE_IMGUI

	void SaveParameter();

public:
	void UseImGui();

#endif // USE_IMGUI

public:
	void Set(DirectX::XMFLOAT3 _pos);
	void Emit();
	void Draw();
	void bloom_Draw();
	void ImGuiDataInit();
	void ImGui();
	bool GetEmitting() { return emitting; }

	// 放出位置の設定関数
//	void SetOriginPos(DirectX::XMFLOAT3 _originPos) { originPos = _originPos; }

	// パーティクルの最大サイズ
	int GetMaxSize() { return MAX_SIZE; }
};
CEREAL_CLASS_VERSION(SparkParticle, 0)

class LocusParticle
{
private:
	struct ParticleImGuiData
	{
		int speed[3];
		//int accel[3];
		int accelStart[3];
		int accelStage[3];
		float radius;
	};

private:
	static const int MAX_SIZE = 25;
	static const int MAX_CNT = 30;

	bool emitting;
	int state;
	Particle data[MAX_SIZE];
	ParticleImGuiData imguiData;
	DirectX::XMFLOAT4 originPos[MAX_SIZE];		// 放出源の座標
	DirectX::XMFLOAT3 originSpeed[MAX_SIZE];
	int cnt[MAX_SIZE];
	bool alive[MAX_SIZE];
	//	int totalCnt;

	//	std::shared_ptr<static_mesh> billboard;

public:
	LocusParticle() : emitting(false)
	{
		for (int i = 0; i < MAX_SIZE; i++)
		{
			createBillboard(&data[i].pMesh, L"./Data/Images/UI/particle.png");
		}
	}
	~LocusParticle() {}

private:
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive, std::uint32_t version)
	{
		archive
		(
			CEREAL_NVP(imguiData.speed),
			CEREAL_NVP(imguiData.accelStart),
			CEREAL_NVP(imguiData.accelStage),
			CEREAL_NVP(imguiData.radius)
		);

		if (1 <= version)
		{
			// archive( CEREAL_NVP( x ) );
		}
	}
	static constexpr const char* SERIAL_ID = "Absorption";

	void LoadParameter(bool isBinary = true);

#if USE_IMGUI

	void SaveParameter();

public:
	void UseImGui();

#endif // USE_IMGUI

public:
	void Set(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _dir);
	void Emit();
	void Draw();
	void bloom_Draw();
	void ImGuiDataInit();
	void ImGui();

	// 放出位置の設定関数
//	void SetOriginPos(DirectX::XMFLOAT3 _originPos) { originPos = _originPos; }

	// パーティクルの最大サイズ
	int GetMaxSize() { return MAX_SIZE; }
};
CEREAL_CLASS_VERSION(LocusParticle, 0)

class AccelParticle
{
private:
	struct ParticleImGuiData
	{
		int speed[3];
		//int accel[3];
		int accelStart[3];
		int accelStage[3];
		float radius;
	};

private:
	static const int MAX_SIZE = 50;
	static const int MAX_CNT = 30;

	bool emitting;
	bool end;
	int state;
	ParticleImGuiData imguiData;
	Particle data[MAX_SIZE];
	DirectX::XMFLOAT4 originPos[MAX_SIZE];		// 放出源の座標
	DirectX::XMFLOAT3 originSpeed[MAX_SIZE];
	int cnt[MAX_SIZE];
	bool alive[MAX_SIZE];

public:
	AccelParticle() : emitting(false)
	{
		for (int i = 0; i < MAX_SIZE; i++)
		{
			createBillboard(&data[i].pMesh, L"./Data/Images/UI/particle.png");
		}
	}
	~AccelParticle() {}

private:
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive, std::uint32_t version)
	{
		archive
		(
			CEREAL_NVP(imguiData.speed),
			CEREAL_NVP(imguiData.accelStart),
			CEREAL_NVP(imguiData.accelStage),
			CEREAL_NVP(imguiData.radius)
		);

		if (1 <= version)
		{
			// archive( CEREAL_NVP( x ) );
		}
	}
	static constexpr const char* SERIAL_ID = "Absorption";

	void LoadParameter(bool isBinary = true);

#if USE_IMGUI

	void SaveParameter();

public:
	void UseImGui();

#endif // USE_IMGUI

public:
	void Set();
	void ReSet();
	void Emit(DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _dir);
	void Draw();
	void bloom_Draw();
	void ImGuiDataInit();
	void ImGui();

	// 放出位置の設定関数
//	void SetOriginPos(DirectX::XMFLOAT3 _originPos) { originPos = _originPos; }

	// パーティクルの最大サイズ
	int GetMaxSize() { return MAX_SIZE; }
};
CEREAL_CLASS_VERSION(AccelParticle, 0)

class StoneBreakParticle
{
private:
	struct ParticleImGuiData
	{
		int speed[3];
		//int accel[3];
		int accelStart[3];
		int accelStage[3];
		float radius;
	};

private:
	static const int MAX_SIZE = 25;
	static const int MAX_CNT = 30;

	bool emitting;
	int state;
	Particle data[MAX_SIZE];
	ParticleImGuiData imguiData;
	DirectX::XMFLOAT4 originPos[MAX_SIZE];		// 放出源の座標
	DirectX::XMFLOAT3 originSpeed[MAX_SIZE];
	int cnt[MAX_SIZE];
	bool alive[MAX_SIZE];

public:
	StoneBreakParticle() : emitting(false)
	{
		for (int i = 0; i < MAX_SIZE; i++)
		{
			createBillboard(&data[i].pMesh, L"./Data/Images/UI/particleC.png");
		}
	}
	~StoneBreakParticle() {}

private:
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive, std::uint32_t version)
	{
		archive
		(
			CEREAL_NVP(imguiData.speed),
			CEREAL_NVP(imguiData.accelStart),
			CEREAL_NVP(imguiData.accelStage),
			CEREAL_NVP(imguiData.radius)
		);

		if (1 <= version)
		{
			// archive( CEREAL_NVP( x ) );
		}
	}
	static constexpr const char* SERIAL_ID = "Absorption";

	void LoadParameter(bool isBinary = true);

#if USE_IMGUI

	void SaveParameter();

public:
	void UseImGui();

#endif // USE_IMGUI

public:
	void Set(DirectX::XMFLOAT3 _pos);
	void Emit();
	void Draw();
	void bloom_Draw();
	void ImGuiDataInit();
	void ImGui();
	bool GetEmitting() { return emitting; }

	// 放出位置の設定関数
//	void SetOriginPos(DirectX::XMFLOAT3 _originPos) { originPos = _originPos; }

	// パーティクルの最大サイズ
	int GetMaxSize() { return MAX_SIZE; }
};
CEREAL_CLASS_VERSION(StoneBreakParticle, 0)

class CatapultBreakParticle
{
private:
	struct ParticleImGuiData
	{
		int speed[3];
		//int accel[3];
		int accelStart[3];
		int accelStage[3];
		float radius;
	};

private:
	static const int MAX_SIZE = 100;
	static const int MAX_CNT = 30;

	bool emitting;
	int state;
	Particle data[MAX_SIZE];
	ParticleImGuiData imguiData;
	DirectX::XMFLOAT4 originPos[MAX_SIZE];		// 放出源の座標
	DirectX::XMFLOAT3 originSpeed[MAX_SIZE];
	int cnt[MAX_SIZE];
	bool alive[MAX_SIZE];

public:
	CatapultBreakParticle() : emitting(false)
	{
		for (int i = 0; i < MAX_SIZE; i++)
		{
			createBillboard(&data[i].pMesh, L"./Data/Images/UI/particleC.png");
		}
	}
	~CatapultBreakParticle() {}

private:
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive, std::uint32_t version)
	{
		archive
		(
			CEREAL_NVP(imguiData.speed),
			CEREAL_NVP(imguiData.accelStart),
			CEREAL_NVP(imguiData.accelStage),
			CEREAL_NVP(imguiData.radius)
		);

		if (1 <= version)
		{
			// archive( CEREAL_NVP( x ) );
		}
	}
	static constexpr const char* SERIAL_ID = "Absorption";

	void LoadParameter(bool isBinary = true);

#if USE_IMGUI

	void SaveParameter();

public:
	void UseImGui();

#endif // USE_IMGUI

public:
	void Set(DirectX::XMFLOAT3 _pos);
	void Emit();
	void Draw();
	void bloom_Draw();
	void ImGuiDataInit();
	void ImGui();
	bool GetEmitting() { return emitting; }

	// 放出位置の設定関数
//	void SetOriginPos(DirectX::XMFLOAT3 _originPos) { originPos = _originPos; }

	// パーティクルの最大サイズ
	int GetMaxSize() { return MAX_SIZE; }
};
CEREAL_CLASS_VERSION(CatapultBreakParticle, 0)

class ShieldParticle
{
private:
	struct ParticleImGuiData
	{
		int speed[3];
		//int accel[3];
		int accelStart[3];
		int accelStage[3];
		float radius;
	};

private:
	static const int MAX_SIZE = 100;
	static const int MAX_CNT = 30;

	bool emitting;
	int state;
	Particle data[MAX_SIZE];
	ParticleImGuiData imguiData;
	DirectX::XMFLOAT4 originPos[MAX_SIZE];		// 放出源の座標
	DirectX::XMFLOAT3 originSpeed[MAX_SIZE];
	DirectX::XMFLOAT2 originScale[MAX_SIZE];
	int cnt;
	bool alive[MAX_SIZE];

public:
	ShieldParticle() : emitting(false)
	{
		for (int i = 0; i < MAX_SIZE; i++)
		{
			createBillboard(&data[i].pMesh, L"./Data/Images/UI/particle.png");
		}
	}
	~ShieldParticle() {}

private:
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive, std::uint32_t version)
	{
		archive
		(
			CEREAL_NVP(imguiData.speed),
			CEREAL_NVP(imguiData.accelStart),
			CEREAL_NVP(imguiData.accelStage),
			CEREAL_NVP(imguiData.radius)
		);

		if (1 <= version)
		{
			// archive( CEREAL_NVP( x ) );
		}
	}
	static constexpr const char* SERIAL_ID = "Absorption";

	void LoadParameter(bool isBinary = true);

#if USE_IMGUI

	void SaveParameter();

public:
	void UseImGui();

#endif // USE_IMGUI

public:
	void Set(DirectX::XMFLOAT3 _pos);
	void Emit();
	void Draw();
	void ImGuiDataInit();
	void ImGui();
	bool GetEmitting() { return emitting; }

	// 放出位置の設定関数
//	void SetOriginPos(DirectX::XMFLOAT3 _originPos) { originPos = _originPos; }

	// パーティクルの最大サイズ
	int GetMaxSize() { return MAX_SIZE; }
};
CEREAL_CLASS_VERSION(ShieldParticle, 0)

class DisappearanceParticle
{
private:
	struct ParticleImGuiData
	{
		int speed[3];
		//int accel[3];
		int accelStart[3];
		int accelStage[3];
		float radius;
	};

private:
	static const int MAX_SIZE = 500;
	static const int MAX_CNT = 30;

	ParticleImGuiData imguiData;
	bool emitting;
	Particle data[MAX_SIZE];
	bool alive[MAX_SIZE];
	float radius[MAX_SIZE];
	float angle[MAX_SIZE];
	int cnt[MAX_CNT];
	Donya::Vector3 originPos;

public:
	DisappearanceParticle() : emitting(false)
	{
		for (int i = 0; i < MAX_SIZE; i++)
		{
			createBillboard(&data[i].pMesh, L"./Data/Images/UI/particle.png");
		}
	}
	~DisappearanceParticle() {}

private:
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive, std::uint32_t version)
	{
		archive
		(
			CEREAL_NVP(imguiData.speed),
			CEREAL_NVP(imguiData.accelStart),
			CEREAL_NVP(imguiData.accelStage),
			CEREAL_NVP(imguiData.radius)
		);

		if (1 <= version)
		{
			// archive( CEREAL_NVP( x ) );
		}
	}
	static constexpr const char* SERIAL_ID = "Absorption";

	void LoadParameter(bool isBinary = true);

#if USE_IMGUI

	void SaveParameter();

public:
	void UseImGui();

#endif // USE_IMGUI

public:
	void Set(DirectX::XMFLOAT3 _pos);
	void Emit();
	void Draw();
	void ImGuiDataInit();
	void ImGui();
	bool GetEmitting() { return emitting; }

	// 放出位置の設定関数
//	void SetOriginPos(DirectX::XMFLOAT3 _originPos) { originPos = _originPos; }

	// パーティクルの最大サイズ
	int GetMaxSize() { return MAX_SIZE; }
};
CEREAL_CLASS_VERSION(DisappearanceParticle, 0)
