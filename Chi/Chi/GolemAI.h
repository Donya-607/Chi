#pragma once

#include <array>
#include <memory>
#include <vector>

#include <cereal/cereal.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/vector.hpp>

#include "Donya/UseImGui.h"

class GolemAI
{
public:
	/// <summary>
	/// Contain all behavior.
	/// </summary>
	enum class ActionState
	{
		WAIT,
		MOVE,
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
		MOVE,

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
	static constexpr int ATTACK_STATE_COUNT	= static_cast<int>( AttackState::END );		// Except an attack with gap.
	static constexpr int ALL_ATTACK_COUNT	= static_cast<int>( AttackState::END ) + 1;	// Contain an attack with gap.
private:
	ActionState status{};

	int timer{};
	int coolTime{};

	std::array<int, ALL_ATTACK_COUNT>	wholeFrame{};		// An attack with gap is stored at back().
	std::array<int, ALL_ATTACK_COUNT>	coolTimeFrame{};	// An attack with gap is stored at back().
	std::array<int, WAIT_STATE_COUNT>	waitPercents{};
	std::array<int, ATTACK_STATE_COUNT>	attackPercents{};

	int attackTimes{};					// Store "gapIntervals" count.
	int intervalIndex{};
	std::vector<int> gapIntervals{};	// At least have 1 element.
public:
	GolemAI() {}
	// ~GolemAI() = default;
private:
	friend class cereal::access;
	template<class Archive>
	void serialize( Archive &archive, std::uint32_t version )
	{
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
	void Update();

	ActionState GetState() const { return status; }
private:
	ActionState ToActionState( WaitState status ) const;
	ActionState ToActionState( AttackState status ) const;

	void LotteryWaitState();
	void LotteryAttackState();

	ActionState GetGapAttack() const;
private:
	void LoadParameter( bool isBinary = true );

#if USE_IMGUI

	void SaveParameter();

public:

	void ImGui();

#endif // USE_IMGUI
};
CEREAL_CLASS_VERSION( GolemAI, 4 )
