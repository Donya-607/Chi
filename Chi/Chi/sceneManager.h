#ifndef SCENEMANAGER_H_
#define SCENEMANAGER_H_

#include <vector>
#include "baseScene.h"
#include "imgui.h"

class sceneManager
{
private:
	baseScene* nextScene;
	bool isStack;
	bool delete_stack;
	sceneManager() {}
	sceneManager(sceneManager& copy) {}
	~sceneManager() {}
	std::vector<baseScene*> activeScene;
public:
	void init(baseScene* firstScene);
	void update();
	void render();
	void setNextScene(baseScene* _nextScene, bool stack);
	void changeScene();
	void deleteStack();
	void deleteNowScene();
	void deleteStackScene();
	void end();
	static sceneManager* getInstance()
	{
		static sceneManager instance;
		return &instance;
	}
};


//stack”Å‚Ìˆ—‚ªv‚¢‚Â‚­‚Ü‚Å‚±‚Á‚¿
class sceneManagerBeta
{
private:
	sceneManagerBeta() {}
	~sceneManagerBeta() {}
	baseScene* nowScene;
	baseScene* nextScene;
public:
	void init(baseScene* _firstScene);
	void update();
	void render();
	void setNextScene(baseScene* _nextScene);
	void end();
	static sceneManagerBeta* getInstance()
	{
		static sceneManagerBeta instance;
		return &instance;
	}

};
#define pSceneManager sceneManager::getInstance()
#endif SCENEMANAGER_H_