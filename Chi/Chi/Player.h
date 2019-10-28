#pragma once

#include <memory>

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
	float scale;				// Usually 1.0f.
	float runSpeed;				// Scalar.
	float rotSlerpFactor;		// Use player's rotation.
	Donya::AABB   hitBoxBody;	// HitBox that collide to boss attacks.
	Donya::Sphere hitBoxPhysic;	// HitBox that collide to stage.
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
			// archive( CEREAL_NVP( x ) );
		}
	}
	static constexpr const char *SERIAL_ID = "Player";
public:
	void Init();
	void Uninit();
public:
	float Scale()		const { return scale; }
	float RunSpeed()	const { return runSpeed; }
	float SlerpFactor()	const { return rotSlerpFactor; }
public:
	void LoadParameter( bool isBinary = true );

#if USE_IMGUI

	void SaveParameter();

	void UseImGui();

#endif // USE_IMGUI
};
CEREAL_CLASS_VERSION( PlayerParam, 1 )

class skinned_mesh;	// With pointer. because I'm not want include this at header.
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
	};
private:
	enum class State
	{
		Idle,
		Run,
		Defend,
		Attack,
	};
private:
	State							status;
	Donya::Vector3					pos;			// In world space.
	Donya::Vector3					velocity;		// In world space.
	Donya::Vector3					lookDirection;	// In world space.
	Donya::Quaternion				orientation;
	std::unique_ptr<skinned_mesh>	pModel;
public:
	Player();
	~Player();
public:
	void Init();
	void Uninit();

	void Update( Input input );

	void Draw( const Donya::Vector4x4 &matView, const Donya::Vector4x4 &matProjection );
private:
	void LoadModel();
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
	/// The position("pos") is only changed by this method.
	/// </summary>
	void ApplyVelocity();
private:
#if USE_IMGUI

	void UseImGui();

#endif // USE_IMGUI
};
