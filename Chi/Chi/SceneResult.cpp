#include "scene.h"
#include "SceneManager.h"
#include "AssetLoader.h"
#include "easing.h"

#include "Donya/Serializer.h"
#include "Donya/UseImGui.h"
#include "Donya/FilePath.h"
#include "Donya/Keyboard.h"
#include "Donya/Sound.h"

SceneResult::SceneResult() :
	rankingScore(), rankingRender(), yourScore(), yourScoreRender(),
	lights(), rankingPos(), yourScorePos(), rankingWidth()
{
	/*pLogo = std::make_shared<skinned_mesh>();
	pRanking = std::make_shared<skinned_mesh>();
	pYourScore = std::make_shared<skinned_mesh>();
	for (int i = 0; i < 10; i++)
	{
		pNummber[i] = std::make_shared<skinned_mesh>();
	}*/

	for (int i = 0; i < MAX_SIZE; i++)
	{
		rankingScore[i] = 0;
	}

	yourScore = GameTimer::GetInstance()->totalTimer.AllCurren();

	spriteLoad(&back, L"./Data/Images/UI/ResultFlame.png");
	spriteLoad(&text, L"./Data/Images/UI/ResultFont.png");
	spriteLoad(&result, L"./Data/Images/UI/ResultParts.png");
	spriteLoad(&ranking, L"./Data/Images/UI/Ranking.png");
	spriteLoad(&testBack, L"./Data/Images/UI/リザルト参考.png");

	state = 0;
	cnt = 0;
}

void SceneResult::init()  
{
	//	pLogo = AssetLoader::GetInstance()->logoModels.result;

	LoadParameter();

	int inNum = 0;
	bool aliveZero = false;
	for (int i = 0; i < MAX_SIZE; i++)
	{
		if (rankingScore[i] == 0)
		{
			rankingScore[i] = yourScore;
			inNum = i;
			aliveZero = true;
			break;
		}
	}

	if (aliveZero)
	{
		for (int i = inNum - 1; 0 <= i; i--)
		{
			if (yourScore <= rankingScore[i])
			{
				rankingScore[i+1] = rankingScore[i];
				rankingScore[i] = yourScore;
			}
		}
	}
	else
	{
		if (yourScore <= rankingScore[4])
		{
			rankingScore[4] = yourScore;
		}
		if (yourScore <= rankingScore[3])
		{
			rankingScore[4] = rankingScore[3];
			rankingScore[3] = yourScore;
		}
		if (yourScore <= rankingScore[2])
		{
			rankingScore[3] = rankingScore[2];
			rankingScore[2] = yourScore;
		}
		if (yourScore <= rankingScore[1])
		{
			rankingScore[2] = rankingScore[1];
			rankingScore[1] = yourScore;
		}
		if (yourScore <= rankingScore[0])
		{
			rankingScore[1] = rankingScore[0];
			rankingScore[0] = yourScore;
		}
	}

	for (int i = 0; i < MAX_SIZE; i++)
	{
		rankingTimer[i].minute = 0;
		rankingTimer[i].second = 0;
		rankingTimer[i].current = rankingScore[i];
		while (1)
		{
			if (60 <= rankingTimer[i].current)
			{
				rankingTimer[i].current -= 60;
				rankingTimer[i].second++;
				if (60 <= rankingTimer[i].second)
				{
					rankingTimer[i].second = 0;
					rankingTimer[i].minute++;
				}
			}
			else
			{
				break;
			}
		}
	}

	rankingPos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	yourScorePos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	rankingWidth = DirectX::XMFLOAT2(20.0f, 20.0f);

	SaveParameter();

	lights.direction.color = Donya::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	getLineLight().setLineLight(lights.direction.position,lights.direction.direction, lights.direction.color);
	for (const auto i : lights.points)
	{
		setPointLight(i);
	}


	backData.pos = Donya::Vector2(0.0f, 0.0f);
	backData.texPos = Donya::Vector2(0.0f, 0.0f);
	backData.texSize = Donya::Vector2(1920.0f, 1080.0f);

	textData.pos = Donya::Vector2(1920.0f - 1110.0f, 0.0f);
	textData.texPos = Donya::Vector2(0.0f, 0.0f);
	textData.texSize = Donya::Vector2(1110.0f, 1080.0f);

	for (int i = 0; i < 3; i++)
	{
		bossBackData[i].pos = Donya::Vector2(1920.0f - 1054.0f, 50.0f * i);
		bossBackData[i].texPos = Donya::Vector2(0.0f, 88.0f * i);
		bossBackData[i].texSize = Donya::Vector2(1054.0f, 88.0f);
	}

	boss1TimeData[0].pos = Donya::Vector2(0.0f, 0.0f);
	boss1TimeData[0].texPos = Donya::Vector2(64.0f * (GameTimer::GetInstance()->timer[0].Minute() / 10), 896.0f);
	boss1TimeData[0].texSize = Donya::Vector2(64.0f, 64.0f);
	boss2TimeData[0].pos = Donya::Vector2(0.0f, 0.0f);
	boss2TimeData[0].texPos = Donya::Vector2(64.0f * (GameTimer::GetInstance()->timer[1].Minute() / 10), 896.0f);
	boss2TimeData[0].texSize = Donya::Vector2(64.0f, 64.0f);
	boss3TimeData[0].pos = Donya::Vector2(0.0f, 0.0f);
	boss3TimeData[0].texPos = Donya::Vector2(64.0f * (GameTimer::GetInstance()->timer[2].Minute() / 10), 896.0f);
	boss3TimeData[0].texSize = Donya::Vector2(64.0f, 64.0f);
	totalTimeData[0].pos = Donya::Vector2(0.0f, 0.0f);
	totalTimeData[0].texPos = Donya::Vector2(224.0f * (GameTimer::GetInstance()->totalTimer.Minute() / 10), 420.0f);
	totalTimeData[0].texSize = Donya::Vector2(224.0f, 476.0f);

	boss1TimeData[1].pos = Donya::Vector2(0.0f, 0.0f);
	boss1TimeData[1].texPos = Donya::Vector2(64.0f * (GameTimer::GetInstance()->timer[0].Minute() % 10), 896.0f);
	boss1TimeData[1].texSize = Donya::Vector2(64.0f, 64.0f);
	boss2TimeData[1].pos = Donya::Vector2(0.0f, 0.0f);
	boss2TimeData[1].texPos = Donya::Vector2(64.0f * (GameTimer::GetInstance()->timer[1].Minute() % 10), 896.0f);
	boss2TimeData[1].texSize = Donya::Vector2(64.0f, 64.0f);
	boss3TimeData[1].pos = Donya::Vector2(0.0f, 0.0f);
	boss3TimeData[1].texPos = Donya::Vector2(64.0f * (GameTimer::GetInstance()->timer[2].Minute() % 10), 896.0f);
	boss3TimeData[1].texSize = Donya::Vector2(64.0f, 64.0f);
	totalTimeData[1].pos = Donya::Vector2(0.0f, 0.0f);
	totalTimeData[1].texPos = Donya::Vector2(224.0f * (GameTimer::GetInstance()->totalTimer.Minute() % 10), 420.0f);
	totalTimeData[1].texSize = Donya::Vector2(224.0f, 476.0f);

	boss1TimeData[2].pos = Donya::Vector2(0.0f, 0.0f);
	boss1TimeData[2].texPos = Donya::Vector2(64.0f * 11, 896.0f);
	boss1TimeData[2].texSize = Donya::Vector2(64.0f, 64.0f);
	boss2TimeData[2].pos = Donya::Vector2(0.0f, 0.0f);
	boss2TimeData[2].texPos = Donya::Vector2(64.0f * 11, 896.0f);
	boss2TimeData[2].texSize = Donya::Vector2(64.0f, 64.0f);
	boss3TimeData[2].pos = Donya::Vector2(0.0f, 0.0f);
	boss3TimeData[2].texPos = Donya::Vector2(64.0f * 11, 896.0f);
	boss3TimeData[2].texSize = Donya::Vector2(64.0f, 64.0f);
	totalTimeData[2].pos = Donya::Vector2(0.0f, 0.0f);
	totalTimeData[2].texPos = Donya::Vector2(224.0f * 11, 420.0f);
	totalTimeData[2].texSize = Donya::Vector2(224.0f, 476.0f);

	boss1TimeData[3].pos = Donya::Vector2(0.0f, 0.0f);
	boss1TimeData[3].texPos = Donya::Vector2(64.0f * (GameTimer::GetInstance()->timer[0].Second() / 10), 896.0f);
	boss1TimeData[3].texSize = Donya::Vector2(64.0f, 64.0f);
	boss2TimeData[3].pos = Donya::Vector2(0.0f, 0.0f);
	boss2TimeData[3].texPos = Donya::Vector2(64.0f * (GameTimer::GetInstance()->timer[1].Second() / 10), 896.0f);
	boss2TimeData[3].texSize = Donya::Vector2(64.0f, 64.0f);
	boss3TimeData[3].pos = Donya::Vector2(0.0f, 0.0f);
	boss3TimeData[3].texPos = Donya::Vector2(64.0f * (GameTimer::GetInstance()->timer[2].Second() / 10), 896.0f);
	boss3TimeData[3].texSize = Donya::Vector2(64.0f, 64.0f);
	totalTimeData[3].pos = Donya::Vector2(0.0f, 0.0f);
	totalTimeData[3].texPos = Donya::Vector2(224.0f * (GameTimer::GetInstance()->totalTimer.Second() / 10), 420.0f);
	totalTimeData[3].texSize = Donya::Vector2(224.0f, 476.0f);

	boss1TimeData[4].pos = Donya::Vector2(0.0f, 0.0f);
	boss1TimeData[4].texPos = Donya::Vector2(64.0f * (GameTimer::GetInstance()->timer[0].Second() % 10), 896.0f);
	boss1TimeData[4].texSize = Donya::Vector2(64.0f, 64.0f);
	boss2TimeData[4].pos = Donya::Vector2(0.0f, 0.0f);
	boss2TimeData[4].texPos = Donya::Vector2(64.0f * (GameTimer::GetInstance()->timer[1].Second() % 10), 896.0f);
	boss2TimeData[4].texSize = Donya::Vector2(64.0f, 64.0f);
	boss3TimeData[4].pos = Donya::Vector2(0.0f, 0.0f);
	boss3TimeData[4].texPos = Donya::Vector2(64.0f * (GameTimer::GetInstance()->timer[2].Second() % 10), 896.0f);
	boss3TimeData[4].texSize = Donya::Vector2(64.0f, 64.0f);
	totalTimeData[4].pos = Donya::Vector2(0.0f, 0.0f);
	totalTimeData[4].texPos = Donya::Vector2(224.0f * (GameTimer::GetInstance()->totalTimer.Second() % 10), 420.0f);
	totalTimeData[4].texSize = Donya::Vector2(224.0f, 476.0f);

	boss1TimeData[5].pos = Donya::Vector2(0.0f, 0.0f);
	boss1TimeData[5].texPos = Donya::Vector2(64.0f * 11, 896.0f);
	boss1TimeData[5].texSize = Donya::Vector2(64.0f, 64.0f);
	boss2TimeData[5].pos = Donya::Vector2(0.0f, 0.0f);
	boss2TimeData[5].texPos = Donya::Vector2(64.0f * 11, 896.0f);
	boss2TimeData[5].texSize = Donya::Vector2(64.0f, 64.0f);
	boss3TimeData[5].pos = Donya::Vector2(0.0f, 0.0f);
	boss3TimeData[5].texPos = Donya::Vector2(64.0f * 11, 896.0f);
	boss3TimeData[5].texSize = Donya::Vector2(64.0f, 64.0f);
	totalTimeData[5].pos = Donya::Vector2(0.0f, 0.0f);
	totalTimeData[5].texPos = Donya::Vector2(224.0f * 11, 420.0f);
	totalTimeData[5].texSize = Donya::Vector2(224.0f, 476.0f);

	boss1TimeData[6].pos = Donya::Vector2(0.0f, 0.0f);
	boss1TimeData[6].texPos = Donya::Vector2(64.0f * (GameTimer::GetInstance()->timer[0].Current() / 10), 896.0f);
	boss1TimeData[6].texSize = Donya::Vector2(64.0f, 64.0f);
	boss2TimeData[6].pos = Donya::Vector2(0.0f, 0.0f);
	boss2TimeData[6].texPos = Donya::Vector2(64.0f * (GameTimer::GetInstance()->timer[1].Current() / 10), 896.0f);
	boss2TimeData[6].texSize = Donya::Vector2(64.0f, 64.0f);
	boss3TimeData[6].pos = Donya::Vector2(0.0f, 0.0f);
	boss3TimeData[6].texPos = Donya::Vector2(64.0f * (GameTimer::GetInstance()->timer[2].Current() / 10), 896.0f);
	boss3TimeData[6].texSize = Donya::Vector2(64.0f, 64.0f);
	totalTimeData[6].pos = Donya::Vector2(0.0f, 0.0f);
	totalTimeData[6].texPos = Donya::Vector2(224.0f * (GameTimer::GetInstance()->totalTimer.Current() / 10), 420.0f);
	totalTimeData[6].texSize = Donya::Vector2(224.0f, 476.0f);

	boss1TimeData[7].pos = Donya::Vector2(0.0f, 0.0f);
	boss1TimeData[7].texPos = Donya::Vector2(64.0f * (GameTimer::GetInstance()->timer[0].Current() % 10), 896.0f);
	boss1TimeData[7].texSize = Donya::Vector2(64.0f, 64.0f);
	boss2TimeData[7].pos = Donya::Vector2(0.0f, 0.0f);
	boss2TimeData[7].texPos = Donya::Vector2(64.0f * (GameTimer::GetInstance()->timer[1].Current() % 10), 896.0f);
	boss2TimeData[7].texSize = Donya::Vector2(64.0f, 64.0f);
	boss3TimeData[7].pos = Donya::Vector2(0.0f, 0.0f);
	boss3TimeData[7].texPos = Donya::Vector2(64.0f * (GameTimer::GetInstance()->timer[2].Current() % 10), 896.0f);
	boss3TimeData[7].texSize = Donya::Vector2(64.0f, 64.0f);
	totalTimeData[7].pos = Donya::Vector2(0.0f, 0.0f);
	totalTimeData[7].texPos = Donya::Vector2(224.0f * (GameTimer::GetInstance()->totalTimer.Current() % 10), 420.0f);
	totalTimeData[7].texSize = Donya::Vector2(224.0f, 476.0f);

	boss1TimePos = Donya::Vector2(1920.0f, 222.0f);
	boss2TimePos = Donya::Vector2(1920.0f, 299.0f);
	boss3TimePos = Donya::Vector2(1920.0f, 376.0f);
	totalTimePos = Donya::Vector2(1920.0f, 500.0f);

	addW1 = 20.0f;
	addW2 = 100.0f;

	addW1_1 = 1.0f;
	addW2_1 = -10.0f;

	addW2_2 = 10.0f;

	rankingBackPos = Donya::Vector2(861.0f, 200.0f);
	rankingBackTexSize = Donya::Vector2(1054.0f, 104.0f);


	ranking1TimeData[0].pos = Donya::Vector2(0.0f, 0.0f);
	ranking1TimeData[0].texPos = Donya::Vector2(128.0f * (rankingTimer[0].minute / 10), 868.0f);
	ranking1TimeData[0].texSize = Donya::Vector2(128.0f, 128.0f);
	ranking2TimeData[0].pos = Donya::Vector2(0.0f, 0.0f);
	ranking2TimeData[0].texPos = Donya::Vector2(128.0f * (rankingTimer[1].minute / 10), 868.0f);
	ranking2TimeData[0].texSize = Donya::Vector2(128.0f, 128.0f);
	ranking3TimeData[0].pos = Donya::Vector2(0.0f, 0.0f);
	ranking3TimeData[0].texPos = Donya::Vector2(128.0f * (rankingTimer[2].minute / 10), 868.0f);
	ranking3TimeData[0].texSize = Donya::Vector2(128.0f, 128.0f);
	ranking4TimeData[0].pos = Donya::Vector2(0.0f, 0.0f);
	ranking4TimeData[0].texPos = Donya::Vector2(128.0f * (rankingTimer[3].minute / 10), 868.0f);
	ranking4TimeData[0].texSize = Donya::Vector2(128.0f, 128.0f);
	ranking5TimeData[0].pos = Donya::Vector2(0.0f, 0.0f);
	ranking5TimeData[0].texPos = Donya::Vector2(128.0f * (rankingTimer[4].minute / 10), 868.0f);
	ranking5TimeData[0].texSize = Donya::Vector2(128.0f, 128.0f);

	ranking1TimeData[1].pos = Donya::Vector2(0.0f, 0.0f);
	ranking1TimeData[1].texPos = Donya::Vector2(128.0f * (rankingTimer[0].minute % 10), 868.0f);
	ranking1TimeData[1].texSize = Donya::Vector2(128.0f, 128.0f);
	ranking2TimeData[1].pos = Donya::Vector2(0.0f, 0.0f);
	ranking2TimeData[1].texPos = Donya::Vector2(128.0f * (rankingTimer[1].minute % 10), 868.0f);
	ranking2TimeData[1].texSize = Donya::Vector2(128.0f, 128.0f);
	ranking3TimeData[1].pos = Donya::Vector2(0.0f, 0.0f);
	ranking3TimeData[1].texPos = Donya::Vector2(128.0f * (rankingTimer[2].minute % 10), 868.0f);
	ranking3TimeData[1].texSize = Donya::Vector2(128.0f, 128.0f);
	ranking4TimeData[1].pos = Donya::Vector2(0.0f, 0.0f);
	ranking4TimeData[1].texPos = Donya::Vector2(128.0f * (rankingTimer[3].minute % 10), 868.0f);
	ranking4TimeData[1].texSize = Donya::Vector2(128.0f, 128.0f);
	ranking5TimeData[1].pos = Donya::Vector2(0.0f, 0.0f);
	ranking5TimeData[1].texPos = Donya::Vector2(128.0f * (rankingTimer[4].minute % 10), 868.0f);
	ranking5TimeData[1].texSize = Donya::Vector2(128.0f, 128.0f);

	ranking1TimeData[2].pos = Donya::Vector2(0.0f, 0.0f);
	ranking1TimeData[2].texPos = Donya::Vector2(128.0f * 11, 868.0f);
	ranking1TimeData[2].texSize = Donya::Vector2(128.0f, 128.0f);
	ranking2TimeData[2].pos = Donya::Vector2(0.0f, 0.0f);
	ranking2TimeData[2].texPos = Donya::Vector2(128.0f * 11, 868.0f);
	ranking2TimeData[2].texSize = Donya::Vector2(128.0f, 128.0f);
	ranking3TimeData[2].pos = Donya::Vector2(0.0f, 0.0f);
	ranking3TimeData[2].texPos = Donya::Vector2(128.0f * 11, 868.0f);
	ranking3TimeData[2].texSize = Donya::Vector2(128.0f, 128.0f);
	ranking4TimeData[2].pos = Donya::Vector2(0.0f, 0.0f);
	ranking4TimeData[2].texPos = Donya::Vector2(128.0f * 11, 868.0f);
	ranking4TimeData[2].texSize = Donya::Vector2(128.0f, 128.0f);
	ranking5TimeData[2].pos = Donya::Vector2(0.0f, 0.0f);
	ranking5TimeData[2].texPos = Donya::Vector2(128.0f * 11, 868.0f);
	ranking5TimeData[2].texSize = Donya::Vector2(128.0f, 128.0f);

	ranking1TimeData[3].pos = Donya::Vector2(0.0f, 0.0f);
	ranking1TimeData[3].texPos = Donya::Vector2(128.0f * (rankingTimer[0].second / 10), 868.0f);
	ranking1TimeData[3].texSize = Donya::Vector2(128.0f, 128.0f);
	ranking2TimeData[3].pos = Donya::Vector2(0.0f, 0.0f);
	ranking2TimeData[3].texPos = Donya::Vector2(128.0f * (rankingTimer[1].second / 10), 868.0f);
	ranking2TimeData[3].texSize = Donya::Vector2(128.0f, 128.0f);
	ranking3TimeData[3].pos = Donya::Vector2(0.0f, 0.0f);
	ranking3TimeData[3].texPos = Donya::Vector2(128.0f * (rankingTimer[2].second / 10), 868.0f);
	ranking3TimeData[3].texSize = Donya::Vector2(128.0f, 128.0f);
	ranking4TimeData[3].pos = Donya::Vector2(0.0f, 0.0f);
	ranking4TimeData[3].texPos = Donya::Vector2(128.0f * (rankingTimer[3].second / 10), 868.0f);
	ranking4TimeData[3].texSize = Donya::Vector2(128.0f, 128.0f);
	ranking5TimeData[3].pos = Donya::Vector2(0.0f, 0.0f);
	ranking5TimeData[3].texPos = Donya::Vector2(128.0f * (rankingTimer[4].second / 10), 868.0f);
	ranking5TimeData[3].texSize = Donya::Vector2(128.0f, 128.0f);

	ranking1TimeData[4].pos = Donya::Vector2(0.0f, 0.0f);
	ranking1TimeData[4].texPos = Donya::Vector2(128.0f * (rankingTimer[0].second % 10), 868.0f);
	ranking1TimeData[4].texSize = Donya::Vector2(128.0f, 128.0f);
	ranking2TimeData[4].pos = Donya::Vector2(0.0f, 0.0f);
	ranking2TimeData[4].texPos = Donya::Vector2(128.0f * (rankingTimer[1].second % 10), 868.0f);
	ranking2TimeData[4].texSize = Donya::Vector2(128.0f, 128.0f);
	ranking3TimeData[4].pos = Donya::Vector2(0.0f, 0.0f);
	ranking3TimeData[4].texPos = Donya::Vector2(128.0f * (rankingTimer[2].second % 10), 868.0f);
	ranking3TimeData[4].texSize = Donya::Vector2(128.0f, 128.0f);
	ranking4TimeData[4].pos = Donya::Vector2(0.0f, 0.0f);
	ranking4TimeData[4].texPos = Donya::Vector2(128.0f * (rankingTimer[3].second % 10), 868.0f);
	ranking4TimeData[4].texSize = Donya::Vector2(128.0f, 128.0f);
	ranking5TimeData[4].pos = Donya::Vector2(0.0f, 0.0f);
	ranking5TimeData[4].texPos = Donya::Vector2(128.0f * (rankingTimer[4].second % 10), 868.0f);
	ranking5TimeData[4].texSize = Donya::Vector2(128.0f, 128.0f);

	ranking1TimeData[5].pos = Donya::Vector2(0.0f, 0.0f);
	ranking1TimeData[5].texPos = Donya::Vector2(128.0f * 11, 868.0f);
	ranking1TimeData[5].texSize = Donya::Vector2(128.0f, 128.0f);
	ranking2TimeData[5].pos = Donya::Vector2(0.0f, 0.0f);
	ranking2TimeData[5].texPos = Donya::Vector2(128.0f * 11, 868.0f);
	ranking2TimeData[5].texSize = Donya::Vector2(128.0f, 128.0f);
	ranking3TimeData[5].pos = Donya::Vector2(0.0f, 0.0f);
	ranking3TimeData[5].texPos = Donya::Vector2(128.0f * 11, 868.0f);
	ranking3TimeData[5].texSize = Donya::Vector2(128.0f, 128.0f);
	ranking4TimeData[5].pos = Donya::Vector2(0.0f, 0.0f);
	ranking4TimeData[5].texPos = Donya::Vector2(128.0f * 11, 868.0f);
	ranking4TimeData[5].texSize = Donya::Vector2(128.0f, 128.0f);
	ranking5TimeData[5].pos = Donya::Vector2(0.0f, 0.0f);
	ranking5TimeData[5].texPos = Donya::Vector2(128.0f * 11, 868.0f);
	ranking5TimeData[5].texSize = Donya::Vector2(128.0f, 128.0f);

	ranking1TimeData[6].pos = Donya::Vector2(0.0f, 0.0f);
	ranking1TimeData[6].texPos = Donya::Vector2(128.0f * (rankingTimer[0].current / 10), 868.0f);
	ranking1TimeData[6].texSize = Donya::Vector2(128.0f, 128.0f);
	ranking2TimeData[6].pos = Donya::Vector2(0.0f, 0.0f);
	ranking2TimeData[6].texPos = Donya::Vector2(128.0f * (rankingTimer[1].current / 10), 868.0f);
	ranking2TimeData[6].texSize = Donya::Vector2(128.0f, 128.0f);
	ranking3TimeData[6].pos = Donya::Vector2(0.0f, 0.0f);
	ranking3TimeData[6].texPos = Donya::Vector2(128.0f * (rankingTimer[2].current / 10), 868.0f);
	ranking3TimeData[6].texSize = Donya::Vector2(128.0f, 128.0f);
	ranking4TimeData[6].pos = Donya::Vector2(0.0f, 0.0f);
	ranking4TimeData[6].texPos = Donya::Vector2(128.0f * (rankingTimer[3].current / 10), 868.0f);
	ranking4TimeData[6].texSize = Donya::Vector2(128.0f, 128.0f);
	ranking5TimeData[6].pos = Donya::Vector2(0.0f, 0.0f);
	ranking5TimeData[6].texPos = Donya::Vector2(128.0f * (rankingTimer[4].current / 10), 868.0f);
	ranking5TimeData[6].texSize = Donya::Vector2(128.0f, 128.0f);

	ranking1TimeData[7].pos = Donya::Vector2(0.0f, 0.0f);
	ranking1TimeData[7].texPos = Donya::Vector2(128.0f * (rankingTimer[0].current % 10), 868.0f);
	ranking1TimeData[7].texSize = Donya::Vector2(128.0f, 128.0f);
	ranking2TimeData[7].pos = Donya::Vector2(0.0f, 0.0f);
	ranking2TimeData[7].texPos = Donya::Vector2(128.0f * (rankingTimer[1].current % 10), 868.0f);
	ranking2TimeData[7].texSize = Donya::Vector2(128.0f, 128.0f);
	ranking3TimeData[7].pos = Donya::Vector2(0.0f, 0.0f);
	ranking3TimeData[7].texPos = Donya::Vector2(128.0f * (rankingTimer[2].current % 10), 868.0f);
	ranking3TimeData[7].texSize = Donya::Vector2(128.0f, 128.0f);
	ranking4TimeData[7].pos = Donya::Vector2(0.0f, 0.0f);
	ranking4TimeData[7].texPos = Donya::Vector2(128.0f * (rankingTimer[3].current % 10), 868.0f);
	ranking4TimeData[7].texSize = Donya::Vector2(128.0f, 128.0f);
	ranking5TimeData[7].pos = Donya::Vector2(0.0f, 0.0f);
	ranking5TimeData[7].texPos = Donya::Vector2(128.0f * (rankingTimer[4].current % 10), 868.0f);
	ranking5TimeData[7].texSize = Donya::Vector2(128.0f, 128.0f);

	ranking1TimePos = Donya::Vector2(1920.0f, 200.0f + 125.0f * 0.0f);
	ranking2TimePos = Donya::Vector2(1920.0f, 200.0f + 125.0f * 1.0f);
	ranking3TimePos = Donya::Vector2(1920.0f, 200.0f + 125.0f * 2.0f);
	ranking4TimePos = Donya::Vector2(1920.0f, 200.0f + 125.0f * 3.0f);
	ranking5TimePos = Donya::Vector2(1920.0f, 200.0f + 125.0f * 4.0f);

	addW1_r = 50.0f;
	addW2_r = 9.0f;

	continuity.pos = Donya::Vector2(1920.0f - 434.0f - 100.0f - 434.0f, 1080.0f - 114.0f - 100.0f);
	continuity.texPos = Donya::Vector2(0.0f, 0.0f);
	continuity.texSize = Donya::Vector2(434.0f, 114.0f);

	toTitle.pos = Donya::Vector2(1920.0f - 434.0f - 50.0f, 1080.0f - 114.0f - 100.0f);
	toTitle.texPos = Donya::Vector2(434.0f, 113.0f);
	toTitle.texSize = Donya::Vector2(434.0f, 114.0f);

	Donya::Sound::Play(GAMECLEAR);
}

void SceneResult::update()
{
	GameLib::camera::setPos(DirectX::XMFLOAT3(0.0f, 0.0f, 100.0f));
	GameLib::camera::setTarget(Donya::Vector3(0.0f, 0.0f, 0.0f));

	Fade::GetInstance()->Update();
	if (Fade::GetInstance()->GetExist()) return;

	switch (state)
	{
	case 0:
		if (MAX_CNT <= cnt)
		{
			cnt = 0;
			state++;
		}
		else
		{
			boss1TimePos.x = easing::OutExp((float)cnt, MAX_CNT, 1590.0f, 1920.0f);
			cnt++;
		}
		break;
	case 1:
		if (MAX_CNT <= cnt)
		{
			cnt = 0;
			state++;
		}
		else
		{
			boss2TimePos.x = easing::OutExp((float)cnt, MAX_CNT, 1590.0f, 1920.0f);
			cnt++;
		}
		break;
	case 2:
		if (MAX_CNT <= cnt)
		{
			cnt = 0;
			state++;
		}
		else
		{
			boss3TimePos.x = easing::OutExp((float)cnt, MAX_CNT, 1590.0f, 1920.0f);
			cnt++;
		}
		break;
	case 3:
		if (MAX_CNT <= cnt)
		{
			cnt = 0;
			state++;
		}
		else
		{
			totalTimePos.x = easing::OutExp((float)cnt, MAX_CNT, 1010.0f, 1920.0f);
			cnt++;
		}
		break;
	case 4:
		if (Donya::Keyboard::Trigger('A') || GameLib::input::xInput::pressedButtons(0, XboxPad_Button::A) == 1)
		{
			cnt = 0;
			state++;
		}
		break;
	case 5:
		if (MAX_CNT <= cnt)
		{
			cnt = 0;
			state++;
		}
		else
		{
			textData.texPos = Donya::Vector2(1110.0f, 0.0f);
			textData.texSize = Donya::Vector2(1110.0f, 1080.0f);
			cnt++;
		}
		break;
	case 6:
		if (MAX_CNT <= cnt)
		{
			cnt = 0;
			state++;
		}
		else
		{
			ranking1TimePos.x = easing::OutExp((float)cnt, MAX_CNT, 1200.0f, 1920.0f);
			cnt++;
		}
		break;
	case 7:
		if (MAX_CNT <= cnt)
		{
			cnt = 0;
			state++;
		}
		else
		{
			ranking2TimePos.x = easing::OutExp((float)cnt, MAX_CNT, 1200.0f, 1920.0f);
			cnt++;
		}
		break;
	case 8:
		if (MAX_CNT <= cnt)
		{
			cnt = 0;
			state++;
		}
		else
		{
			ranking3TimePos.x = easing::OutExp((float)cnt, MAX_CNT, 1200.0f, 1920.0f);
			cnt++;
		}
		break;
	case 9:
		if (MAX_CNT <= cnt)
		{
			cnt = 0;
			state++;
		}
		else
		{
			ranking4TimePos.x = easing::OutExp((float)cnt, MAX_CNT, 1200.0f, 1920.0f);
			cnt++;
		}
		break;
	case 10:
		if (MAX_CNT <= cnt)
		{
			cnt = 0;
			state++;
		}
		else
		{
			ranking5TimePos.x = easing::OutExp((float)cnt, MAX_CNT, 1200.0f, 1920.0f);
			cnt++;
		}
		break;
	default: break;
	}

	if (state == 11)
	{
		using namespace GameLib::input;
		constexpr int   PAD_NO = 0;
		constexpr float STICK_RANGE_MAX = 32768.0f;
		const auto leftStick = xInput::getThumbL(PAD_NO);
		if (leftStick.x < 0 || xInput::pressedButtons(0, XboxPad_Button::DPAD_LEFT))
		{
			lastState = 0;
			continuity.texPos.x = 0.0f;
			toTitle.texPos.x = 434.0f;
		}
		if (0 < leftStick.x || xInput::pressedButtons(0, XboxPad_Button::DPAD_RIGHT))
		{
			lastState = 1;
			continuity.texPos.x = 434.0f;
			toTitle.texPos.x = 0.0f;
		}

		constexpr int TRIGGER_FLAG = 1;
		if (TRIGGER_FLAG == xInput::pressedButtons(PAD_NO, XboxPad_Button::A))
		{
			if (lastState == 0) Fade::GetInstance()->Init(1);
			else if (lastState == 1) Fade::GetInstance()->Init(0);
			cnt = 0;
			state++;
		}
	}

	/*if (Donya::Keyboard::Trigger('T') || GameLib::input::xInput::pressedButtons(0, XboxPad_Button::X) == 1)
	{
		pSceneManager->setNextScene(new sceneTitle, false);
	}*/
}

void SceneResult::render()
{
	clearWindow(0.5f, 0.5f, 0.5f, 1.0f);

	DirectX::XMFLOAT4X4 viewProjection;
	DirectX::XMStoreFloat4x4(&viewProjection, getViewMatrix() * getProjectionMatrix());

	setBlendMode_NONE(1.0f);

	spriteRenderRect(&testBack, Donya::Vector2(0.0f,0.0f), Donya::Vector2(0.0f, 0.0f), Donya::Vector2(1920.0f, 1080.0f));
	spriteRenderRect(&back, backData.pos, backData.texPos, backData.texSize);
	spriteRenderRect(&text, textData.pos, textData.texPos, textData.texSize);

	if (state <= 4)
	{
		spriteRenderRect(&result, Donya::Vector2(bossBackData[0].pos.x, 209.0f), bossBackData[0].texPos, bossBackData[0].texSize);
		spriteRenderRect(&result, Donya::Vector2(bossBackData[1].pos.x, 285.0f), bossBackData[1].texPos, bossBackData[1].texSize);
		spriteRenderRect(&result, Donya::Vector2(bossBackData[2].pos.x, 366.0f), bossBackData[2].texPos, bossBackData[2].texSize);
		if (state == 4)
		{
			spriteRenderRect(&result, Donya::Vector2(1920.0f - 340.0f, 1080.0f - 156.0f), Donya::Vector2(0.0f, 264.0f), Donya::Vector2(340.0f, 156.0f));
		}

		for (int i = 0; i < 8; i++)
		{
			if (i == 4 || i == 7)
			{
				spriteRenderRect(&result, Donya::Vector2(boss1TimePos.x + addW1 * i + addW1_1, boss1TimePos.y), boss1TimeData[i].texPos, boss1TimeData[i].texSize);
				spriteRenderRect(&result, Donya::Vector2(boss2TimePos.x + addW1 * i + addW1_1, boss2TimePos.y), boss2TimeData[i].texPos, boss2TimeData[i].texSize);
				spriteRenderRect(&result, Donya::Vector2(boss3TimePos.x + addW1 * i + addW1_1, boss3TimePos.y), boss3TimeData[i].texPos, boss3TimeData[i].texSize);
				spriteRenderRect(&result, Donya::Vector2(totalTimePos.x + addW2 * i + addW2_1, totalTimePos.y), totalTimeData[i].texPos, totalTimeData[i].texSize);
			}
			else if (i == 2)
			{
				spriteRenderRect(&result, Donya::Vector2(boss1TimePos.x + addW1 * i, boss1TimePos.y), boss1TimeData[i].texPos, boss1TimeData[i].texSize);
				spriteRenderRect(&result, Donya::Vector2(boss2TimePos.x + addW1 * i, boss2TimePos.y), boss2TimeData[i].texPos, boss2TimeData[i].texSize);
				spriteRenderRect(&result, Donya::Vector2(boss3TimePos.x + addW1 * i, boss3TimePos.y), boss3TimeData[i].texPos, boss3TimeData[i].texSize);
				spriteRenderRect(&result, Donya::Vector2(totalTimePos.x + addW2 * i + 16.0f, totalTimePos.y), totalTimeData[i].texPos, totalTimeData[i].texSize);
			}
			else if (i == 5)
			{
				spriteRenderRect(&result, Donya::Vector2(boss1TimePos.x + addW1 * i, boss1TimePos.y), boss1TimeData[i].texPos, boss1TimeData[i].texSize);
				spriteRenderRect(&result, Donya::Vector2(boss2TimePos.x + addW1 * i, boss2TimePos.y), boss2TimeData[i].texPos, boss2TimeData[i].texSize);
				spriteRenderRect(&result, Donya::Vector2(boss3TimePos.x + addW1 * i, boss3TimePos.y), boss3TimeData[i].texPos, boss3TimeData[i].texSize);
				spriteRenderRect(&result, Donya::Vector2(totalTimePos.x + addW2 * i + 13.0f, totalTimePos.y), totalTimeData[i].texPos, totalTimeData[i].texSize);
			}
			else
			{
				spriteRenderRect(&result, Donya::Vector2(boss1TimePos.x + addW1 * i, boss1TimePos.y), boss1TimeData[i].texPos, boss1TimeData[i].texSize);
				spriteRenderRect(&result, Donya::Vector2(boss2TimePos.x + addW1 * i, boss2TimePos.y), boss2TimeData[i].texPos, boss2TimeData[i].texSize);
				spriteRenderRect(&result, Donya::Vector2(boss3TimePos.x + addW1 * i, boss3TimePos.y), boss3TimeData[i].texPos, boss3TimeData[i].texSize);
				spriteRenderRect(&result, Donya::Vector2(totalTimePos.x + addW2 * i, totalTimePos.y), totalTimeData[i].texPos, totalTimeData[i].texSize);
			}
		}
	}
	else
	{
		spriteRenderRect(&ranking, Donya::Vector2(861.0f, 200.0f + 125.0f * 0.0f), Donya::Vector2(0.0f, 228.0f + 128.0f * 0.0f), Donya::Vector2(1054.0f, 128.0f));
		spriteRenderRect(&ranking, Donya::Vector2(861.0f, 200.0f + 125.0f * 1.0f), Donya::Vector2(0.0f, 228.0f + 128.0f * 1.0f), Donya::Vector2(1054.0f, 128.0f));
		spriteRenderRect(&ranking, Donya::Vector2(861.0f, 200.0f + 125.0f * 2.0f), Donya::Vector2(0.0f, 228.0f + 128.0f * 2.0f), Donya::Vector2(1054.0f, 128.0f));
		spriteRenderRect(&ranking, Donya::Vector2(861.0f, 200.0f + 125.0f * 3.0f), Donya::Vector2(0.0f, 228.0f + 128.0f * 3.0f), Donya::Vector2(1054.0f, 128.0f));
		spriteRenderRect(&ranking, Donya::Vector2(861.0f, 200.0f + 125.0f * 4.0f), Donya::Vector2(0.0f, 228.0f + 128.0f * 4.0f), Donya::Vector2(1054.0f, 128.0f));

		for (int i = 0; i < 8; i++)
		{
			if (i == 2 || i == 5)
			{
				spriteRenderRect(&ranking, Donya::Vector2(ranking1TimePos.x + addW1_r * i + addW2_r, ranking1TimePos.y), ranking1TimeData[i].texPos, ranking1TimeData[i].texSize);
				spriteRenderRect(&ranking, Donya::Vector2(ranking2TimePos.x + addW1_r * i + addW2_r, ranking2TimePos.y), ranking2TimeData[i].texPos, ranking2TimeData[i].texSize);
				spriteRenderRect(&ranking, Donya::Vector2(ranking3TimePos.x + addW1_r * i + addW2_r, ranking3TimePos.y), ranking3TimeData[i].texPos, ranking3TimeData[i].texSize);
				spriteRenderRect(&ranking, Donya::Vector2(ranking4TimePos.x + addW1_r * i + addW2_r, ranking4TimePos.y), ranking4TimeData[i].texPos, ranking4TimeData[i].texSize);
				spriteRenderRect(&ranking, Donya::Vector2(ranking5TimePos.x + addW1_r * i + addW2_r, ranking5TimePos.y), ranking5TimeData[i].texPos, ranking5TimeData[i].texSize);
			}
			else
			{
				spriteRenderRect(&ranking, Donya::Vector2(ranking1TimePos.x + addW1_r * i, ranking1TimePos.y), ranking1TimeData[i].texPos, ranking1TimeData[i].texSize);
				spriteRenderRect(&ranking, Donya::Vector2(ranking2TimePos.x + addW1_r * i, ranking2TimePos.y), ranking2TimeData[i].texPos, ranking2TimeData[i].texSize);
				spriteRenderRect(&ranking, Donya::Vector2(ranking3TimePos.x + addW1_r * i, ranking3TimePos.y), ranking3TimeData[i].texPos, ranking3TimeData[i].texSize);
				spriteRenderRect(&ranking, Donya::Vector2(ranking4TimePos.x + addW1_r * i, ranking4TimePos.y), ranking4TimeData[i].texPos, ranking4TimeData[i].texSize);
				spriteRenderRect(&ranking, Donya::Vector2(ranking5TimePos.x + addW1_r * i, ranking5TimePos.y), ranking5TimeData[i].texPos, ranking5TimeData[i].texSize);
			}
		}

		if (11 <= state)
		{
			spriteRenderRect(&ranking, continuity.pos, continuity.texPos, continuity.texSize);
			spriteRenderRect(&ranking, toTitle.pos, toTitle.texPos, toTitle.texSize);
		}
	}
	setBlendMode_ALPHA(1.0f);

	Fade::GetInstance()->Draw();
}

void SceneResult::uninit()
{

}

void SceneResult::LoadParameter(bool isBinary)
{
	Donya::Serializer::Extension ext = (isBinary)
		? Donya::Serializer::Extension::BINARY
		: Donya::Serializer::Extension::JSON;
	std::string filePath = GenerateSerializePath(SERIAL_ID, ext);

	Donya::Serializer seria;
	seria.Load(ext, filePath.c_str(), SERIAL_ID, *this);
}


void SceneResult::imGui()
{
#if USE_IMGUI

	if ( ImGui::BeginIfAllowed( "Result" ) )
	{
		ImGui::Text("ranking");
		ImGui::Text("1 : %d", rankingScore[0]);
		ImGui::Text("2 : %d", rankingScore[1]);
		ImGui::Text("3 : %d", rankingScore[2]);
		ImGui::Text("4 : %d", rankingScore[3]);
		ImGui::Text("5 : %d", rankingScore[4]);

		ImGui::Text(" ");

		if ( ImGui::Button( "GoTo Title" ) )
		{
			pSceneManager->setNextScene( new sceneTitle(), false );
		}
		ImGui::Text(" ");

		ImGui::Text("Your stage 1 Time : %d, %d : %d : %d", GameTimer::GetInstance()->timer[0].AllCurren(), GameTimer::GetInstance()->timer[0].Minute(), GameTimer::GetInstance()->timer[0].Second(), GameTimer::GetInstance()->timer[0].Current());
		ImGui::Text("Your stage 2 Time : %d, %d : %d : %d", GameTimer::GetInstance()->timer[1].AllCurren(), GameTimer::GetInstance()->timer[1].Minute(), GameTimer::GetInstance()->timer[1].Second(), GameTimer::GetInstance()->timer[1].Current());
		ImGui::Text("Your stage 3 Time : %d, %d : %d : %d", GameTimer::GetInstance()->timer[2].AllCurren(), GameTimer::GetInstance()->timer[2].Minute(), GameTimer::GetInstance()->timer[2].Second(), GameTimer::GetInstance()->timer[2].Current());
		ImGui::Text("Your Total Time : %d, %d : %d : %d", yourScore, GameTimer::GetInstance()->totalTimer.Minute(), GameTimer::GetInstance()->totalTimer.Second(), GameTimer::GetInstance()->totalTimer.Current());

		if (ImGui::TreeNode("BillBoard Test Draw Parametor"))
		{
			/*ImGui::DragFloat3("pos",		&data[0].pos.x);
			ImGui::DragFloat2("scale",		&data[0].scale.x);
			ImGui::DragFloat2("texPos",		&data[0].texPos.x);
			ImGui::DragFloat2("texSize",	&data[0].texSize.x);
			ImGui::Text(" ");
			ImGui::DragFloat3("pos ",		&data[1].pos.x);
			ImGui::DragFloat2("scale ",		&data[1].scale.x);
			ImGui::DragFloat2("texPos ",	&data[1].texPos.x);
			ImGui::DragFloat2("texSize ",	&data[1].texSize.x);*/

			/*ImGui::DragFloat3("rankingPos", &rankingPos.x);
			ImGui::DragFloat3("yourScorePos", &yourScorePos.x);
			ImGui::DragFloat2("rankingWidth", &rankingWidth.x);*/

			ImGui::DragFloat2("boss1TimePos", &boss1TimePos.x);
			ImGui::DragFloat2("boss2TimePos", &boss2TimePos.x);
			ImGui::DragFloat2("boss3TimePos", &boss3TimePos.x);
			ImGui::DragFloat2("totalTimePos", &totalTimePos.x);

			ImGui::DragFloat("addW1", &addW1);
			ImGui::DragFloat("addW2", &addW2);

			ImGui::DragFloat("addW1_1", &addW1_1);
			ImGui::DragFloat("addW2_1", &addW2_1);
			ImGui::DragFloat("addW2_2", &addW2_2);

			ImGui::DragFloat2("rankingBackPos", &rankingBackPos.x);
			ImGui::DragFloat2("rankingBackTexPos", &rankingBackTexPos.x);
			ImGui::DragFloat2("rankingBackTexSize", &rankingBackTexSize.x);

			ImGui::DragFloat("addW1_r", &addW1_r);
			ImGui::DragFloat("addW2_r", &addW2_r);

			ImGui::TreePop();
		}

		ImGui::End();
	}

#endif // USE_IMGUI
}

void SceneResult::SaveParameter()
{
	Donya::Serializer::Extension bin = Donya::Serializer::Extension::BINARY;
	Donya::Serializer::Extension json = Donya::Serializer::Extension::JSON;
	std::string binPath = GenerateSerializePath(SERIAL_ID, bin);
	std::string jsonPath = GenerateSerializePath(SERIAL_ID, json);

	Donya::Serializer seria;
	seria.Save(bin, binPath.c_str(), SERIAL_ID, *this);
	seria.Save(json, jsonPath.c_str(), SERIAL_ID, *this);
}
