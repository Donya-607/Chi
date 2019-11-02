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

#include "BossAI.h"

class skinned_mesh;	// With pointer. because I'm not want include this at header.

/// <summary>
/// Boss's serialize parameter.
/// </summary>
class BossParam final : public Donya::Singleton<BossParam>
{
	friend Donya::Singleton<BossParam>;
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
	float							scale;					// Usually 1.0f.
	float							stageBodyRadius;		// Using for collision to stage's wall.
	float							targetDistNear;			// 0.0f ~ 1.0f.
	float							targetDistFar;			// 0.0f ~ 1.0f.
	float							idleSlerpFactor;		// 0.0 ~ 1.0. Use when status is idle.
	float							moveMoveSpeed;			// Use when status is move.
	float							moveSlerpFactor;		// 0.0 ~ 1.0. Use when status is move.
	float							attackFastMoveSpeed;	// Use when status is attack of fast.
	float							attackFastSlerpFactor;	// 0.0 ~ 1.0. Use when status is attack of fast.
	std::vector<Donya::Vector3>		initPosPerStage;		// The index(stage number) is 1-based. 0 is tutorial.
	std::vector<Donya::Vector3>		drawOffsetsPerStage;	// The index(stage number) is 1-based. 0 is tutorial.
	std::vector<Donya::AABB>		hitBoxesBody;			// Body's hit boxes.
	std::vector<Donya::OBBFrame>	OBBAttacksSwing;
	std::vector<OBBFrameWithName>	OBBFAttacksFast;
private:
	BossParam();
public:
	~BossParam();
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
				CEREAL_NVP( initPosPerStage ),
				CEREAL_NVP( drawOffsetsPerStage ),
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
	static constexpr const char *SERIAL_ID = "Boss";
public:
	void Init();
	void Uninit();
public:
	float								Scale()				const	{ return scale; }
	float								StageBodyRadius()	const	{ return stageBodyRadius; }
	float								TargetDistNear()	const	{ return targetDistNear; }
	float								TargetDistFar()		const	{ return targetDistFar; }
	float								MoveSpeed  ( BossAI::ActionState status ) const;
	float								SlerpFactor( BossAI::ActionState status ) const;
	Donya::Vector3						GetInitPosition( int stageNumber ) const;
	Donya::Vector3						GetDrawOffset  ( int stageNumber ) const;
	std::vector<Donya::OBBFrame>		*OBBAtksSwing()			{ return &OBBAttacksSwing; }
	const std::vector<Donya::OBBFrame>	*OBBAtksSwing()	const	{ return &OBBAttacksSwing; }
	std::vector<OBBFrameWithName>		*OBBFAtksFast()			{ return &OBBFAttacksFast; }
	const std::vector<OBBFrameWithName>	*OBBFAtksFast()	const	{ return &OBBFAttacksFast; }
	const std::vector<Donya::AABB>		*BodyHitBoxes()	const	{ return &hitBoxesBody; }
public:
	void LoadParameter( bool isBinary = true );

#if USE_IMGUI

	void SaveParameter();

	void UseImGui();

#endif // USE_IMGUI
};
CEREAL_CLASS_VERSION( BossParam, 6 )
CEREAL_CLASS_VERSION( BossParam::OBBFrameWithName, 0 )

class Boss
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
		std::unique_ptr<skinned_mesh> pAtkFast{ nullptr };
		std::unique_ptr<skinned_mesh> pAtkSwing{ nullptr };
	};
private:
	BossAI::ActionState		status;
	BossAI					AI;
	int						stageNo;		// 1-based.
	float					fieldRadius;	// For collision to wall. the field is perfect-circle, so I can detect collide to wall by distance.
	float					slerpFactor;	// 0.0f ~ 1.0. Use orientation's rotation.
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

	void Update( TargetStatus target );

	void Draw( const Donya::Vector4x4 &matView, const Donya::Vector4x4 &matProjection );
public:
	std::vector<Donya::OBB> GetAttackHitBoxes() const;
	std::vector<Donya::OBB> GetBodyHitBoxes() const;

	void ReceiveImpact();

	void SetFieldRadius( float fieldRadius );
private:
	void LoadModel();

	float CalcNormalizedDistance( Donya::Vector3 wsTargetPos );
private:
	void ChangeStatus( TargetStatus target );
	void UpdateCurrentStatus( TargetStatus target );

	void WaitInit( TargetStatus target );
	void WaitUpdate( TargetStatus target );
	void WaitUninit();

	void MoveInit( TargetStatus target );
	void MoveUpdate( TargetStatus target );
	void MoveUninit();

	void AttackSwingInit( TargetStatus target );
	void AttackSwingUpdate( TargetStatus target );
	void AttackSwingUninit();
	
	void AttackFastInit( TargetStatus target );
	void AttackFastUpdate( TargetStatus target );
	void AttackFastUninit();
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
