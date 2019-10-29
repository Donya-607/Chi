#ifndef SCENE_H_
#define SCENE_H_

#include <Windows.h>	// Insert by Donya, must include this before "Xinput.h".
#include <Xinput.h>

#include "baseScene.h"
#include "testObj.h"
#include "floor.h"

#include "rayPick.h"

#include "Sprite.h"
#include "Particle.h"
#include "BossAI.h"

using namespace DirectX;
class sceneTitle : public baseScene
{
private:
	Ray pl_down_ray;
	XMFLOAT4X4 World[2], world_view_projection[2];
	float camPos[3] = { 0,6,-10 };
	bool flg;
	XMFLOAT3 pos;
	skinned_mesh test;
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

class sceneLightting : public baseScene
{
private:
	ID3D11RenderTargetView* renderTarget;
	ID3D11ShaderResourceView* shaderResource;
	Sprite texture;
	testOBJ model;
	Floor ground;
	Sprite test;
	skinned_mesh test2;
	bool enable = true;
	float camPos[3] = { 0,6,-10 };
	float lightAmbient[3];
	float lightColor[3];
	PointLight pointLight[5];
	int light_index;
public:
	sceneLightting() {}
	~sceneLightting()
	{
		uninit();
	}
	void init();
	void update();
	void render();
	void uninit();
	void imGui();
	void testOBJImGui();
	bool saveLightInfo();
	void loadLightInfo();
};

class sceneShadow : public baseScene
{
private:
	ID3D11RenderTargetView* renderTarget;
	ID3D11ShaderResourceView* shaderResource;
	testOBJ2 model;
	Floor2 ground;
	bool enable = false;
	float camPos[3] = { 0,6,-10 };
	float lightDirection[3] = { 0,-6,10 };
	float lightPos[3] = { 0,6,-10 };
public:
	sceneShadow() {}
	~sceneShadow()
	{
		uninit();
	}
	void init();
	void update();
	void render();
	void uninit();
	void imGui();
	bool saveLightInfo();
	void loadLightInfo();
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

class SceneEffect : public baseScene
{
private:
	FlashParticle flashParticle;
	BubbleParticle bubbleParticle;
	BossAI bossAI;
	int keyCnt;

public:
	SceneEffect() {}
	~SceneEffect()
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
