#pragma once

#include <memory>

#include "Donya/Collision.h"
#include "Donya/Quaternion.h"
#include "Donya/Serializer.h"
#include "Donya/Template.h"
#include "Donya/UseImGui.h"
#include "Donya/Vector.h"

class skinned_mesh;	// With pointer. because I'm not want include this at header.

/// <summary>
/// Shield's serialize parameter.
/// </summary>
class ShieldParam final : public Donya::Singleton<ShieldParam>
{
	friend Donya::Singleton<ShieldParam>;
public:
	struct Member
	{
		int					maxUnfoldableFrame{};
		int					consumptionAmount{};	// Subtract amount when unfold.
		int					decreaseSpeed{};		// Subtract to the frame of unfoldable while unfolding.
		int					increaseSpeed{};		// Add to the frame of unfoldable while does not unfolding.
		int					recoveryAmount{};
		float				drawScaleMax{};			// Maximum scale.
		float				drawScaleMin{};			// Minimum scale.
		Donya::Vector3		drawOffset{};
		Donya::AABB			hitBoxMax{};			// Maximum size hit box.
		Donya::AABB			hitBoxMin{};			// Minimum size hit box.
		std::vector<float>	motionSpeeds{};			// Magnification.
	};
private:
	Member m{};
private:
	ShieldParam();
public:
	~ShieldParam();
private:
	friend class cereal::access;
	template<class Archive>
	void serialize( Archive &archive, std::uint32_t version )
	{
		archive
		(
			CEREAL_NVP( m.maxUnfoldableFrame ),
			CEREAL_NVP( m.consumptionAmount ),
			CEREAL_NVP( m.decreaseSpeed ),
			CEREAL_NVP( m.increaseSpeed ),
			CEREAL_NVP( m.recoveryAmount ),
			CEREAL_NVP( m.drawScaleMax ),
			CEREAL_NVP( m.drawScaleMin ),
			CEREAL_NVP( m.drawOffset ),
			CEREAL_NVP( m.hitBoxMax ),
			CEREAL_NVP( m.hitBoxMin ),
			CEREAL_NVP( m.motionSpeeds )
		);

		if ( 1 <= version )
		{
			// archive( CEREAL_NVP( x ) );
		}
	}
	static constexpr const char *SERIAL_ID = "Shield";
public:
	void Init( size_t motionCount );
	void Uninit();
public:
	Member Content() const { return m; }
	static Member Open();

	float CalcUnfoldPercent( int currentTime );
public:
	void LoadParameter( bool isBinary = true );

#if USE_IMGUI

	void SaveParameter();

	void UseImGui();

#endif // USE_IMGUI
};
CEREAL_CLASS_VERSION( ShieldParam, 0 )

struct fbx_shader;	// Use for argument.
class Shield
{
private:
	struct Models
	{
		std::shared_ptr<skinned_mesh> pOpen{ nullptr };
		std::shared_ptr<skinned_mesh> pIdle{ nullptr };
		std::shared_ptr<skinned_mesh> pReact{ nullptr };
	};
	enum class State
	{
		NotExist,
		Open,
		Idle,
		React,
	};
	enum MotionKind
	{
		Open = 0,
		Idle,
		React,

		MOTION_COUNT
	};
private:
	State		status;
	int			unfoldTimer;
	int			fluctuation;	// Will add to "unfoldTimer" at last in Update().
	MotionKind	currentMotion;
	Models		models;
	bool		nowUnfolding;	// Store old status. Used for "moment of unfold" judgement.
public:
	Shield();
	~Shield();
public:
	void Init();
	void Uninit();

	void Update( bool isUnfolding );

	void Draw( fbx_shader &HLSL, const Donya::Vector4x4 &matView, const Donya::Vector4x4 &matProjection, const Donya::Vector4x4 &matParent );
public:
	bool CanUnfold() const;

	/// <summary>
	/// Returns local space AABB.
	/// </summary>
	Donya::AABB GetHitBox() const;
	/// <summary>
	/// Returns local space OBB.
	/// </summary>
	Donya::OBB GetHitBox( const Donya::Quaternion &parentOrientation ) const;

	/// <summary>
	/// Recover internal timer of unfoldable.
	/// </summary>
	void Recover();
private:
	void LoadModels();

	void AddTimer( int addition );

	void ApplyState( bool isUnfolding );

	void Fluctuate();

	void Elapse();

	void ChangeMotion( State newStatus );

	void DetectEndAnimation();

#if USE_IMGUI

	void UseImGui();

#endif // USE_IMGUI
};
