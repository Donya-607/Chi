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

#include "RivalAI.h"

class skinned_mesh;	// With pointer. because I'm not want include this at header.

/// <summary>
/// Rival's serialize parameter.
/// </summary>
class RivalParam final : public Donya::Singleton<RivalParam>
{
	friend Donya::Singleton<RivalParam>;
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
		struct Move
		{
			float	moveSpeed{};
			float	slerpFactor{}; // 0.0f ~ 1.0f.
		private:
			friend class cereal::access;
			template<class Archive>
			void serialize( Archive &archive, std::uint32_t version )
			{
				archive
				(
					CEREAL_NVP( moveSpeed ),
					CEREAL_NVP( slerpFactor )
				);

				if ( 1 <= version )
				{
					// archive( CEREAL_NVP( x ) );
				}
			}
		};
		struct Barrage
		{
			float	moveSpeed{};
			float	slerpFactor{}; // 0.0f ~ 1.0f.
		private:
			friend class cereal::access;
			template<class Archive>
			void serialize( Archive &archive, std::uint32_t version )
			{
				archive
				(
					CEREAL_NVP( moveSpeed ),
					CEREAL_NVP( slerpFactor )
				);

				if ( 1 <= version )
				{
					// archive( CEREAL_NVP( x ) );
				}
			}
		};
		struct Line
		{
			float	moveSpeed{};
			float	slerpFactor{}; // 0.0f ~ 1.0f.
		private:
			friend class cereal::access;
			template<class Archive>
			void serialize( Archive &archive, std::uint32_t version )
			{
				archive
				(
					CEREAL_NVP( moveSpeed ),
					CEREAL_NVP( slerpFactor )
				);

				if ( 1 <= version )
				{
					// archive( CEREAL_NVP( x ) );
				}
			}
		};
		struct Raid
		{
			float	moveSpeed{};
			float	slerpFactor{}; // 0.0f ~ 1.0f.
		private:
			friend class cereal::access;
			template<class Archive>
			void serialize( Archive &archive, std::uint32_t version )
			{
				archive
				(
					CEREAL_NVP( moveSpeed ),
					CEREAL_NVP( slerpFactor )
				);

				if ( 1 <= version )
				{
					// archive( CEREAL_NVP( x ) );
				}
			}
		};
		struct Rush
		{
			float	moveSpeed{};
			float	slerpFactor{}; // 0.0f ~ 1.0f.
		private:
			friend class cereal::access;
			template<class Archive>
			void serialize( Archive &archive, std::uint32_t version )
			{
				archive
				(
					CEREAL_NVP( moveSpeed ),
					CEREAL_NVP( slerpFactor )
				);

				if ( 1 <= version )
				{
					// archive( CEREAL_NVP( x ) );
				}
			}
		};

		float				scale{};				// Usually 1.0f.
		float				stageBodyRadius{};		// Using for collision to stage's wall.
		float				targetDistNear{};		// 0.0f ~ 1.0f.
		float				targetDistFar{};		// 0.0f ~ 1.0f.
		float				idleSlerpFactor{};		// 0.0 ~ 1.0. Use when status is idle.
		Donya::Vector3		initPos{};				// The index(stage number) is 1-based. 0 is tutorial.
		Donya::Vector3		drawOffset{};			// The index(stage number) is 1-based. 0 is tutorial.
		Donya::AABB			hitBoxBody{};			// Body's hit box.
		Move				move{};					// Use when status is move.
		Barrage				barrage{};				// Use when status is attack of barrage.
		Line				line{};					// Use when status is attack of line.
		Raid				raid{};					// Use when status is attack of raid.
		Rush				rush{};					// Use when status is attack of rush.
	};
private:
	Member m{};
private:
	RivalParam();
public:
	~RivalParam();
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
			CEREAL_NVP( m.initPos ),
			CEREAL_NVP( m.drawOffset ),
			CEREAL_NVP( m.hitBoxBody ),
			CEREAL_NVP( m.move ),
			CEREAL_NVP( m.barrage ),
			CEREAL_NVP( m.line ),
			CEREAL_NVP( m.raid ),
			CEREAL_NVP( m.rush )
		);

		if ( 1 <= version )
		{
			// archive( CEREAL_NVP( x ) );
		}
	}
	static constexpr const char *SERIAL_ID = "Rival";
public:
	void Init();
	void Uninit();
public:
	float MoveSpeed( RivalAI::ActionState status );
	float SlerpFactor( RivalAI::ActionState status );
	Member Content() const { return m; }
	static Member Open();
public:
	void LoadParameter( bool isBinary = true );

#if USE_IMGUI

	void SaveParameter();

	void UseImGui();

#endif // USE_IMGUI
};
CEREAL_CLASS_VERSION( RivalParam, 0 )
CEREAL_CLASS_VERSION( RivalParam::Member::Move,	0 )
CEREAL_CLASS_VERSION( RivalParam::Member::Barrage,	0 )
CEREAL_CLASS_VERSION( RivalParam::Member::Line,		0 )
CEREAL_CLASS_VERSION( RivalParam::Member::Raid,		0 )
CEREAL_CLASS_VERSION( RivalParam::Member::Rush,		0 )

struct fbx_shader; // Use for argument.
/// <summary>
/// Rival number 3.
/// </summary>
class Rival
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
		std::shared_ptr<skinned_mesh> pRun{ nullptr };
		std::shared_ptr<skinned_mesh> pBreak{ nullptr };
		std::shared_ptr<skinned_mesh> pLeave{ nullptr };
		std::shared_ptr<skinned_mesh> pDefeat{ nullptr };
		std::shared_ptr<skinned_mesh> pAtkBarrage{ nullptr };
		std::shared_ptr<skinned_mesh> pAtkLine{ nullptr };
		std::shared_ptr<skinned_mesh> pAtkRaid{ nullptr };
		std::shared_ptr<skinned_mesh> pAtkRushPre{ nullptr };
		std::shared_ptr<skinned_mesh> pAtkRushRaid{ nullptr };
		std::shared_ptr<skinned_mesh> pAtkRushPost{ nullptr };
	};
	enum class ExtraState
	{
		BREAK,
		LEAVE,
		DEFEAT,
		NONE
	};
private:
	RivalAI::ActionState	status;
	ExtraState				extraStatus;
	RivalAI					AI;
	int						timer;				// Recycle between each state.
	float					fieldRadius;		// For collision to wall. the field is perfect-circle, so I can detect collide to wall by distance.
	float					slerpFactor;		// 0.0f ~ 1.0f. Use orientation's rotation.
	Donya::Vector3			pos;				// Contain world space position. actual position is "pos + extraOffset".
	Donya::Vector3			velocity;
	Donya::Vector3			extraOffset;		// Actual position is "pos + extraOffset".
	Donya::Quaternion		orientation;
	Models					models;
public:
	Rival();
	~Rival();
public:
	void Init( int stageNo );
	void Uninit();

	void Update( TargetStatus target );

	void Draw( fbx_shader &HLSL, const Donya::Vector4x4 &matView, const Donya::Vector4x4 &matProjection );
public:
	bool IsCollideAttackHitBoxes( const Donya::AABB   judgeOther, bool disableCollidingHitBoxes );
	bool IsCollideAttackHitBoxes( const Donya::OBB    judgeOther, bool disableCollidingHitBoxes );

	/// <summary>
	/// Returns world space hit-box of body.
	/// </summary>
	Donya::AABB GetBodyHitBox() const;

	/// <summary>
	/// Please call when defended Rival's attack.
	/// </summary>
	void WasDefended();
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
	
	void AttackBarrageInit( TargetStatus target );
	void AttackBarrageUpdate( TargetStatus target );
	void AttackBarrageUninit();

	void AttackLineInit( TargetStatus target );
	void AttackLineUpdate( TargetStatus target );
	void AttackLineUninit();

	void AttackRaidInit( TargetStatus target );
	void AttackRaidUpdate( TargetStatus target );
	void AttackRaidUninit();

	void AttackRushInit( TargetStatus target );
	void AttackRushUpdate( TargetStatus target );
	void AttackRushUninit();
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
