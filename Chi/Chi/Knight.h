#pragma once

#include <memory>
#include <vector>

#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>

#include "Donya/Collision.h"
#include "Donya/Quaternion.h"
#include "Donya/Serializer.h"
#include "Donya/Template.h"
#include "Donya/UseImGui.h"
#include "Donya/Vector.h"

#include "KnightAI.h"

class skinned_mesh;	// With pointer. because I'm not want include this at header.

/// <summary>
/// Knight's serialize parameter.
/// </summary>
class KnightParam final : public Donya::Singleton<KnightParam>
{
	friend Donya::Singleton<KnightParam>;
public:
	struct Member
	{
		float			scale{};			// Usually 1.0f.
		float			stageBodyRadius{};	// Using for collision to stage's wall.
		float			targetDistNear{};	// 0.0f ~ 1.0f.
		float			targetDistFar{};	// 0.0f ~ 1.0f.
		float			idleSlerpFactor{};	// 0.0 ~ 1.0. Use when status is idle.
		float			moveMoveSpeed{};	// Use when status is move.
		float			moveSlerpFactor{};	// 0.0 ~ 1.0. Use when status is move.
		float			explMoveSpeed{};	// Use when status is explosion.
		float			explSlerpFactor{};	// 0.0 ~ 1.0. Use when status is explosion.
		float			swingMoveSpeed{};	// Use when status is swing.
		float			swingSlerpFactor{};	// 0.0 ~ 1.0. Use when status is swing.
		float			raidMoveSpeed{};	// Use when status is raid.
		float			raidSlerpFactor{};	// 0.0 ~ 1.0. Use when status is raid.
		Donya::Vector3	initPos{};			// The index(stage number) is 1-based. 0 is tutorial.
		Donya::Vector3	drawOffset{};		// The index(stage number) is 1-based. 0 is tutorial.
		Donya::Sphere	hitBoxBody{};		// Body's hit boxes.
	};
private:
	Member m{};
private:
	KnightParam();
public:
	~KnightParam();
private:
	friend class cereal::access;
	template<class Archive>
	void serialize( Archive &archive, std::uint32_t version )
	{
		archive
		(
			CEREAL_NVP( m.scale ),
			CEREAL_NVP( m.stageBodyRadius ),
			CEREAL_NVP( m.targetDistNear ),
			CEREAL_NVP( m.targetDistFar ),
			CEREAL_NVP( m.idleSlerpFactor ),
			CEREAL_NVP( m.moveMoveSpeed ),
			CEREAL_NVP( m.moveSlerpFactor ),
			CEREAL_NVP( m.explMoveSpeed ),
			CEREAL_NVP( m.explSlerpFactor ),
			CEREAL_NVP( m.swingMoveSpeed ),
			CEREAL_NVP( m.swingSlerpFactor ),
			CEREAL_NVP( m.raidMoveSpeed ),
			CEREAL_NVP( m.raidSlerpFactor ),
			CEREAL_NVP( m.initPos ),
			CEREAL_NVP( m.drawOffset ),
			CEREAL_NVP( m.hitBoxBody )
		);

		if ( 1 <= version )
		{
			// archive( CEREAL_NVP( x ) );
		}
	}
	static constexpr const char *SERIAL_ID = "Knight";
public:
	void Init();
	void Uninit();
public:
	float SlerpFactor( KnightAI::ActionState status );
	float MoveSpeed( KnightAI::ActionState status );
	Member Content() const { return m; }
	static Member Open();
public:
	void LoadParameter( bool isBinary = true );

#if USE_IMGUI

	void SaveParameter();

	void UseImGui();

#endif // USE_IMGUI
};
CEREAL_CLASS_VERSION( KnightParam, 0 )

/// <summary>
/// Knight number 3.
/// </summary>
class Knight
{
public:
	struct TargetStatus
	{
		Donya::Vector3 pos{}; // World space.
	};
private:
	struct Models
	{
		std::unique_ptr<skinned_mesh> pIdle{ nullptr };
		std::unique_ptr<skinned_mesh> pRunFront{ nullptr };
		std::unique_ptr<skinned_mesh> pAtkExpl{ nullptr };
		std::unique_ptr<skinned_mesh> pAtkSwing{ nullptr };
		std::unique_ptr<skinned_mesh> pAtkRaid{ nullptr };
	};
private:
	KnightAI::ActionState	status;
	KnightAI				AI;
	int						timer;			// Recycle between each state.
	float					fieldRadius;	// For collision to wall. the field is perfect-circle, so I can detect collide to wall by distance.
	float					slerpFactor;	// 0.0f ~ 1.0f. Use orientation's rotation.
	Donya::Vector3			pos;			// Contain world space position. actual position is "pos + extraOffset".
	Donya::Vector3			velocity;
	Donya::Vector3			extraOffset;	// Actual position is "pos + extraOffset".
	Donya::Quaternion		orientation;
	Models					models;
public:
	Knight();
	~Knight();
public:
	void Init( int stageNo );
	void Uninit();

	void Update( TargetStatus target );

	void Draw( const Donya::Vector4x4 &matView, const Donya::Vector4x4 &matProjection );
public:
	Donya::Sphere	GetBodyHitBoxes() const;

	void ReceiveImpact();

	void SetFieldRadius( float fieldRadius );

	Donya::Vector3 GetPos() const { return pos + extraOffset; }
private:
	void LoadModel();

	float CalcNormalizedDistance( Donya::Vector3 wsTargetPos );

	Donya::Vector4x4 CalcWorldMatrix() const;
private:
	void ChangeStatus( TargetStatus target );
	void UpdateCurrentStatus( TargetStatus target );

	void WaitInit( TargetStatus target );
	void WaitUpdate( TargetStatus target );
	void WaitUninit();

	void MoveInit( TargetStatus target );
	void MoveUpdate( TargetStatus target );
	void MoveUninit();

	void AttackExplosionInit( TargetStatus target );
	void AttackExplosionUpdate( TargetStatus target );
	void AttackExplosionUninit();

	void AttackSwingInit( TargetStatus target );
	void AttackSwingUpdate( TargetStatus target );
	void AttackSwingUninit();

	void AttackRaidInit( TargetStatus target );
	void AttackRaidUpdate( TargetStatus target );
	void AttackRaidUninit();
private:
	/// <summary>
	/// The position("pos") is only changed by this method(or CollideToWall method).
	/// </summary>
	void ApplyVelocity( TargetStatus targert );
	/// <summary>
	/// If the position("pos") without to field range, clamp to field range.
	/// </summary>
	void CollideToWall();
private:
#if USE_IMGUI

	void UseImGui();

#endif // USE_IMGUI
};
