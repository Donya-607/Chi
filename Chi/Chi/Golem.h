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

#include "GolemAI.h"

class skinned_mesh;	// With pointer. because I'm not want include this at header.

/// <summary>
/// Golem's serialize parameter.
/// </summary>
class GolemParam final : public Donya::Singleton<GolemParam>
{
	friend Donya::Singleton<GolemParam>;
public:
	struct OBBFrameWithName
	{
		Donya::OBBFrame OBBF{};
		std::string meshName{};
	private:
		friend class cereal::access;
		template<class Archive>
		void serialize( Archive &archive, std::uint32_t version )
		{
			archive
			(
				CEREAL_NVP( OBBF ),
				CEREAL_NVP( meshName )
			);

			if ( 1 <= version )
			{
				// archive( CEREAL_NVP( x ) );
			}
		}
	public:
		Donya::OBB CalcTransformedOBB( skinned_mesh *pMesh, const Donya::Vector4x4 &parentSpaceMatrix ) const;
	};
private:
	int								swingStopFrame;			// Use when status is attack of swing.
	float							swingStopLength;		// Per second. Use when status is attack of swing.
	int								rotLeaveEaseKind;		// Use when status is attack of rotate.
	int								rotLeaveEaseType;		// Use when status is attack of rotate.
	int								rotLeaveStartFrame;		// Use when status is attack of rotate.
	int								rotLeaveWholeFrame;		// Use when status is attack of rotate.
	float							rotLeaveDistance;		// Use when status is attack of rotate.
	float							scale;					// Usually 1.0f.
	float							stageBodyRadius;		// Using for collision to stage's wall.
	float							targetDistNear;			// 0.0f ~ 1.0f.
	float							targetDistFar;			// 0.0f ~ 1.0f.
	float							idleSlerpFactor;		// 0.0 ~ 1.0. Use when status is idle.
	float							moveMoveSpeed;			// Use when status is move.
	float							moveSlerpFactor;		// 0.0 ~ 1.0. Use when status is move.
	int								defeatMotionLength{};	// Per frame. Use when status is defeat(END).
	float							defeatHideSpeed{};		// Use after motionLength.
	float							attackFastMoveSpeed;	// Use when status is attack of fast.
	float							attackFastSlerpFactor;	// 0.0 ~ 1.0. Use when status is attack of fast.
	float							attackRotateMoveSpeed;	// Use when status is attack of rotate.
	float							attackRotateSlerpFactor;// 0.0 ~ 1.0. Use when status is attack of rotate.
	Donya::Vector3					initPos;
	Donya::Vector3					drawOffset;
	std::vector<float>				motionSpeeds;			// Magnification.
	std::vector<Donya::Sphere>		hitBoxesBody;			// Body's hit boxes.
	std::vector<Donya::OBBFrame>	OBBAttacksSwing;
	std::vector<OBBFrameWithName>	OBBFAttacksFast;
	std::vector<Donya::SphereFrame>	rotateAttackCollisions;
private:
	GolemParam();
public:
	~GolemParam();
private:
	friend class cereal::access;
	template<class Archive>
	void serialize( Archive &archive, std::uint32_t version )
	{
		if ( 4 <= version )
		{
			archive
			(
				CEREAL_NVP( scale ),
				CEREAL_NVP( hitBoxesBody ),
				CEREAL_NVP( initPos ),
				CEREAL_NVP( drawOffset ),
				CEREAL_NVP( OBBAttacksSwing ),
				CEREAL_NVP( OBBFAttacksFast )
			);

			if ( 5 <= version )
			{
				archive
				(
					CEREAL_NVP( stageBodyRadius ),
					CEREAL_NVP( targetDistNear ),
					CEREAL_NVP( targetDistFar ),
					CEREAL_NVP( moveMoveSpeed ),
					CEREAL_NVP( attackFastMoveSpeed )
				);
			}
			if ( 6 <= version )
			{
				archive
				(
					CEREAL_NVP( idleSlerpFactor ),
					CEREAL_NVP( moveSlerpFactor ),
					CEREAL_NVP( attackFastSlerpFactor )
				);
			}
			if ( 7 <= version )
			{
				archive
				(
					CEREAL_NVP( attackRotateMoveSpeed ),
					CEREAL_NVP( attackRotateSlerpFactor ),
					CEREAL_NVP( rotateAttackCollisions )
				);
			}
			if ( 8 <= version )
			{
				archive
				(
					CEREAL_NVP( rotLeaveEaseKind ),
					CEREAL_NVP( rotLeaveEaseType ),
					CEREAL_NVP( rotLeaveStartFrame ),
					CEREAL_NVP( rotLeaveWholeFrame ),
					CEREAL_NVP( rotLeaveDistance )
				);
			}
			if ( 9 <= version )
			{
				archive
				(
					CEREAL_NVP( swingStopFrame ),
					CEREAL_NVP( swingStopLength )
				);
			}
			if ( 10 <= version )
			{
				archive
				(
					CEREAL_NVP( defeatMotionLength ),
					CEREAL_NVP( defeatHideSpeed )
				);
			}
			if ( 11 <= version )
			{
				archive( CEREAL_NVP( motionSpeeds ) );
			}
			if ( 12 <= version )
			{
				// archive( CEREAL_NVP( x ) );
			}

			return;
		}
		// else

		/*
		archive
		(
			CEREAL_NVP( scale )
		);

		//if ( 1 <= version )
		//{
		//	archive
		//	( 
		//		CEREAL_NVP( OBBAttacksFast ),
		//		CEREAL_NVP( OBBAttacksSwing )
		//	);
		//}
		if ( 2 <= version )
		{
			archive( CEREAL_NVP( hitBoxesBody ) );
		}
		if ( 3 <= version )
		{
			archive
			(
				CEREAL_NVP( initPosPerStage ),
				CEREAL_NVP( drawOffsetsPerStage )
			);
		}
		if ( 4 <= version )
		{
			archive
			(
				CEREAL_NVP( OBBAttacksSwing ),
				CEREAL_NVP( OBBFAttacksFast )
			);
		}
		if ( 5 <= version )
		{
			// archive( CEREAL_NVP( x ) );
		}
		*/
	}
	static constexpr const char *SERIAL_ID = "Golem";
public:
	void Init( size_t motionCount );
	void Uninit();
public:
	// These getter method for maintaining private member.

	int										SwingStopFrame()		const	{ return swingStopFrame; }
	float									SwingStopSecond()		const	{ return swingStopLength; }
	int										RotLeaveEaseKind()		const	{ return rotLeaveEaseKind; }
	int										RotLeaveEaseType()		const	{ return rotLeaveEaseType; }
	int										RotLeaveStartFrame()	const	{ return rotLeaveStartFrame; }
	int										RotLeaveWholeFrame()	const	{ return rotLeaveWholeFrame; }
	float									RotLeaveDistance()		const	{ return rotLeaveDistance; }
	float									Scale()					const	{ return scale; }
	float									StageBodyRadius()		const	{ return stageBodyRadius; }
	float									TargetDistNear()		const	{ return targetDistNear; }
	float									TargetDistFar()			const	{ return targetDistFar; }
	float									MoveSpeed  ( GolemAI::ActionState status ) const;
	float									SlerpFactor( GolemAI::ActionState status ) const;
	int										DefeatMotionLength()	const	{ return defeatMotionLength; }
	float									DefeatHideSpeed()		const	{ return defeatHideSpeed; }
	Donya::Vector3							GetInitPosition()		const	{ return initPos; }
	Donya::Vector3							GetDrawOffset  ()		const	{ return drawOffset; }
	std::vector<float>						*MotionSpeeds()					{ return &motionSpeeds; }
	const std::vector<float>				*MotionSpeeds()			const	{ return &motionSpeeds; }
	std::vector<Donya::OBBFrame>			*OBBAtksSwing()					{ return &OBBAttacksSwing; }
	const std::vector<Donya::OBBFrame>		*OBBAtksSwing()			const	{ return &OBBAttacksSwing; }
	std::vector<OBBFrameWithName>			*OBBFAtksFast()					{ return &OBBFAttacksFast; }
	const std::vector<OBBFrameWithName>		*OBBFAtksFast()			const	{ return &OBBFAttacksFast; }
	std::vector<Donya::SphereFrame>			*RotateAtkCollisions()			{ return &rotateAttackCollisions; }
	const std::vector<Donya::SphereFrame>	*RotateAtkCollisions()	const	{ return &rotateAttackCollisions; }
	const std::vector<Donya::Sphere>		*BodyHitBoxes()			const	{ return &hitBoxesBody; }
public:
	void LoadParameter( bool isBinary = true );

#if USE_IMGUI

	void SaveParameter();

	void UseImGui();

#endif // USE_IMGUI
};
CEREAL_CLASS_VERSION( GolemParam, 11 )
CEREAL_CLASS_VERSION( GolemParam::OBBFrameWithName, 0 )

struct fbx_shader; // Use for argument.
/// <summary>
/// Golem number 3.
/// </summary>
class Golem
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
		std::shared_ptr<skinned_mesh> pAtkFast{ nullptr };
		std::shared_ptr<skinned_mesh> pAtkSwing{ nullptr };
		std::shared_ptr<skinned_mesh> pAtkRotate{ nullptr };
	};
	enum MotionKind
	{
		Idle = 0,
		Defeat,
		AtkSwing,
		AtkFast,
		AtkRotate,

		MOTION_COUNT
	};
private:
	GolemAI::ActionState	status;
	GolemAI					AI;
	int						stageNo;		// 1-based.
	int						timer;			// Recycle between each state.
	int						swingTimer;		// Use when status is attack of swing.
	float					moveSign;		// Use when aim-move state. store destination sign(-1:left, +1:right).
	float					fieldRadius;	// For collision to wall. the field is perfect-circle, so I can detect collide to wall by distance.
	float					slerpFactor;	// 0.0f ~ 1.0f. Use orientation's rotation.
	float					easeFactor;		// 0.0f ~ 1.0f.
	MotionKind				currentMotion;
	Donya::Vector3			pos;			// Contain world space position. actual position is "pos + extraOffset".
	Donya::Vector3			velocity;
	Donya::Vector3			extraOffset;	// Actual position is "pos + extraOffset".
	Donya::Quaternion		orientation;
	Models					models;
public:
	Golem();
	~Golem();
public:
	void Init( int stageNo );
	void Uninit();

	void Update( TargetStatus target );

	void Draw(fbx_shader& HLSL, const Donya::Vector4x4& matView, const Donya::Vector4x4& matProjection, float animationAcceleration = 1.0f);
	void z_Draw(fbx_shader& HLSL, const Donya::Vector4x4& matView, const Donya::Vector4x4& matProjection, float animationAcceleration = 1.0f);
	void bloom_Draw(fbx_shader& HLSL, const Donya::Vector4x4& matView, const Donya::Vector4x4& matProjection, float animationAcceleration = 1.0f);
public:
	bool IsCollideAttackHitBoxes( const Donya::AABB   judgeOther, bool disableCollidingHitBoxes );
	bool IsCollideAttackHitBoxes( const Donya::OBB    judgeOther, bool disableCollidingHitBoxes );
	bool IsCollideAttackHitBoxes( const Donya::Sphere judgeOther, bool disableCollidingHitBoxes );
	std::vector<Donya::OBB>		RequireAttackHitBoxesOBB() const;
	std::vector<Donya::Sphere>	RequireAttackHitBoxesSphere() const;
	std::vector<Donya::Sphere>	GetBodyHitBoxes() const;

	void ReceiveImpact();

	void SetFieldRadius( float fieldRadius );

	Donya::Vector3 GetPos() const { return pos + extraOffset; }
	GolemAI::ActionState GetStatus() const { return status; }

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

	void MoveInit( TargetStatus target, GolemAI::ActionState statusDetail );
	void MoveUpdate( TargetStatus target );
	void MoveUninit();

	void AttackSwingInit( TargetStatus target );
	void AttackSwingUpdate( TargetStatus target );
	void AttackSwingUninit();
	
	void AttackFastInit( TargetStatus target );
	void AttackFastUpdate( TargetStatus target );
	void AttackFastUninit();

	void AttackRotateInit( TargetStatus target );
	void AttackRotateUpdate( TargetStatus target );
	void AttackRotateUninit();

	void DefeatInit();
	void DefeatUpdate();
	void DefeatUninit();
private:
	/// <summary>
	/// The position("pos") is only changed by this method(or CollideToWall method).
	/// </summary>
	void ApplyVelocity( TargetStatus target );
	/// <summary>
	/// If the position("pos") without to field range, clamp to field range.
	/// </summary>
	void CollideToWall();

	void FxUpdate( TargetStatus target );
private:
#if USE_IMGUI

	void UseImGui( float normalizedTargetDistance );

#endif // USE_IMGUI
};
