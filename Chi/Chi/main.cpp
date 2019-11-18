#include <windows.h>
#include <memory>
#include <assert.h>
#include <tchar.h>
#include <thread>
#include "GameLibFunctions.h"
#include "scene.h"
#include "sceneManager.h"
#include "imgui.h"

#include "Donya/Sound.h"

INT WINAPI wWinMain(HINSTANCE instance, HINSTANCE prev_instance, LPWSTR cmd_line, INT cmd_show)
{
#if defined( DEBUG ) | defined( _DEBUG )
	// reference:https://docs.microsoft.com/ja-jp/visualstudio/debugger/crt-debug-heap-details?view=vs-2015
	//_CrtSetDbgFlag
	//(
	//	_CRTDBG_ALLOC_MEM_DF
	//	| _CRTDBG_LEAK_CHECK_DF
	//	// | _CRTDBG_CHECK_ALWAYS_DF
	//);
	// When memory leak detected, if you assign the output number to "_crtBreakAlloc",
	// program will be stop in that memory allocate place. ex : _crtBreakAlloc = 123;
	// _crtBreakAlloc = ;
#endif

	startUpLibrary
	(L"コンクエスト", instance, 1920, 1080, false, -1, 60.0f);

	DragAcceptFiles(GameLib::getHandle(), true);
	Donya::Sound::Init();

	pSceneManager->init(new sceneTitle);

	while (GameLib::gameLoop(true))
	{
		pSceneManager->update();


		pSceneManager->render();

		GameLib::present(0, 0);


		//動画実装用だから気にしないで～
#if 1
		static OPENFILENAME ofn;

		static wchar_t cfilename[MAX_PATH];
		static char filename[MAX_PATH];

		static long eventCode;

		//DirectShow用変数宣言
		static IGraphBuilder* pGraphBuilder = NULL;
		static IMediaControl* pMediaControl = NULL;
		static IMediaEventEx* pMediaEventEx = NULL;
		static IVideoWindow* pVideoWindow = NULL;
		static OAFilterState* pFilterState = NULL;

		//VMR9
		static ICaptureGraphBuilder2* pCaptureGraphBuilder2 = NULL;
		static IBaseFilter* pVmr9 = NULL;
		static IBaseFilter* pSource = NULL;
		static IVMRFilterConfig* pVMRCfg = NULL;
		static IVMRWindowlessControl* pVMRWndCont = NULL;
		//VMR9

		if (getKeyState(KEY_INPUT_0) == 1)
		{
			if (!pGraphBuilder)//pGraphBuilderに値の割り当てがない場合以下を実施・多重送出を避ける
			{
				while (ShowCursor(TRUE) < 0);//カーソル表示
			   //ファイルオープン
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = GameLib::getHandle();
				ofn.Flags = 0;
				ofn.lpstrFile = cfilename;
				ofn.nMaxFile = MAX_PATH;
				ofn.lpstrFilter =
					L"全てのファイル (*.*)\0"	"*.*\0"
					L"mpg (*.mpg)\0"	"*.mpg\0"
					L"mpeg (*.mpeg)\0""*.mpeg\0"
					L"mp4 (*.mp4)\0"	"*.mp4\0"
					;
				ofn.nFilterIndex = 1;

				ofn.lpstrFile = cfilename;
				ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
				ofn.lpstrTitle = L"File Open";

				if (!GetOpenFileName(&ofn))
				{
					MessageBox(NULL, L"ファイルの取得を失敗しました", L"GetOpenFileName", MB_OK);
				}
				else
				{
					IGraphBuilder* pGraphBuilder;
					IMediaControl* pMediaControl;
					IMediaEvent* pMediaEvent;
					long eventCode;
					IVideoWindow* pVideoWindow;
					IMediaPosition* pMediaPosition;

					// COMを初期化
					CoInitialize(NULL);

					CoCreateInstance(CLSID_FilterGraph,
						NULL,
						CLSCTX_INPROC,
						IID_IGraphBuilder,
						(LPVOID*)&pGraphBuilder);

					pGraphBuilder->QueryInterface(IID_IMediaControl,
						(LPVOID*)&pMediaControl);

					pGraphBuilder->QueryInterface(IID_IMediaEvent,
						(LPVOID*)&pMediaEvent);

					pMediaControl->RenderFile(cfilename);

					pGraphBuilder->QueryInterface(IID_IVideoWindow,
						(LPVOID*)&pVideoWindow);

					pGraphBuilder->QueryInterface(IID_IMediaPosition,
						(LPVOID*)&pMediaPosition);


					// Full Screen 開始
					pVideoWindow->put_FullScreenMode(OATRUE);

					pMediaControl->Run();

					pMediaEvent->WaitForCompletion(-1, &eventCode);

					pMediaPosition->put_CurrentPosition(0);
					pMediaEvent->WaitForCompletion(-1, &eventCode);

					pVideoWindow->Release();
					pMediaEvent->Release();
					pMediaControl->Release();
					pGraphBuilder->Release();
					CoUninitialize();


				}
			}


		}

		if (getKeyState(KEY_INPUT_9) == 1)
		{
			if (!pGraphBuilder)return 0;//pGraphBuilderに割り当てがない場合以下を実行しない・異常終了を避ける
			pMediaControl->Stop();//映像送出停止

			delete pFilterState;
			pFilterState = NULL;

			pVMRWndCont->Release();
			pVMRWndCont = NULL;//VMR9
			pVMRCfg = NULL;//VMR9
			pCaptureGraphBuilder2->Release();//VMR9
			pCaptureGraphBuilder2 = NULL;//VMR9
			pVmr9->Release();//VMR9
			pVmr9 = NULL;//VMR

			pMediaEventEx->Release();
			pMediaEventEx = NULL;
			pMediaControl->Release();
			pMediaControl = NULL;
			pGraphBuilder->Release();
			pGraphBuilder = NULL;

			CoUninitialize();

		}

#endif

	}

	Donya::Sound::Uninit();

	pSceneManager->end();
	shutDownLibrary();
	return 0;
}
