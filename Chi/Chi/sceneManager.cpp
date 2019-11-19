#include "sceneManager.h"

#include "Donya/Keyboard.h"
#include "Donya/Sound.h"

void sceneManager::init(baseScene* firstScene)
{
	if (!activeScene.empty())
	{
		activeScene.clear();
	}
	activeScene.push_back(firstScene);
	activeScene.back()->init();
	nextScene = nullptr;
	delete_stack = false;
}

void sceneManager::update()
{
	Donya::Keyboard::Update();	// Insert by Donya.
	Donya::Sound::Update();		// Insert by Donya.

	if (nextScene)
		changeScene();
	if (delete_stack)
		deleteStackScene();
	for (auto& p : activeScene)
	{
		if (p->getIsStack())
			continue;
		p->imGui();
		p->update();
	}
}

void sceneManager::render()
{
	for (auto& p : activeScene)
	{
		p->render();
		ImGui::Render();

	}
}

void sceneManager::setNextScene(baseScene * _nextScene, bool stack)
{
	nextScene = _nextScene;
	isStack = stack;
}

void sceneManager::changeScene()
{
	activeScene.back()->uninit();
	if (!isStack)
	{
		if (!activeScene.empty())
		{
			for (auto& p : activeScene)
			{
				delete p;
			}
			activeScene.clear();
		}
	}
	else
	{
		if (!activeScene.empty())
		{
			for (auto& p : activeScene)
			{
				p->setIsStack(true);
			}
		}
	}
	activeScene.push_back(nextScene);

	//activeScene.push_back(_nextScene);
	activeScene.back()->init();
	nextScene = nullptr;
}

void sceneManager::deleteStack()
{
	activeScene.erase(activeScene.begin() + 1, activeScene.end());
}

void sceneManager::deleteNowScene()
{
	delete_stack = true;
}

void sceneManager::deleteStackScene()
{
	delete activeScene.back();
	

	activeScene.erase(activeScene.begin()+1);
	if (!activeScene.empty())
	{
		for (auto& p : activeScene)
		{
			p->setIsStack(false);
		}
	}
	delete_stack = false;
}

void sceneManager::end()
{
	for (auto& p : activeScene)
	{
		delete p;
	}

	activeScene.clear();
}


void sceneManagerBeta::init(baseScene* _firstScene)
{
	nowScene = _firstScene;
	nowScene->init();
	nextScene = nullptr;
}

void sceneManagerBeta::update()
{
	if (nextScene)
	{
		nowScene->uninit();
		delete nowScene;
		nowScene = nextScene;
		nowScene->init();
		nextScene = nullptr;
	}
	nowScene->update();
	nowScene->imGui();
}

void sceneManagerBeta::render()
{
	nowScene->render();
}

void sceneManagerBeta::setNextScene(baseScene* _nextScene)
{
	nextScene = _nextScene;
}

void sceneManagerBeta::end()
{
	nowScene->uninit();
	delete nowScene;
}