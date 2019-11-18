#pragma once

#include <array>
#include <memory>
#include <vector>

#include <cereal/cereal.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/vector.hpp>

#include "Donya/UseImGui.h"


#include "AIBase.h"

class RivalAI
{
public:
	/// <summary>
	/// Contain all behavior.
	/// </summary>
	enum class ActionState
	{
		WAIT,
		MOVE_GET_NEAR,
		MOVE_GET_FAR,
		MOVE_SIDE,
		MOVE_AIM_SIDE,
		ATTACK_BARRAGE,
		ATTACK_LINE,
		ATTACK_RAID,
		ATTACK_RUSH,

		END
	};
	static constexpr int ACTION_STATE_COUNT = static_cast<int>( ActionState::END );
	static constexpr bool IsAction( ActionState status )
	{
		switch ( status )
		{
		case ActionState::ATTACK_BARRAGE:	return true;	// break;
		case ActionState::ATTACK_LINE:		return true;	// break;
		case ActionState::ATTACK_RAID:		return true;	// break;
		case ActionState::ATTACK_RUSH:		return true;	// break;
		default: break;
		}

		return false;
	}
private:
	/// <summary>
	/// Only cool-time behavior.
	/// </summary>
	enum class WaitState
	{
		WAIT,
		MOVE_GET_NEAR,
		MOVE_GET_FAR,
		MOVE_SIDE,
		MOVE_AIM_SIDE,

		END
	};
	static constexpr int WAIT_STATE_COUNT = static_cast<int>( WaitState::END );
	/// <summary>
	/// Only attack behavior. but excepting an attack with gap.
	/// </summary>
	enum class AttackState
	{
		BARRAGE,
		LINE,
		RAID,

		END
	};
	static constexpr int ATTACK_STATE_COUNT	= static_cast<int>( AttackState::END );		// Except an attack with gap.
	static constexpr int ALL_ATTACK_COUNT	= static_cast<int>( AttackState::END ) + 1;	// Contain an attack with gap.
private:
	ActionState		status{};
	ActionStorage	storage{};
	ActionStorage	initStorage{};

	float timer{};
	float coolTime{};
	float initCoolTime{};

	std::array<float, ALL_ATTACK_COUNT>	wholeFrame{};		// An attack with gap is stored at back().
	std::array<float, ALL_ATTACK_COUNT>	coolTimeFrame{};	// An attack with gap is stored at back().
	std::array<std::unique_ptr<LotteryBase>, ALL_ATTACK_COUNT>
		pAttackChoosers{};				// 0:by distance. 1:fixed.

	int attackTimes{};					// Store "gapIntervals" count.
	int intervalIndex{};
	std::vector<int> gapIntervals{};	// At least have 1 element.

	bool stopUpdate{ false };
public:
	RivalAI() {}
	// ~RivalAI() = default;
private:
	friend class cereal::access;
	template<class Archive>
	void serialize( Archive &archive, std::uint32_t version )
	{
		archive
		(
				CEREAL_NVP( wholeFrame ),
				CEREAL_NVP( coolTimeFrame )
		);

		if ( 1 <= version )
		{
			archive
			(
				CEREAL_NVP( pAttackChoosers ),
				CEREAL_NVP( initStorage ),
				CEREAL_NVP( initCoolTime ),
				CEREAL_NVP( gapIntervals )
			);
		}
		if ( 2 <= version )
		{
			// archive( CEREAL_NVP( x ) );
		}
	}
	static constexpr const char *SERIAL_ID = "RivalAI";
public:
	void Init();
	void Update( float elapsedTime, float normalizedTargetDistance );

	void StopUpdate()	{ stopUpdate = true;  }
	void ResumeUpdate()	{ stopUpdate = false; }
	ActionState GetState() const { return status; }

	void OverwriteState( ActionState newState );
	void FinishCurrentState( float normalizedTargetDistance );
private:
	ActionState ToActionState( WaitState status ) const;
	ActionState ToActionState( AttackState status ) const;
	AttackState ToAttackState( ActionState status ) const;

	void AssignState( float normalizedTargetDistance );
	void AssignWaitState();
	void AssignAttackState( float normalizedTargetDistance );
	ActionState LotteryAttack();

	ActionState GetGapAttack() const;
private:
	void LoadParameter( bool isBinary = true );

#if USE_IMGUI

	void SaveParameter();

public:

	void ImGui( float normalizedTargetDistance );

#endif // USE_IMGUI
};
CEREAL_CLASS_VERSION( RivalAI, 1 )
