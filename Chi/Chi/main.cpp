#include <windows.h>
#include <memory>
#include <assert.h>
#include <tchar.h>
#include <thread>
#include "GameLibFunctions.h"
#include "scene.h"
#include "sceneManager.h"
#include "imgui.h"

#include "Donya/Sound.h" // Insert by Donya.

INT WINAPI wWinMain(HINSTANCE instance, HINSTANCE prev_instance, LPWSTR cmd_line, INT cmd_show)
{
	startUpLibrary
	(L"コンクエスト", instance, 1920,1080, false, -1, 60.0f);

	DragAcceptFiles(GameLib::getHandle(), true);
	pSceneManager->init(new sceneTitle);

	Donya::Sound::Init();



	while (GameLib::gameLoop(true))
	{
		pSceneManager->update();


		pSceneManager->render();

		GameLib::present(0,0);
	}

	Donya::Sound::Uninit();

	pSceneManager->end();
	shutDownLibrary();
	return 0;
}
