#pragma once

#include <memory>
#include <vector>
#include <DirectXMath.h>


class Particle
{
protected:
	DirectX::XMFLOAT3 originPos;	// ���o���̍��W
	DirectX::XMFLOAT3 pos;			// ���W
	DirectX::XMFLOAT3 speed;		// ���x
	DirectX::XMFLOAT3 accel;		// �����x
	DirectX::XMFLOAT4 color;		// �F
	bool exist;						// ���݃t���O
	float scale;					// ���S���W����̔��a

public:
	Particle()
		: originPos( 0.0f, 0.0f, 0.0f ), pos( 0.0f, 0.0f, 0.0f ), speed( 0.0f, 0.0f, 0.0f ),
		accel( 0.0f, 0.0f, 0.0f ), color( 1.0f, 1.0f, 1.0f, 1.0f ), exist( false ), scale( 1.0f )
	{}
	~Particle() {}

	void Init( DirectX::XMFLOAT3 _originPos, DirectX::XMFLOAT3 _speed, DirectX::XMFLOAT3 _accel, float _scale );
	void Update();

public: // Get�֐�
	DirectX::XMFLOAT3 GetOriginPos()	{ return originPos;	}
	DirectX::XMFLOAT3 GetPos()			{ return pos;		}
	DirectX::XMFLOAT3 GetSpeed()		{ return speed;		}
	DirectX::XMFLOAT3 GetAccel()		{ return accel;		}
	DirectX::XMFLOAT4 GetColor()		{ return color;		}
	bool GetExist()						{ return exist;		}
	float GetScale()					{ return scale;		}

public: // Set�֐�
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

};

class FlashParticle
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

	Particle data[MAX_SIZE];
	ParticleImGuiData imguiData;
	int cnt;

public:
	void Set();
	void Emit();
	void ImGuiDataInit();
	void ImGui();
};

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

	Particle data[MAX_SIZE];
	ParticleImGuiData imguiData;
	int cnt;

public:
	void Set();
	void Emit();
	void ImGuiDataInit();
	void ImGui();
};
