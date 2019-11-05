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
	// DEBUG : _CrtSetDbgFlag関数で _CRTDBG_CHECK_ALWAYS_DF フラグを設定するために，いろんなところで設定しています

//#if defined( DEBUG ) | defined( _DEBUG )
//	// reference:https://docs.microsoft.com/ja-jp/visualstudio/debugger/crt-debug-heap-details?view=vs-2015
//	_CrtSetDbgFlag
//	(
//		_CRTDBG_ALLOC_MEM_DF
//		| _CRTDBG_LEAK_CHECK_DF
//		| _CRTDBG_CHECK_ALWAYS_DF // This configuration is heavy.
//	);
//	// When memory leak detected, if you assign the output number to _crtBreakAlloc,
//	// program will be stop in that memory allocate place.
//	// _crtBreakAlloc = ;
//#endif

	startUpLibrary
	(L"コンクエスト", instance, 1920,1080, false, -1, 60.0f);

//#if defined( DEBUG ) | defined( _DEBUG )
//	_CrtSetDbgFlag
//	(
//		_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF // This configuration is heavy.
//	);
//#endif

	DragAcceptFiles(GameLib::getHandle(), true);
	pSceneManager->init(new sceneTitle);

//#if defined( DEBUG ) | defined( _DEBUG )
//	_CrtSetDbgFlag
//	(
//		_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF // This configuration is heavy.
//	);
//#endif

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
