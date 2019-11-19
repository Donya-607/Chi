#include "scene.h"
#include "sceneManager.h"

void scenePose::init()
{
	isStack = false;
	spriteLoad(&texture[0], L"./Data/Images/Pause/Pause.png");
	spriteLoad(&texture[1], L"./Data/Images/Pause/PauseFlame.png");
	timer = 0;
	pos[0] = { 1920,0 };//上フレーム
	pos[1] = { -1920,0 };//下フレーム
	pos[2] = { 0,-1080 };//左フレーム
	pos[3] = { 0,1080 };//右フレーム
	verocity[0] = { 0,0 };
	verocity[1] = { 0,0 };
	verocity[2] = { 0,0 };
	verocity[3] = { 0,0 };
	flg[0] = false;
	flg[1] = false;
	flg[2] = false;
	flg[3] = false;
	cursol = 0;
	accelerator = { 0.1f, 0.05f };
	
}

void scenePose::update()
{
	timer++;

	//枠移動
	{
		if (timer == 10)
			flg[0] = true;
		if (flg[0])
		{
			verocity[0].x -= accelerator.x;
			verocity[1].x += accelerator.x;

			pos[0].x += verocity[0].x;
			pos[1].x += verocity[1].x;

			if (pos[0].x <= 0)
			{
				pos[0].x = 0;
				pos[1].x = 0;
				flg[0] = false;
				flg[1] = true;
			}
		}
		if (flg[1])
		{
			verocity[0].y += accelerator.y;
			verocity[1].y -= accelerator.y;

			pos[2].y += verocity[0].y;
			pos[3].y += verocity[1].y;

			if (pos[3].y <= 0)
			{
				pos[2].y = 0;
				pos[3].y = 0;
				flg[1] = false;
			}
		}
	}

	//ボタン出現
	if (timer > 30 && !flg[0] && !flg[1])
		flg[2] = true;

	//演出終了から決定まで
	if (flg[2])
	{
#if DEBUG_MODE
		if (getKeyState(KEY_INPUT_UP) == 1)
		{
			cursol--;
			if (cursol < 0)
				cursol = 2;
		}
		else if (getKeyState(KEY_INPUT_DOWN) == 1)
		{
			cursol++;
			if (cursol > 2)
				cursol = 0;
		}

		if (getKeyState(KEY_INPUT_SPACE) == 1)
		{
			flg[2] = false;
			flg[3] = true;
		}
#endif
		if (getKeyState(KEY_INPUT_UP) == 1)
		{
			cursol--;
			if (cursol < 0)
				cursol = 2;
		}
		else if (getKeyState(KEY_INPUT_DOWN) == 1)
		{
			cursol++;
			if (cursol > 2)
				cursol = 0;
		}

		if (pressedButtons(0,A) == 1)
		{
			flg[2] = false;
			flg[3] = true;
		}
		if (pressedButtons(0, START) == 1)
		{
			pSceneManager->deleteNowScene();
			return;
		}
	}

	//終わり枠移動
	if (flg[3])
	{
		switch (cursol)
		{
		case 0:
			pSceneManager->deleteNowScene();
			break;

		case 1:
			GameTimer::GetInstance()->Init();
			pSceneManager->setNextScene(new SceneGame, false);
			break;

		case 2:
			pSceneManager->setNextScene(new sceneTitle, false);
			break;
		}

	}
}

void scenePose::render()
{
	resetRendertarget();
	setBlendMode_ALPHA(1);
	//枠
	{
		spriteRenderRect(&texture[1], pos[2], { 0,2120 }, { 1920,1080 });
		spriteRenderRect(&texture[1], pos[3], { 0,3240 }, { 1920,1080 });
		spriteRenderRect(&texture[1], pos[0], { 0,0 }, { 1920,1080 });
		spriteRenderRect(&texture[1], pos[1], { 0,1080 }, { 1920,1080 });
	}

	//他
	if (flg[2])
	{
		spriteRenderRect(&texture[0], { 625,180 }, { 0,342 }, { 630,270 });
		switch (cursol)
		{
		case 0:
			spriteRenderRect(&texture[0], { 733,450 }, { 0 ,0 }, { 434,114 });
			spriteRenderRect(&texture[0], { 733,594 }, { 434 ,114 }, { 434,114 });
			spriteRenderRect(&texture[0], { 733,738 }, { 434 ,228 }, { 434,114 });
			break;

		case 1:
			spriteRenderRect(&texture[0], { 733,450 }, { 434 ,0 }, { 434,114 });
			spriteRenderRect(&texture[0], { 733,594 }, { 0 ,114 }, { 434,114 });
			spriteRenderRect(&texture[0], { 733,738 }, { 434 ,228 }, { 434,114 });
			break;

		case 2:
			spriteRenderRect(&texture[0], { 733,450 }, { 434 ,0 }, { 434,114 });
			spriteRenderRect(&texture[0], { 733,594 }, { 434 ,114 }, { 434,114 });
			spriteRenderRect(&texture[0], { 733,738 }, { 0 ,228 }, { 434,114 });
			break;

		}
	}

}

void scenePose::uninit()
{
}

void scenePose::imGui()
{

}