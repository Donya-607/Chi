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
	struct SphereFrameWithName
	{
		Donya::SphereFrame sphereF{};
		std::string meshName{};
	private:
		friend class cereal::access;
		template<class Archive>
		void serialize( Archive &archive, std::uint32_t version )
		{
			archive
			(
				CEREAL_NVP( sphereF ),
				CEREAL_NVP( meshName )
			);

			if ( 1 <= version )
			{
				// archive( CEREAL_NVP( x ) );
			}
		}
	public:
		Donya::Sphere CalcTransformedSphere( skinned_mesh *pMesh, const Donya::Vector4x4 &parentSpaceMatrix ) const;
	};
	struct Member
	{
		float				scale{};				// Usually 1.0f.
		float				stageBodyRadius{};		// Using for collision to stage's wall.
		float				targetDistNear{};		// 0.0f ~ 1.0f.
		float				targetDistFar{};		// 0.0f ~ 1.0f.
		float				idleSlerpFactor{};		// 0.0 ~ 1.0. Use when status is idle.
		float				moveMoveSpeed{};		// Use when status is move.
		float				moveSlerpFactor{};		// 0.0 ~ 1.0. Use when status is move.
		int					defeatMotionLength{};	// Per frame. Use when status is defeat(END).
		float				defeatHideSpeed{};		// Use after motionLength.
		float				explMoveSpeed{};		// Use when status is explosion.
		float				explSlerpFactor{};		// 0.0 ~ 1.0. Use when status is explosion.
		float				swingMoveSpeed{};		// Use when status is swing.
		float				swingSlerpFactor{};		// 0.0 ~ 1.0. Use when status is swing.
		float				raidMoveSpeed{};		// Use when status is raid. speed of before jump.
		float				raidSlerpFactor{};		// 0.0 ~ 1.0. Use when status is raid.
		int					raidJumpStartFrame{};	// Use when status is raid.
		int					raidJumpLastFrame{};	// Use when status is raid.
		float				raidJumpDistance{};		// Use when status is raid. whole distance of long-jump's.
		int					raidEaseKind{};			// Use when status is raid.
		int					raidEaseType{};			// Use when status is raid.
		float				explRotationSpeed{};	// Use when status is explosion.
		float				explScaleStart{};		// Use when status is explosion.
		float				explScaleLast{};		// Use when status is explosion.
		float				explScaleDraw{};		// Use when status is explosion.
		int					explScalingFrame{};		// Take time(frame) of scaling.
		int					explChargeFrame{};		// Take time(frame) of charge. use before the explosion and the animation will stop.
		int					explReviveColFrame{};	// Take time(frame) of revive the collision of explosion, since hit to anything.
		float				explHideSpeed{};		// Use when status is explosion.
		Donya::Vector3		initPos{};				// The index(stage number) is 1-based. 0 is tutorial.
		Donya::Vector3		drawOffset{};			// The index(stage number) is 1-based. 0 is tutorial.
		Donya::Sphere		hitBoxBody{};			// Body's hit boxes.
		Donya::SphereFrame	hitBoxExpl{};			// Attack of explosion hit box.
		SphereFrameWithName	hitBoxSwing{};			// Attack of swing hit box.
		SphereFrameWithName	hitBoxRaid{};			// Attack of raid hit box.
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
			archive( CEREAL_NVP( m.hitBoxSwing ) );
		}
		if ( 2 <= version )
		{
			archive( CEREAL_NVP( m.hitBoxRaid ) );
		}
		if ( 3 <= version )
		{
			archive
			(
				CEREAL_NVP( m.explRotationSpeed ),
				CEREAL_NVP( m.explScaleStart ),
				CEREAL_NVP( m.explScaleLast ),
				CEREAL_NVP( m.explScalingFrame ),
				CEREAL_NVP( m.explChargeFrame ),
				CEREAL_NVP( m.explReviveColFrame ),
				CEREAL_NVP( m.hitBoxExpl )
			);
		}
		if ( 4 <= version )
		{
			archive( CEREAL_NVP( m.explScaleDraw ) );
		}
		if ( 5 <= version )
		{
			archive( CEREAL_NVP( m.explHideSpeed ) );
		}
		if ( 6 <= version )
		{
			archive
			(
				CEREAL_NVP( m.raidJumpStartFrame ),
				CEREAL_NVP( m.raidJumpLastFrame ),
				CEREAL_NVP( m.raidJumpDistance ),
				CEREAL_NVP( m.raidEaseKind ),
				CEREAL_NVP( m.raidEaseType )
			);
		}
		if ( 7 <= version )
		{
			archive
			(
				CEREAL_NVP( m.defeatMotionLength ),
				CEREAL_NVP( m.defeatHideSpeed )
			);
		}
		if ( 8 <= version )
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

	Donya::SphereFrame			&HitBoxExplosion()			{ return m.hitBoxExpl; }
	const Donya::SphereFrame	&HitBoxExplosion()	const	{ return m.hitBoxExpl; }
	SphereFrameWithName			&HitBoxSwing()				{ return m.hitBoxSwing; }
	const SphereFrameWithName	&HitBoxSwing()		const	{ return m.hitBoxSwing; }
	SphereFrameWithName			&HitBoxRaid()				{ return m.hitBoxRaid; }
	const SphereFrameWithName	&HitBoxRaid()		const	{ return m.hitBoxRaid; }
public:
	void LoadParameter( bool isBinary = true );

#if USE_IMGUI

	void SaveParameter();

	void UseImGui();

#endif // USE_IMGUI
};
CEREAL_CLASS_VERSION( KnightParam, 7 )

struct fbx_shader; // Use for argument.
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
		std::shared_ptr<skinned_mesh> pIdle{ nullptr };
		std::shared_ptr<skinned_mesh> pDefeat{ nullptr };
		std::shared_ptr<skinned_mesh> pRunFront{ nullptr };
		std::shared_ptr<skinned_mesh> pRunLeft{ nullptr };
		std::shared_ptr<skinned_mesh> pRunRight{ nullptr };
		std::shared_ptr<skinned_mesh> pRunBack{ nullptr };
		std::shared_ptr<skinned_mesh> pAtkExpl{ nullptr };
		std::shared_ptr<skinned_mesh> pAtkSwing{ nullptr };
		std::shared_ptr<skinned_mesh> pAtkRaid{ nullptr };
		std::shared_ptr<skinned_mesh> pFxExpl{ nullptr };
	};
private:
	KnightAI::ActionState	status;
	KnightAI				AI;
	int						timer;				// Recycle between each state.
	int						reviveCollisionTime;// Use when disable collision.
	float					moveSign;			// Use when aim-move state. store destination sign(-1:left, +1:right).
	float					fieldRadius;		// For collision to wall. the field is perfect-circle, so I can detect collide to wall by distance.
	float					slerpFactor;		// 0.0f ~ 1.0f. Use orientation's rotation.
	Donya::Vector3			pos;				// Contain world space position. actual position is "pos + extraOffset".
	Donya::Vector3			velocity;
	Donya::Vector3			extraOffset;		// Actual position is "pos + extraOffset".
	Donya::Quaternion		orientation;
	Models					models;
public:
	Knight();
	~Knight();
public:
	void Init( int stageNo );
	void Uninit();

	void Update( TargetStatus target );

	void Draw( fbx_shader &HLSL, const Donya::Vector4x4 &matView, const Donya::Vector4x4 &matProjection, float animationAcceleration = 1.0f );
public:
	bool IsCollideAttackHitBoxes( const Donya::AABB   judgeOther, bool disableCollidingHitBoxes );
	bool IsCollideAttackHitBoxes( const Donya::OBB    judgeOther, bool disableCollidingHitBoxes );

	/// <summary>
	/// Returns world space hit-box of body.
	/// </summary>
	Donya::Sphere GetBodyHitBox() const;
	/// <summary>
	/// Returns world space hit-box of attack of explosion.
	/// </summary>
	Donya::Sphere CalcAttackHitBoxExplosion() const;
	/// <summary>
	/// Returns world space hit-box of attack of swing.
	/// </summary>
	Donya::Sphere CalcAttackHitBoxSwing() const;
	/// <summary>
	/// Returns world space hit-box of attack of raid.
	/// </summary>
	Donya::Sphere CalcAttackHitBoxRaid() const;

	void ReceiveImpact();

	void SetFieldRadius( float fieldRadius );

	Donya::Vector3 GetPos() const { return pos + extraOffset; }

	/// <summary>
	/// Returns true when the status is "Defeat" and the defeat motion was finished completely.
	/// </summary>
	bool IsDefeated() const;
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

	void MoveInit( TargetStatus target, KnightAI::ActionState statusDetail );
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

	void DefeatInit();
	void DefeatUpdate();
	void DefeatUninit();
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

	void UseImGui( float normalizedTargetDistance );

#endif // USE_IMGUI
};
