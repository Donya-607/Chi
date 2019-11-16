#pragma once

#include <array>
#include <memory>
#include <vector>

#include <cereal/cereal.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/vector.hpp>

#include "Donya/UseImGui.h"

#include "AIBase.h"

class GolemAI
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
		ATTACK_SWING,
		ATTACK_FAST,
		ATTACK_ROTATE,

		END
	};
	static constexpr int ACTION_STATE_COUNT = static_cast<int>( ActionState::END );
	static constexpr bool IsAction( ActionState status )
	{
		switch ( status )
		{
		case ActionState::ATTACK_SWING:		return true;	// break;
		case ActionState::ATTACK_FAST:		return true;	// break;
		case ActionState::ATTACK_ROTATE:	return true;	// break;
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
		FAST,
		ROTATE,

		END
	};
	static constexpr int ATTACK_STATE_COUNT = static_cast<int>( AttackState::END );		// Except an attack with gap.
	static constexpr int ALL_ATTACK_COUNT = static_cast<int>( AttackState::END ) + 1;	// Contain an attack with gap.
private:
	ActionState		status{};
	ActionStorage	storage{};	// Store next status's data.
	ActionStorage	initStorage{};

	int timer{};
	int coolTime{};
	int initCoolTime{};

	std::array<int, ALL_ATTACK_COUNT>	wholeFrame{};		// An attack with gap is stored at back().
	std::array<int, ALL_ATTACK_COUNT>	coolTimeFrame{};	// An attack with gap is stored at back().
	std::array<std::unique_ptr<LotteryBase>, ALL_ATTACK_COUNT>
		pAttackChoosers{};				// 0:by distance. 1:fixed.

	int attackTimes{};					// Store "gapIntervals" count.
	int intervalIndex{};
	std::vector<int> gapIntervals{};	// At least have 1 element.

	bool stopUpdate{ false };
public:
	GolemAI() {}
	// ~GolemAI() = default;
private:
	friend class cereal::access;
	template<class Archive>
	void serialize( Archive &archive, std::uint32_t version )
	{
		if ( 5 <= version )
		{
			archive
			(
				CEREAL_NVP( wholeFrame ),
				CEREAL_NVP( coolTimeFrame ),
				CEREAL_NVP( pAttackChoosers ),
				CEREAL_NVP( initStorage ),
				CEREAL_NVP( initCoolTime ),
				CEREAL_NVP( gapIntervals )
			);

			if ( 6 <= version )
			{
				// archive( CEREAL_NVP( x ) );
			}
		}

		// OLD archives.

		/*
		if ( 2 <= version )
		{
			archive
			(
				CEREAL_NVP( wholeFrame ),
				CEREAL_NVP( coolTimeFrame )
			);

			if ( 3 <= version )
			{
				archive
				(
					CEREAL_NVP( waitPercents ),
					CEREAL_NVP( attackPercents )
				);
			}
			if ( 4 <= version )
			{ 
				archive( CEREAL_NVP( gapIntervals ) );
			}
			if ( 5 <= version )
			{
				// archive( CEREAL_NVP( x ) );
			}

			return;
		}
		// else
		*/

		/*
		archive
		(
			CEREAL_NVP( percent )
		);

		if ( 1 <= version )
		{
			archive
			(
				CEREAL_NVP( wholeFrame ),
				CEREAL_NVP( coolTimeFrame )
			);
		}
		*/
	}
	static constexpr const char *SERIAL_ID = "GolemAI";
public:
	void Init();
	void Update( float normalizedTargetDistance );

	void StopUpdate()	{ stopUpdate = true;  }
	void ResumeUpdate()	{ stopUpdate = false; }
	ActionState GetState() const { return status; }

	void OverwriteState( ActionState newState );
private:
	ActionState ToActionState( WaitState status ) const;
	ActionState ToActionState( AttackState status ) const;
	AttackState ToAttackState( ActionState status ) const;

	void AssignWaitState();
	void AssignAttackState( float normalizedTargetDistance );
	ActionState LotteryAttack();

	ActionState GetGapAttack() const;
private:
	void LoadParameter( bool isBinary = true );

#if USE_IMGUI

	void SaveParameter();

public:

	void ImGui();

#endif // USE_IMGUI
};
CEREAL_CLASS_VERSION( GolemAI, 5 )
