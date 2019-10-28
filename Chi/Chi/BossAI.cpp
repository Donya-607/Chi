#include "BossAI.h"
#include "imgui.h"

void BossAI::Init()
{
	state = ActionStateNum::WAIT;

	for (int i = 0; i < MAX_SIZE_CNT; i++)
	{
		cnt[i] = 0;
	}

	maxCnt[0] = 60;
	maxCnt[1] = 120;
	maxCnt[2] = 180;

	coolTimeMaxCnt[0] = 30;
	coolTimeMaxCnt[1] = 40;
	coolTimeMaxCnt[2] = 50;

	percent[0] = 25;
	percent[1] = 35;
	percent[2] = 40;
}

void BossAI::Update()
{
	if (cnt[state] <= maxCnt[state]) // 行動中
	{
		switch (state)
		{
		case ActionStateNum::WAIT:
			// 待機

			break;
		case ActionStateNum::MOVE:
			// 移動

			break;
		case ActionStateNum::ATTACK:
			// 攻撃

			break;
		}
	}
	else // クールタイム中
	{

	}

	if (maxCnt[state] + coolTimeMaxCnt[state] < cnt[state]++) // クールタイム後
	{
		cnt[state] = 0;
		StateRand();
	}
}

void BossAI::StateRand()
{
#if 0
	state = rand() % ActionStateNum::END;
#else
	int percentRand = rand() % 101;
	if (percentRand <= percent[0])
	{
		state = ActionStateNum::WAIT;
	}
	else if (percentRand <= percent[0] + percent[1])
	{
		state = ActionStateNum::MOVE;
	}
	else if (percentRand <= percent[0] + percent[1] + percent[2])
	{
		state = ActionStateNum::ATTACK;
	}
#endif
}

void  BossAI::ImGui()
{
	ImGui::Text("state : %d", state);
	ImGui::Text("wait_cnt : %d", cnt[0]);
	ImGui::Text("move_cnt : %d", cnt[1]);
	ImGui::Text("attack_cnt : %d", cnt[2]);
	ImGui::Text("wait_max_cnt : %d", maxCnt[0]);
	ImGui::Text("move_max_cnt : %d", maxCnt[1]);
	ImGui::Text("attack_max_cnt : %d", maxCnt[2]);
	ImGui::Text("wait_coolTime_max_cnt : %d", coolTimeMaxCnt[0]);
	ImGui::Text("move_coolTime_max_cnt : %d", coolTimeMaxCnt[1]);
	ImGui::Text("attack_coolTime_max_cnt : %d", coolTimeMaxCnt[2]);
}