#pragma once

#include <memory>

// For serialize "std::string".
#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>

#include "Donya/Collision.h"
#include "Donya/Quaternion.h"
#include "Donya/Serializer.h"
#include "Donya/Template.h"
#include "Donya/UseImGui.h"
#include "Donya/Vector.h"

/// <summary>
/// Player's serialize parameter.
/// </summary>
class PlayerParam final : public Donya::Singleton<PlayerParam>
{
	friend Donya::Singleton<PlayerParam>;
private:
	int		frameUnfoldableDefence;	// 1 ~ N. Use when State::Defend.
	int		shieldsRecastFrame;		// Frame of reuse shield.
	int		frameStopAnime;			// 1 ~ N. Frame of stopping animation of attack.Use when State::Attack.
	int		frameStopTiming;		// 0 ~ N. Timing of stopping animation of attack.Use when State::Attack.
	int		frameCancelableAttack;	// 1 ~ "frameWholeAttacking". this frame is irrelevant by "frameStopAnime". Use when State::Attack.
	int		frameWholeAttacking;	// 1 ~ N. this whole frame is irrelevant by "frameCancelableAttack". Use when State::Attack.
	int		advanceStartFrame;		// 1 ~ N.
	int		advanceFinFrame;		// 1 ~ N. this frame is same as start frame of return.
	int		returnFinFrame;			// 1 ~ "frameWholeAttacking".
	int		advanceEaseKind;
	int		advanceEaseType;
	int		returnEaseKind;
	int		returnEaseType;
	float	advanceDistance;		// Use when State::Attack.
	float	scale;					// Usually 1.0f.
	float	runSpeed;				// Scalar.
	float	rotSlerpFactor;			// Use player's rotation.
	Donya::AABB		hitBoxBody;		// HitBox that collide to boss attacks.
	Donya::Sphere	hitBoxPhysic;	// HitBox that collide to stage.
	Donya::AABB		hitBoxShield;	// HitBox of shield.
	Donya::OBBFrame	hitBoxLance;	// HitBox of lance.
	std::string		lanceMeshName;	// Use for find a mesh that transform the OBB of lance.
private:
	PlayerParam();
public:
	~PlayerParam();
private:
	friend class cereal::access;
	template<class Archive>
	void serialize( Archive &archive, std::uint32_t version )
	{
		archive
		(
			CEREAL_NVP( scale ),
			CEREAL_NVP( runSpeed ),
			CEREAL_NVP( rotSlerpFactor )
		);
		
		if ( 1 <= version )
		{
			archive
			(
				CEREAL_NVP( hitBoxBody ),
				CEREAL_NVP( hitBoxPhysic )
			);
		}
		if ( 2 <= version )
		{
			archive
			(
				CEREAL_NVP( frameUnfoldableDefence ),
				CEREAL_NVP( frameCancelableAttack ),
				CEREAL_NVP( frameWholeAttacking ),
				CEREAL_NVP( hitBoxShield )
			);
		}
		if ( 3 <= version )
		{
			archive( CEREAL_NVP( hitBoxLance ) );
		}
		if ( 4 <= version )
		{
			archive( CEREAL_NVP( lanceMeshName ) );
		}
		if ( 5 <= version )
		{
			archive( CEREAL_NVP( shieldsRecastFrame ) );
		}
		if ( 6 <= version )
		{
			archive
			(
				CEREAL_NVP( frameStopAnime ),
				CEREAL_NVP( frameStopTiming )
			);
		}
		if ( 7 <= version )
		{
			archive
			(
				CEREAL_NVP( advanceStartFrame ),
				CEREAL_NVP( advanceFinFrame ),
				CEREAL_NVP( returnFinFrame ),
				CEREAL_NVP( advanceEaseKind ),
				CEREAL_NVP( advanceEaseType ),
				CEREAL_NVP( returnEaseKind ),
				CEREAL_NVP( returnEaseType )
			);
		}
		if ( 8 <= version )
		{
			// archive( CEREAL_NVP( x ) );
		}
	}
	static constexpr const char *SERIAL_ID = "Player";
public:
	void Init();
	void Uninit();
public:
	int		FrameWholeDefence()		const { return frameUnfoldableDefence; }
	int		FrameReuseShield()		const { return shieldsRecastFrame; }
	int		FrameStopAnimeLength()	const { return frameStopAnime; }
	int		FrameStopAnimeTiming()	const { return frameStopTiming; }
	int		FrameCancelableAttack()	const { return frameCancelableAttack; }
	int		FrameWholeAttacking()	const { return frameWholeAttacking; }
	int		FrameAdvanceStart()		const { return advanceStartFrame; }
	int		FrameAdvanceFin()		const { return advanceFinFrame; }
	int		FrameReturnFin()		const { return returnFinFrame; }
	int		AdvanceEaseKind()		const { return advanceEaseKind; }
	int		AdvanceEaseType()		const { return advanceEaseType; }
	int		ReturnEaseKind()		const { return returnEaseKind; }
	int		ReturnEaseType()		const { return returnEaseType; }
	float	AdvanceDistance()		const { return advanceDistance; }
	float	Scale()					const { return scale; }
	float	RunSpeed()				const { return runSpeed; }
	float	SlerpFactor()			const { return rotSlerpFactor; }
	Donya::AABB		HitBoxBody()	const { return hitBoxBody;   }
	Donya::Sphere	HitBoxPhysic()	const { return hitBoxPhysic; }
	Donya::AABB		HitBoxShield()	const { return hitBoxShield; }
	Donya::OBBFrame			*HitBoxAttackF()			{ return &hitBoxLance;  }
	const Donya::OBBFrame	*HitBoxAttackF()	const	{ return &hitBoxLance;  }
	std::string LanceMeshName()		const { return lanceMeshName; }
public:
	void LoadParameter( bool isBinary = true );

#if USE_IMGUI

	void SaveParameter();

	void UseImGui();

#endif // USE_IMGUI
};
CEREAL_CLASS_VERSION( PlayerParam, 7 )

class  skinned_mesh;	// With pointer. because I'm not want include this at header.
struct fbx_shader;		// Use for argument.
class Player
{
public:
	struct Input
	{
		Donya::Vector3 moveVector; // Normalized.
		bool doDefend;
		bool doAttack;
	public:
		Input() :
			moveVector(),
			doDefend( false ),
			doAttack( false )
		{}
		Input( Donya::Vector3 moveVector, bool doDefend, bool doAttack ) :
			moveVector( moveVector ),
			doDefend( doDefend ),
			doAttack( doAttack )
		{}
	public:
		static Input NoOperation()
		{
			return Input{ Donya::Vector3::Zero(), false, false };
		}
		static Input MakeByExternalInput( Donya::Vector4x4 viewMat );
	};
private:
	struct Models
	{
		std::shared_ptr<skinned_mesh> pIdle{ nullptr };
		std::shared_ptr<skinned_mesh> pRun{ nullptr };
		std::shared_ptr<skinned_mesh> pDefend{ nullptr };
		std::shared_ptr<skinned_mesh> pAttack{ nullptr };
	};
	enum class State
	{
		Idle,
		Run,
		Defend,
		Attack,
		Dead,
	};
private:
	State					status;
	int						timer;				// Recycle between each state.
	int						shieldsRecastTime;	// I can defend when this time is zero.
	float					fieldRadius;		// For collision to wall. the field is perfect-circle, so I can detect collide to wall by distance.
	Donya::Vector3			pos;				// In world space.
	Donya::Vector3			velocity;			// In world space.
	Donya::Vector3			lookDirection;		// In world space.
	Donya::Vector3			extraOffset;		// Actual position is "pos + extraOffset".
	Donya::Quaternion		orientation;
	Models					models;
	std::vector<Donya::Box>	wallCollisions;		// Stored hit-boxes belongs world space, XZ plane. This variable only assigned at Init().
	bool				wasSucceededDefence;
public:
	Player();
	~Player();
public:
	void Init( const Donya::Vector3 &wsInitialPos, const Donya::Vector3 &initialAnglesRadian, const std::vector<Donya::Box> &wsXZWallCollisions );
	void Uninit();

	void Update( Input input );

	void Draw( fbx_shader &HLSL, const Donya::Vector4x4 &matView, const Donya::Vector4x4 &matProjection );
public:
	/// <summary>
	/// Returns world-space position.
	/// </summary>
	Donya::Vector3 GetPosition() const { return pos + extraOffset; }

	/// <summary>
	/// Returns world space hurt-box.
	/// </summary>
	Donya::OBB GetHurtBox() const;
	/// <summary>
	/// Returns world space hit-box of shield.
	/// </summary>
	Donya::OBB GetShieldHitBox() const;
	/// <summary>
	/// Returns world space hit-box of attack.
	/// </summary>
	Donya::OBB CalcAttackHitBox() const;

	/// <summary>
	/// Please call when succeeded defence by enemy's attack.
	/// </summary>
	void SucceededDefence();

	void ReceiveImpact();

	void SetFieldRadius( float fieldRadius );

#if DEBUG_MODE

	void ReplaceWallCollisions( const std::vector<Donya::Box> &wsXZWallCollisions )
	{
		wallCollisions = wsXZWallCollisions;
	}

#endif // DEBUG_MODE
private:
	void LoadModel();

	/// <summary>
	/// Returns matrix transform to world space from local space.
	/// </summary>
	Donya::Vector4x4 CalcWorldMatrix() const;
private:
	void ChangeStatus( Input input );
	void UpdateCurrentStatus( Input input );

	void ChangeStatusFromIdle( Input input );
	void IdleInit( Input input );
	void IdleUpdate( Input input );
	void IdleUninit();

	void ChangeStatusFromRun( Input input );
	void RunInit( Input input );
	void RunUpdate( Input input );
	void RunUninit();

	void ChangeStatusFromDefend( Input input );
	void DefendInit( Input input );
	void DefendUpdate( Input input );
	void DefendUninit();

	void ChangeStatusFromAttack( Input input );
	void AttackInit( Input input );
	void AttackUpdate( Input input );
	void AttackUninit();
private:
	void AssignInputVelocity( Input input );

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
