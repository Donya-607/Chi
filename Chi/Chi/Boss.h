#pragma once

#include <memory>
#include <vector>

#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>

#include "Donya/Collision.h"
#include "Donya/Quaternion.h"
#include "Donya/Serializer.h"
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
	std::vector<Donya::OBBFrame> OBBAttacksFast;
	std::vector<Donya::OBBFrame> OBBAttacksSwing;
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

		if ( 1 == version )
		{
			archive
			( 
				CEREAL_NVP( OBBAttacksFast ),
				CEREAL_NVP( OBBAttacksSwing )
			);
		}
		
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
	float								Scale()			const { return scale; }
	std::vector<Donya::OBBFrame>		*OBBAtksFast()  { return &OBBAttacksFast; }
	std::vector<Donya::OBBFrame>		*OBBAtksSwing() { return &OBBAttacksSwing; }
	const std::vector<Donya::OBBFrame>	*OBBAtksFast()	const { return &OBBAttacksFast; }
	const std::vector<Donya::OBBFrame>	*OBBAtksSwing()	const { return &OBBAttacksSwing; }
public:
	void LoadParameter( bool isBinary = true );

#if USE_IMGUI

	void SaveParameter();

	void UseImGui();

#endif // USE_IMGUI
};
CEREAL_CLASS_VERSION( BossParam, 1 )

class skinned_mesh;	// With pointer. because I'm not want include this at header.
struct fbx_shader;
class Boss
{
private:
	struct Models
	{
		std::unique_ptr<skinned_mesh> pIdle{ nullptr };
		std::unique_ptr<skinned_mesh> pAtkFast{ nullptr };
		std::unique_ptr<skinned_mesh> pAtkSwing{ nullptr };
	};
private:
	BossAI::ActionStateNum	status;
	BossAI					AI;
	int						stageNo; // 1-based.
	Donya::Vector3			pos;
	Donya::Vector3			velocity;
	Donya::Quaternion		orientation;
	Models					models;
public:
	Boss();
	~Boss();
public:
	void Init( int stageNo );
	void Uninit();

	void Update();

	void Draw(fbx_shader& hlsl, const Donya::Vector4x4 &matView, const Donya::Vector4x4 &matProjection );
public:
	std::vector<Donya::OBB> GetAttackHitBoxes() const;
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
