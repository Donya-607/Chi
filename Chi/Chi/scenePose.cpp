#include "scene.h"
#include "sceneManager.h"

void scenePose::init()
{
	isStack = false;

}

void scenePose::update()
{
	if (GetAsyncKeyState('G') < 0)
		pSceneManager->deleteNowScene();

}

void scenePose::render()
{
	setBlendMode_ALPHA(0.4f);

	drawRect({ 0,0 }, { 1920,1080 }, { 0,0 }, 0, { 0,0,0});

	setBlendMode_NONE(1.0f);
}

void scenePose::uninit()
{
}

void scenePose::imGui()
{

}