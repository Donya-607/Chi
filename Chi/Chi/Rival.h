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
	};
	struct IntervalSpeed
	{
		int		current{};
		int		start{};	// Will be serialize.
		int		last{};		// Will be serialize.
		float	speed{};	// Will be serialize.
		bool	withinFrame{ false };
	private:
		friend class cereal::access;
		template<class Archive>
		void serialize( Archive &archive, std::uint32_t version )
		{
			archive
			(
				CEREAL_NVP( start ),
				CEREAL_NVP( last ),
				CEREAL_NVP( speed )
			);

			if ( 1 <= version )
			{
				// archive( CEREAL_NVP( x ) );
			}
		}
	public:
		void Update( int elapsedTime = 1 )
		{
			current += elapsedTime;

			withinFrame = WithinEnableFrame() ? true : false;
		}
		bool WithinEnableFrame() const
		{
			return ( start <= current && current <= last ) ? true : false;
		}
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
		public:
			float			moveSpeed{};
			float			slerpFactor{}; // 0.0f ~ 1.0f.
			std::string		collideMeshName;
			std::vector<Donya::SphereFrame>	collisions;
			std::vector<IntervalSpeed>		walkTimings;
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
					archive
					(
						CEREAL_NVP( collideMeshName ),
						CEREAL_NVP( collisions ),
						CEREAL_NVP( walkTimings )
					);
				}
				if ( 2 <= version )
				{
					// archive( CEREAL_NVP( x ) );
				}
			}
		public:
			void Update( int elapsedTime = 1 )
			{
				for ( auto &it : collisions )
				{
					it.Update();
				}
				for ( auto &it : walkTimings )
				{
					it.Update();
				}
			}
		};
		struct Line
		{
			float	moveSpeed{};
			float	slerpFactor{};	// 0.0f ~ 1.0f.
			int		generateFrame{};
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
					archive( CEREAL_NVP( generateFrame ) );
				}
				if ( 2 <= version )
				{
					// archive( CEREAL_NVP( x ) );
				}
			}
		};
		struct Raid
		{
			float				moveSpeed{};
			float				slerpFactor{}; // 0.0f ~ 1.0f.
			int					easingKind{};
			int					easingType{};
			int					jumpStartFrame{};
			int					jumpLastFrame{};
			float				jumpDistance{};
			SphereFrameWithName	hitBox{};
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
					archive
					(
						CEREAL_NVP( easingKind ),
						CEREAL_NVP( easingType ),
						CEREAL_NVP( jumpStartFrame ),
						CEREAL_NVP( jumpLastFrame ),
						CEREAL_NVP( jumpDistance ),
						CEREAL_NVP( hitBox )
					);
				}
				if ( 2 <= version )
				{
					// archive( CEREAL_NVP( x ) );
				}
			}
		};
		struct Rush
		{
			float				moveSpeed{};
			float				slerpFactor{}; // 0.0f ~ 1.0f.
			int					waitLength{};
			float				rushSpeed{};
			int					slashStopAnimeFrame{};
			Donya::Sphere		slashOccurRange{};
			Donya::SphereFrame	hitBox{};
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
					archive
					(
						CEREAL_NVP( waitLength ),
						CEREAL_NVP( rushSpeed ),
						CEREAL_NVP( slashStopAnimeFrame ),
						CEREAL_NVP( slashOccurRange ),
						CEREAL_NVP( hitBox )
					);
				}
				if ( 2 <= version )
				{
					// archive( CEREAL_NVP( x ) );
				}
			}
		};
		struct Break
		{
			float	moveSpeed{};
			float	slerpFactor{};	// 0.0f ~ 1.0f.
			int		breakFrame{};
			int		leaveEasingKind{};
			int		leaveEasingType{};
			int		leaveWholeFrame{};
			int		leaveMoveFrame{};
			float	leaveDistance{};
		private:
			friend class cereal::access;
			template<class Archive>
			void serialize( Archive &archive, std::uint32_t version )
			{
				archive
				(
					CEREAL_NVP( moveSpeed ),
					CEREAL_NVP( slerpFactor ),
					CEREAL_NVP( breakFrame ),
					CEREAL_NVP( leaveEasingKind ),
					CEREAL_NVP( leaveEasingType ),
					CEREAL_NVP( leaveWholeFrame ),
					CEREAL_NVP( leaveMoveFrame ),
					CEREAL_NVP( leaveDistance )
				);

				if ( 1 <= version )
				{
					// archive( CEREAL_NVP( x ) );
				}
			}
		};
		struct Defeat
		{
			int		motionLength{};	// Per frame.
			float	hideSpeed{};	// Use after motionLength.
		private:
			friend class cereal::access;
			template<class Archive>
			void serialize( Archive &archive, std::uint32_t version )
			{
				archive
				(
					CEREAL_NVP( motionLength ),
					CEREAL_NVP( hideSpeed )
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
		Break				breakdown{};			// Use when status is attack of break.
		Defeat				defeat{};				// Use when status is attack of defeat.
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
			archive( CEREAL_NVP( m.breakdown ) );
		}
		if ( 2 <= version )
		{
			archive( CEREAL_NVP( m.defeat ) );
		}
		if ( 3 <= version )
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

	/// <summary>
	/// Update "walkTimings" and "collisions".
	/// </summary>
	void UpdateBarrage( int elapsedTime = 1 );
	/// <summary>
	/// Reset  "walkTimings" and "collisions".
	/// </summary>
	void ResetBarrage();
	std::vector<Donya::SphereFrame>			&BarrageHitBoxes()			{ return m.barrage.collisions; }
	const std::vector<Donya::SphereFrame>	&BarrageHitBoxes()	const	{ return m.barrage.collisions; }
	
	SphereFrameWithName						&RaidHitBox()				{ return m.raid.hitBox; }
	const SphereFrameWithName				&RaidHitBox()		const	{ return m.raid.hitBox; }
	
	Donya::SphereFrame						&RushHitBox()				{ return m.rush.hitBox; }
	const Donya::SphereFrame				&RushHitBox()		const	{ return m.rush.hitBox; }
public:
	void LoadParameter( bool isBinary = true );

#if USE_IMGUI

	void SaveParameter();

	void UseImGui();

#endif // USE_IMGUI
};
CEREAL_CLASS_VERSION( RivalParam, 2 )
CEREAL_CLASS_VERSION( RivalParam::IntervalSpeed,	0 )
CEREAL_CLASS_VERSION( RivalParam::Member::Move,		0 )
CEREAL_CLASS_VERSION( RivalParam::Member::Barrage,	1 )
CEREAL_CLASS_VERSION( RivalParam::Member::Line,		1 )
CEREAL_CLASS_VERSION( RivalParam::Member::Raid,		1 )
CEREAL_CLASS_VERSION( RivalParam::Member::Rush,		1 )
CEREAL_CLASS_VERSION( RivalParam::Member::Break,	0 )
CEREAL_CLASS_VERSION( RivalParam::Member::Defeat,	0 )

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
		std::shared_ptr<skinned_mesh> pAtkRushWait{ nullptr };
		std::shared_ptr<skinned_mesh> pAtkRushSlash{ nullptr };
	};
	enum class ExtraState
	{
		RUSH_WAIT,
		RUSH_RUN,
		RUSH_SLASH,
		BREAK,
		LEAVE,
		DEFEAT,
		NONE
	};
private:
	RivalAI::ActionState	status;
	ExtraState				extraStatus;		// Internal status.
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

	void Draw( fbx_shader &HLSL, const Donya::Vector4x4 &matView, const Donya::Vector4x4 &matProjection, float animationAcceleration = 1.0f );
public:
	bool IsCollideAttackHitBoxes( const Donya::AABB   judgeOther, bool disableCollidingHitBoxes );
	bool IsCollideAttackHitBoxes( const Donya::OBB    judgeOther, bool disableCollidingHitBoxes );

	/// <summary>
	/// Returns world space hit-box of body.
	/// </summary>
	Donya::AABB GetBodyHitBox() const;

	std::vector<Donya::SphereFrame> &BarragesLocalHitBoxes();

	Donya::Sphere RaidWorldHitBox() const;
	Donya::Sphere RushWorldHitBox() const;

	/// <summary>
	/// Please call when defended Rival's attack.
	/// </summary>
	void WasDefended();
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

	void BreakInit();
	void BreakUpdate( TargetStatus target );
	void BreakUninit();

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

	void UseImGui();

#endif // USE_IMGUI
};