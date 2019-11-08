#pragma once

#include <memory>
#include <vector>
#include <DirectXMath.h>

#include "skinned_mesh.h"
#include "Donya/Serializer.h"
#include "Donya/UseImGui.h"


class Particle
{
protected:
	DirectX::XMFLOAT3 pos;			// 座標
	DirectX::XMFLOAT3 speed;		// 速度
	DirectX::XMFLOAT3 accel;		// 加速度
	DirectX::XMFLOAT4 color;		// 色
	bool exist;						// 存在フラグ
	float scale;					// 中心座標からの半径

public:
	Particle()
		: pos( 0.0f, 0.0f, 0.0f ), speed( 0.0f, 0.0f, 0.0f ),
		accel( 0.0f, 0.0f, 0.0f ), color( 1.0f, 1.0f, 1.0f, 1.0f ), exist( false ), scale( 1.0f )
	{}
	~Particle() {}

	void Init( DirectX::XMFLOAT3 _pos, DirectX::XMFLOAT3 _speed, DirectX::XMFLOAT3 _accel, float _scale, bool _exist = true );
	void Update();

public: // Get関数
	DirectX::XMFLOAT3 GetPos()			{ return pos;		}
	DirectX::XMFLOAT3 GetSpeed()		{ return speed;		}
	DirectX::XMFLOAT3 GetAccel()		{ return accel;		}
	DirectX::XMFLOAT4 GetColor()		{ return color;		}
	bool GetExist()						{ return exist;		}
	float GetScale()					{ return scale;		}

public: // Set関数
	void SetPos( DirectX::XMFLOAT3 _pos )		{ pos = _pos;		}
	void SetPosX( float _posX )					{ pos.x = _posX;	}
	void SetPosY( float _posY )					{ pos.y = _posY;	}
	void SetPosZ( float _posZ )					{ pos.z = _posZ;	}
	void SetSpeed( DirectX::XMFLOAT3 _speed )	{ speed = _speed;	}
	void SetAccel( DirectX::XMFLOAT3 _accel )	{ accel = _accel;	}
	void SetColor( DirectX::XMFLOAT4 _color )	{ color = _color;	}
	void SetAlpha( float _alpha )				{ color.w = _alpha;	}
	void SetExist( bool _exist )				{ exist = _exist;	}
	void SetScale( float _scale )				{ scale = _scale;	}

public:
	void AddPosX ( float _addX ) { pos.x += _addX; }
	void AddPosY ( float _addY ) { pos.y += _addY; }
	void AddPosZ ( float _addZ ) { pos.z += _addZ; }

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
	DirectX::XMFLOAT3 originPos;	// 放出源の座標
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
	void SetOriginPos(DirectX::XMFLOAT3 _originPos) { originPos = _originPos; }

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
	static const int MAX_SIZE = 100;
	static const int MAX_CNT = 180;

	bool emitting;
	int state;
	Particle data[MAX_SIZE];
	ParticleImGuiData imguiData;
	DirectX::XMFLOAT3 originPos[MAX_SIZE];	// 放出源の座標
	int cnt[MAX_SIZE];
	int totalCnt;

public:
	EruptionParticle() : emitting(false) {}
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
	void Emit();
//	void Draw();
	void ImGuiDataInit();
	void ImGui();

	// 放出位置の設定関数
//	void SetOriginPos(DirectX::XMFLOAT3 _originPos) { originPos = _originPos; }

	// パーティクルの最大サイズ
	int GetMaxSize() { return MAX_SIZE; }
};
CEREAL_CLASS_VERSION(EruptionParticle, 0)