#pragma once

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
	float scale;			// Usually 1.0f.
	float runSpeed;			// Scalar.
	float rotSlerpFactor;	// Use player's rotation.
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
			CEREAL_NVP( runSpeed )
			CEREAL_NVP( runSlerpFactor )
		);
		
		if ( 1 <= version )
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
CEREAL_CLASS_VERSION( PlayerParam, 0 )

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
	};
private:
	enum class State
	{
		Idle,
		Run,
	};
private:
	State				status;
	Donya::Vector3		pos;			// In world space.
	Donya::Vector3		velocity;		// In world space.
	Donya::Vector3		lookDirection;	// In world space.
	Donya::Quaternion	orientation;
	std::shared_ptr<Donya::StaticMesh>	pModel;
public:
	Player();
	~Player();
public:
	void Init();
	void Uninit();

	void Update( Input input );

	void Draw
	(
		const Donya::Vector4x4	&matView,
		const Donya::Vector4x4	&matProjection,
		const Donya::Vector4	&lightDirection,
		const Donya::Vector4	&lightColor,
		const Donya::Vector4	&materialColor = { 1.0f, 1.0f, 1.0f, 1.0f }
	) const;
private:
	void LoadModel();

	void Run( Input input );

	/// <summary>
	/// The position("pos") is only changed by this method.
	/// </summary>
	void ApplyVelocity();
private:
#if USE_IMGUI

	void UseImGui();

#endif // USE_IMGUI
};
