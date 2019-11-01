#pragma once

#include <array>
#include <memory>

#include <cereal/cereal.hpp>
#include <cereal/types/array.hpp>

#include "Donya/UseImGui.h"

class BossAI
{
public:
	enum ActionState
	{
		WAIT,
		MOVE,
		ATTACK_SWING,
		ATTACK_FAST,

		END
	};
	static constexpr int ACTION_STATE_COUNT = static_cast<int>( END );
private:
	int state{};
	std::array<int, ACTION_STATE_COUNT> cnt{};
	std::array<int, ACTION_STATE_COUNT> maxCnt{};
	std::array<int, ACTION_STATE_COUNT> coolTimeMaxCnt{};
	std::array<int, ACTION_STATE_COUNT> percent{};
private:
	void StateRand();
public:
	BossAI() {}
	~BossAI() {}
private:
	friend class cereal::access;
	template<class Archive>
	void serialize( Archive &archive, std::uint32_t version )
	{
		archive
		(
			CEREAL_NVP( percent )
		);

		if ( 1 <= version )
		{
			archive
			(
				CEREAL_NVP( maxCnt ),
				CEREAL_NVP( coolTimeMaxCnt )
			);
		}
		if ( 2 <= version )
		{
			// archive( CEREAL_NVP( x ) );
		}
	}
	static constexpr const char *SERIAL_ID = "BossAI";
public:
	void Init();
	void Update();
	int GetState() { return state; }
private:
	void LoadParameter( bool isBinary = true );

#if USE_IMGUI

	void SaveParameter();

public:

	void ImGui();

#endif // USE_IMGUI
};
CEREAL_CLASS_VERSION( BossAI, 1 )
