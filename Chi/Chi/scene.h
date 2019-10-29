#ifndef SCENE_H_
#define SCENE_H_

#include <Windows.h>	// Insert by Donya, must include this before "Xinput.h".
#include <Xinput.h>

#include "baseScene.h"

#include "rayPick.h"

#include "Sprite.h"

using namespace DirectX;
class sceneTitle : public baseScene
{
private:
	Ray pl_down_ray;
	XMFLOAT4X4 World[2], world_view_projection[2];
	float camPos[3] = { 0,6,-10 };
	bool flg;
	XMFLOAT3 pos;
public:
	sceneTitle() {}
	~sceneTitle()
	{
		uninit();
	}
	void init();
	void update();
	void render();
	void uninit();
	void imGui();
};

#include <memory>
/// <summary>
/// By Donya.
/// </summary>
class SceneGame : public baseScene
{
public:
	struct Impl;
private:
	std::unique_ptr<Impl> pImpl;

public:
	SceneGame();
	~SceneGame();
public:
	void init()   override;
	void update() override;
	void render() override;
	void uninit() override;
	void imGui()  override;
};


class scenePose : public baseScene
{
private:

public:
	scenePose() {}
	~scenePose()
	{
		uninit();
	}
	void init();
	void update();
	void render();
	void uninit();
	void imGui();

};

#endif // !SCENE_H_
