#pragma once

#include <memory>


class BossAI
{
private:
	static const int MAX_SIZE_CNT = 3;

	int state;
	int cnt[MAX_SIZE_CNT];
	int maxCnt[MAX_SIZE_CNT];
	int coolTimeMaxCnt[MAX_SIZE_CNT];
	int percent[MAX_SIZE_CNT];

public:
	enum ActionStateNum
	{
		WAIT,
		MOVE,
		ATTACK,
		END
	};

private:
	void StateRand();

public:
	BossAI() {}
	~BossAI() {}

	void Init();
	void Update();
	void ImGui();
	int GetState() { return state; }
};