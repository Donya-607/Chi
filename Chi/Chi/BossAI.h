#pragma once

#include <array>
#include <memory>

#include <cereal/cereal.hpp>
#include <cereal/types/array.hpp>

#include "Donya/UseImGui.h"

class BossAI
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
	/// Only attack behavior.
	/// </summary>
	enum class AttackState
	{
		SWING,
		FAST,
		ROTATE,

		END
	};
	static constexpr int ATTACK_STATE_COUNT = static_cast<int>( AttackState::END );
private:
	ActionState status{};

	int timer{};
	int coolTime{};

	std::array<int, ATTACK_STATE_COUNT>	wholeFrame{};
	std::array<int, ATTACK_STATE_COUNT>	coolTimeFrame{};
	std::array<int, WAIT_STATE_COUNT>	waitPercents{};
	std::array<int, ATTACK_STATE_COUNT>	attackPercents{};
public:
	BossAI() {}
	// ~BossAI() = default;
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
	static constexpr const char *SERIAL_ID = "BossAI";
public:
	void Init();
	void Update();
	ActionState GetState() const { return status; }
private:
	ActionState ToActionState( WaitState status ) const;
	ActionState ToActionState( AttackState status ) const;

	void LotteryWaitState();
	void LotteryAttackState();

	bool NowStatusAction() const;
private:
	void LoadParameter( bool isBinary = true );

#if USE_IMGUI

	void SaveParameter();

public:

	void ImGui();

#endif // USE_IMGUI
};
CEREAL_CLASS_VERSION( BossAI, 2 )
