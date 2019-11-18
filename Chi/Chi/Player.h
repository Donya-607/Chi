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

#include "Shield.h"

/// <summary>
/// Player's serialize parameter.
/// </summary>
class PlayerParam final : public Donya::Singleton<PlayerParam>
{
	friend Donya::Singleton<PlayerParam>;
private:
	float				FXIntervalFrame{};		// Interval of generate the FX and SFX.
	float				frameUnfoldableDefence;	// 1 ~ N. Use when State::Defend.
	float				shieldsRecastFrame;		// Frame of reuse shield.
	float				frameStopAnime;			// 1 ~ N. Frame of stopping animation of attack.Use when State::Attack.
	float				frameStopTiming;		// 0 ~ N. Timing of stopping animation of attack.Use when State::Attack.
	float				frameCancelableAttack;	// 1 ~ "frameWholeAttacking". this frame is irrelevant by "frameStopAnime". Use when State::Attack.
	float				frameWholeAttacking;	// 1 ~ N. this whole frame is irrelevant by "frameCancelableAttack". Use when State::Attack.
	float				advanceStartFrame;		// 1 ~ N.
	float				advanceFinFrame;		// 1 ~ N. this frame is same as start frame of return.
	float				returnFinFrame;			// 1 ~ "frameWholeAttacking".
	int					advanceEaseKind;
	int					advanceEaseType;
	int					returnEaseKind;
	int					returnEaseType;
	float				advanceDistance;		// Use when State::Attack.
	float				scale;					// Usually 1.0f.
	float				runSpeed;				// Scalar.
	float				rotSlerpFactor;			// Use player's rotation.
	Donya::AABB			hitBoxBody;				// HitBox that collide to boss attacks.
	Donya::Sphere		hitBoxPhysic;			// HitBox that collide to stage.
	Donya::AABB			hitBoxShield;			// HitBox of shield.
	Donya::OBBFrame		hitBoxLance;			// HitBox of lance.
	std::string			lanceMeshName;			// Use for find a mesh that transform the OBB of lance.
	std::vector<float>	motionSpeeds;			// Magnification.
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
			archive( CEREAL_NVP( advanceDistance ) );
		}
		if ( 9 <= version )
		{
			archive( CEREAL_NVP( motionSpeeds ) );
		}
		if ( 10 <= version )
		{
			archive( CEREAL_NVP( FXIntervalFrame ) );
		}
		if ( 11 <= version )
		{
			// archive( CEREAL_NVP( x ) );
		}
	}
	static constexpr const char *SERIAL_ID = "Player";
public:
	void Init( size_t motionCount  );
	void Uninit();
public:
	float			FXGenerateInterval()	const { return FXIntervalFrame; }
	float			FrameWholeDefence()		const { return frameUnfoldableDefence; }
	float			FrameReuseShield()		const { return shieldsRecastFrame; }
	float			FrameStopAnimeLength()	const { return frameStopAnime; }
	float			FrameStopAnimeTiming()	const { return frameStopTiming; }
	float			FrameCancelableAttack()	const { return frameCancelableAttack; }
	float			FrameWholeAttacking()	const { return frameWholeAttacking; }
	float			FrameAdvanceStart()		const { return advanceStartFrame; }
	float			FrameAdvanceFin()		const { return advanceFinFrame; }
	float			FrameReturnFin()		const { return returnFinFrame; }
	int				AdvanceEaseKind()		const { return advanceEaseKind; }
	int				AdvanceEaseType()		const { return advanceEaseType; }
	int				ReturnEaseKind()		const { return returnEaseKind; }
	int				ReturnEaseType()		const { return returnEaseType; }
	float			AdvanceDistance()		const { return advanceDistance; }
	float			Scale()					const { return scale; }
	float			RunSpeed()				const { return runSpeed; }
	float			SlerpFactor()			const { return rotSlerpFactor; }
	Donya::AABB		HitBoxBody()			const { return hitBoxBody;   }
	Donya::Sphere	HitBoxPhysic()			const { return hitBoxPhysic; }
	Donya::AABB		HitBoxShield()			const { return hitBoxShield; }
	Donya::OBBFrame				*HitBoxAttackF()			{ return &hitBoxLance;  }
	const Donya::OBBFrame		*HitBoxAttackF()	const	{ return &hitBoxLance;  }
	std::string LanceMeshName()				const { return lanceMeshName; }
	std::vector<float>			*MotionSpeeds()				{ return &motionSpeeds; }
	const std::vector<float>	*MotionSpeeds()		const	{ return &motionSpeeds; }
public:
	void LoadParameter( bool isBinary = true );

#if USE_IMGUI

	void SaveParameter();

	void UseImGui();

#endif // USE_IMGUI
};
CEREAL_CLASS_VERSION( PlayerParam, 10 )

class  skinned_mesh;	// With pointer. because I'm not want include this at header.
struct fbx_shader;		// Use for argument.
class Player
{
public:
	struct Input
	{
		Donya::Vector3 moveVector;	// Normalized.
		bool doDefend;
		bool doAttack;
		bool onlyRotation;	// If you want only rotate by "moveVector", enable this.
	public:
		Input() :
			moveVector(),
			doDefend( false ),
			doAttack( false ),
			onlyRotation( false )
		{}
		Input( const Donya::Vector3 &moveVector, bool doDefend, bool doAttack, bool onlyRotation = false ) :
			moveVector( moveVector ),
			doDefend( doDefend ),
			doAttack( doAttack ),
			onlyRotation( onlyRotation )
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
		std::shared_ptr<skinned_mesh> pDefeat{ nullptr };
	};
	enum class State
	{
		Idle,
		Run,
		Defend,
		Attack,
		Dead,
	};
	enum MotionKind
	{
		Idle = 0,
		Run,
		Defend,
		Attack,
		Defeat,

		MOTION_COUNT
	};
private:
	State					status;
	int						stageNo;			// 0-based.
	float					timer;				// Recycle between each state.
	float					shieldsRecastTime;	// I can defend when this time is zero.
	float					FXIntervalTimer;	// Used to only FX's generate timing and SE.
	float					fieldRadius;		// For collision to wall. the field is perfect-circle, so I can detect collide to wall by distance.
	MotionKind				currentMotion;
	Donya::Vector3			pos;				// In world space.
	Donya::Vector3			velocity;			// In world space.
	Donya::Vector3			lookDirection;		// In world space.
	Donya::Vector3			extraOffset;		// Actual position is "pos + extraOffset".
	Donya::Quaternion		orientation;
	Models					models;
	Shield					shield;
	std::vector<Donya::Box>	wallCollisions;		// Stored hit-boxes belongs world space, XZ plane. This variable only assigned at Init().
	bool					wasSucceededDefence;
	bool					isContinuingDefence;// Prevent continuous unfold since finished the shield's remaining frame.
public:
	Player();
	~Player();
public:
	void Init( int stageNumber, const Donya::Vector3 &wsInitialPos, const Donya::Vector3 &initialAnglesRadian, const std::vector<Donya::Box> &wsXZWallCollisions );
	void Uninit();

	void Update( Input input, float elapsedTime );
	/// <summary>
	/// Update the position, then doing collision to walls and repulsion.
	/// </summary>
	void PhysicUpdate( const std::vector<Donya::Circle> &xzCylinderWalls );

	void Draw( fbx_shader &HLSL, const Donya::Vector4x4 &matView, const Donya::Vector4x4 &matProjection );
	void DrawZ( fbx_shader &HLSL, const Donya::Vector4x4 &matView, const Donya::Vector4x4 &matProjection );
	void DrawBloom( fbx_shader &HLSL, const Donya::Vector4x4 &matView, const Donya::Vector4x4 &matProjection );
public:
	bool IsDefeated() const;

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
	void ChangeStatus( Input input, float elapsedTime );
	void UpdateCurrentStatus( Input input, float elapsedTime );

	void ChangeStatusFromIdle( Input input );
	void IdleInit( Input input );
	void IdleUpdate( Input input, float elapsedTime );
	void IdleUninit();

	void ChangeStatusFromRun( Input input );
	void RunInit( Input input );
	void RunUpdate( Input input, float elapsedTime );
	void RunUninit();

	void ChangeStatusFromDefend( Input input );
	void DefendInit( Input input );
	void DefendUpdate( Input input, float elapsedTime );
	void DefendUninit();

	void ChangeStatusFromAttack( Input input );
	void AttackInit( Input input );
	void AttackUpdate( Input input, float elapsedTime );
	void AttackUninit();

	void DefeatInit();
	void DefeatUpdate( float elapsedTime );
	void DefeatUninit();
private:
	void AssignInputVelocity( Input input, float elapsedTime );

	/// <summary>
	/// The position("pos") is only changed by this method(or CollideToStagesWall method).
	/// </summary>
	void ApplyVelocity( const std::vector<Donya::Circle> &xzCylinderWalls );
	/// <summary>
	/// If the position("pos") without to field range, clamp to field range.
	/// </summary>
	void CollideToStagesWall();
private:
	void ShieldUpdate( float elapsedTime );
	bool CanUnfoldShield() const;

	void FXUpdate( float elapsedTime );
private:
#if USE_IMGUI

	void UseImGui();

#endif // USE_IMGUI
};
