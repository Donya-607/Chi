#pragma once
#include "scene.h"
#include "sceneManager.h"
#include "baseScene.h"


class Fade
{
private:
	bool exist; // fade��
	float alpha;
	int state;
	int sceneNo;

	/*Sprite sprite;

	struct SpriteData
	{
		Donya::Vector2 pos;
		Donya::Vector2 texPos;
		Donya::Vector2 texSize;
	};
	SpriteData back;*/

public:
	Fade() : exist(false), alpha(0.0f), state(0), sceneNo(0) {}
	~Fade() {}

	static Fade* GetInstance()
	{
		static Fade instance;
		return &instance;
	}

	// 0 : �^�C�g��, 1 : �Q�[��, 2 : �Q�[���I�[�o�[, 3 : ���U���g
	void Init(int _sceneNo);
	void Update();
	void Draw();
	bool GetExist() { return exist; }

	void ChangeScene(int sceneNo)
	{
		/*if (sceneNo == 0) pSceneManager->setNextScene(new sceneTitle(), false);
		else if (sceneNo == 1) pSceneManager->setNextScene(new SceneGame(), false);
		else if (sceneNo == 2) pSceneManager->setNextScene(new SceneGameOver(), false);
		else if (sceneNo == 3) pSceneManager->setNextScene(new SceneResult(), false);*/
	}
};