#pragma once

#include <memory>
#include <vector>

#include "Donya/Collision.h"
#include "Donya/Quaternion.h"
#include "Donya/Template.h"
#include "Donya/UseImGui.h"
#include "Donya/Vector.h"

#include "BossAI.h"

/// <summary>
/// Boss's serialize parameter.
/// </summary>
class BossParam final : public Donya::Singleton<BossParam>
{
	friend Donya::Singleton<BossParam>;
private:
	float	scale;	// Usually 1.0f.
private:
	BossParam();
public:
	~BossParam();
private:
	friend class cereal::access;
	template<class Archive>
	void serialize( Archive &archive, std::uint32_t version )
	{
		archive
		(
			CEREAL_NVP( scale )
		);

		if ( 1 <= version )
		{
			// archive( CEREAL_NVP( x ) );
		}
	}
	static constexpr const char *SERIAL_ID = "Boss";
public:
	void Init();
	void Uninit();
public:
	float	Scale()					const { return scale; }
public:
	void LoadParameter( bool isBinary = true );

#if USE_IMGUI

	void SaveParameter();

	void UseImGui();

#endif // USE_IMGUI
};
CEREAL_CLASS_VERSION( BossParam, 0 )

class skinned_mesh;	// With pointer. because I'm not want include this at header.
class Boss
{
private:
	struct Models
	{
		std::unique_ptr<skinned_mesh> pIdle;
		std::unique_ptr<skinned_mesh> pAtkFast;
		std::unique_ptr<skinned_mesh> pAtkSwing;
	};
private:
	BossAI::ActionStateNum	status;
	BossAI					AI;
	Donya::Vector3			pos;
	Donya::Vector3			velocity;
	Donya::Quaternion		orientation;
	Models					models;
public:
	Boss();
	~Boss();
public:
	void Init();
	void Uninit();

	void Update();

	void Draw( const Donya::Vector4x4 &matView, const Donya::Vector4x4 &matProjection );
private:
	void LoadModel();
private:
	void ChangeStatus();
	void UpdateCurrentStatus();

	void WaitInit();
	void WaitUpdate();
	void WaitUninit();

	void MoveInit();
	void MoveUpdate();
	void MoveUninit();

	void AttackInit();
	void AttackUpdate();
	void AttackUninit();
private:
	void AssignInputVelocity();

	/// <summary>
	/// The position("pos") is only changed by this method(or CollideToWall method).
	/// </summary>
	void ApplyVelocity();
	/// <summary>
	/// If the position("pos") without to field range, clamp to field range.
	/// </summary>
	void CollideToWall();
private:
#if USE_IMGUI

	void UseImGui();

#endif // USE_IMGUI
};
