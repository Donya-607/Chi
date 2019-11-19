#include "Fade.h"
#include "GameLibFunctions.h"


void Fade::Init(int _sceneNo)
{
	exist = true;
	alpha = 0.0f;
	state = 0;
	sceneNo = _sceneNo;

	/*spriteLoad(&sprite, L"./Data/Images/UI/blackback.png");

	back.pos = Donya::Vector2(0.0f, 0.0f);
	back.texPos = Donya::Vector2(0.0f, 228.0f);
	back.texSize = Donya::Vector2(658.0f, 218.0f);*/
}
void Fade::Update()
{
	if (exist)
	{
		if (state == 0)
		{
			alpha += 0.01f;
			if (1.0f <= alpha)
			{
				GameLib::setFlameSpeed(1.0f);

				alpha = 1.0f;
				state = 1;
				if (sceneNo == 0) pSceneManager->setNextScene(new sceneTitle(), false);
				else if (sceneNo == 1) pSceneManager->setNextScene(new SceneGame(), false);
				else if (sceneNo == 2) pSceneManager->setNextScene(new SceneGameOver(), false);
				else if (sceneNo == 3) pSceneManager->setNextScene(new SceneResult(), false);
			}
		}
		else if (state == 1)
		{
			alpha -= 0.01f;
			if (alpha <= 0.0f)
			{
				alpha = 0.0f;
				exist = false;
			}
		}
	}
}
void Fade::Draw()
{
	if (exist)
	{
		setBlendMode_ALPHA(alpha);
		drawRect(Donya::Vector2(0.0f, 0.0f), Donya::Vector2(1920.0f, 1080.0f), Donya::Vector2(0.0f, 0.0f), 0.0f, Donya::Vector3(0.0f, 0.0f, 0.0f));
		//spriteRenderRect(&sprite, back.pos, back.texPos, back.texSize);
		setBlendMode_ALPHA(1.0f);
	}
}