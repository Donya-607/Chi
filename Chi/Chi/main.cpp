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
#if DEBUG_MODE
	pSceneManager->init(new sceneTitle);
#else
	pSceneManager->init(new sceneTitle);
#endif // DEBUG_MODE


	Donya::Sound::Init();

	//とりあえず最初に一括ロードしてみる
	//BGM
	Donya::Sound::Load(TITLE, "./Data/Sounds/BGM/Title.wav", true);
	Donya::Sound::Load(STAGE1, "./Data/Sounds/BGM/Stage01.wav", true);
	Donya::Sound::Load(STAGE2, "./Data/Sounds/BGM/Stage02.wav", true);
	Donya::Sound::Load(STAGE3, "./Data/Sounds/BGM/Stage03.wav", true);
	Donya::Sound::Load(GAMEOVER, "./Data/Sounds/BGM/GameOver.wav", false);
	Donya::Sound::Load(GAMECLEAR, "./Data/Sounds/BGM/GameClear.wav", false);

	//SE
	Donya::Sound::Load(CATAPAULT, "./Data/Sounds/SE/Tutorial/Catapault.wav", false);
	Donya::Sound::Load(ROCK, "./Data/Sounds/SE/Tutorial/Rock_Impact.wav", false);

	Donya::Sound::Load(PL_DEFEND, "./Data/Sounds/SE/Player/Defend.wav", false);
	Donya::Sound::Load(PL_DEATH, "./Data/Sounds/SE/Player/Player_Death.wav", false);
	Donya::Sound::Load(PL_HIT, "./Data/Sounds/SE/Player/Player_Hit.wav", false);
	Donya::Sound::Load(PL_RUN_1, "./Data/Sounds/SE/Player/Player_Run_Stage01.wav", false);
	Donya::Sound::Load(PL_RUN_2, "./Data/Sounds/SE/Player/Player_Run_Stage02.wav", false);
	Donya::Sound::Load(PL_RUN_3, "./Data/Sounds/SE/Player/Player_Run_Stage03.wav", false);
	Donya::Sound::Load(PL_SHILED_INPACT, "./Data/Sounds/SE/Player/Player_Shiled_Impact.wav", false);
	Donya::Sound::Load(PL_SHILED_OPEN, "./Data/Sounds/SE/Player/Player_Shiled_Open_.wav", false);
	Donya::Sound::Load(PL_THRUST, "./Data/Sounds/SE/Player/Player_Thrust.wav", false);
	Donya::Sound::Load(PROTECTED, "./Data/Sounds/SE/Player/Protected.wav", false);

	Donya::Sound::Load(BOSS_2_CHARGE, "./Data/Sounds/SE/Boss/Boss02/Boss02_Charge.wav", false);
	Donya::Sound::Load(BOSS_2_EXPLOSION, "./Data/Sounds/SE/Boss/Boss02/Boss02_Explosion.wav", false);
	Donya::Sound::Load(BOSS_2_JUMP, "./Data/Sounds/SE/Boss/Boss02/Boss02_Jump.wav", false);
	Donya::Sound::Load(BOSS_2_RUN, "./Data/Sounds/SE/Boss/Boss02/Boss02_Run.wav", false);
	Donya::Sound::Load(BOSS_2_SWING, "./Data/Sounds/SE/Boss/Boss02/Boss02_Swing.wav", false);

	Donya::Sound::Load(BOSS_3_ROCK, "./Data/Sounds/SE/Boss/Boss03/Boss03_Attack_Rock.wav", false);
	Donya::Sound::Load(BOSS_3_BACK, "./Data/Sounds/SE/Boss/Boss03/Boss03_BackStep.wav", false);
	Donya::Sound::Load(BOSS_3_PUNTCH, "./Data/Sounds/SE/Boss/Boss03/Boss03_Punch.wav", false);

	Donya::Sound::Load(BOSS_4_JUMP, "./Data/Sounds/SE/Boss/Boss04/Boss04_Jump.wav", false);
	Donya::Sound::Load(BOSS_4_LINE, "./Data/Sounds/SE/Boss/Boss04/Boss04_Line.wav", false);
	Donya::Sound::Load(BOSS_4_POINT, "./Data/Sounds/SE/Boss/Boss04/Boss04_Point.wav", false);
	Donya::Sound::Load(BOSS_4_RUN, "./Data/Sounds/SE/Boss/Boss04/Boss04_Run.wav", false);
	Donya::Sound::Load(BOSS_4_RUSH, "./Data/Sounds/SE/Boss/Boss04/Boss04_Rush.wav", false);
	Donya::Sound::Load(BOSS_4_SLASH, "./Data/Sounds/SE/Boss/Boss04/Boss04_Slash.wav", false);



	while (GameLib::gameLoop(true))
	{
		pSceneManager->update();


		pSceneManager->render();

		GameLib::present(0,0);


		//動画実装用だから気にしないで～
#if 0
		static OPENFILENAME ofn;

		static wchar_t cfilename[MAX_PATH];
		static char filename[MAX_PATH];

		static long eventCode;

		//DirectShow用変数宣言
		static IGraphBuilder* pGraphBuilder = NULL;
		static IMediaControl* pMediaControl = NULL;
		static IMediaEventEx* pMediaEventEx = NULL;

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
					//動画用
					IGraphBuilder* pGraphBuilder = NULL;
					IMediaControl* pMediaControl = NULL;
					IMediaEvent* pMediaEvent = NULL;
					long eventCode = 0;
					IVideoWindow* pVideoWindow = NULL;
					IMediaPosition* pMediaPosition = NULL;

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

					while (1)
					{
						pMediaPosition->put_CurrentPosition(0);
						pMediaEvent->WaitForCompletion(-1, &eventCode);
					}


						pVideoWindow->Release();
						pMediaEvent->Release();
						pMediaControl->Release();
						pGraphBuilder->Release();
						pMediaPosition->Release();
						pMediaPosition = NULL;
						pVideoWindow = NULL;
						pMediaEvent = NULL;
						pMediaControl = NULL;
						pGraphBuilder = NULL;
						CoUninitialize();



					}
				

			}
		}

		if (getKeyState(KEY_INPUT_9) == 1)
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
					//動画用
					IGraphBuilder* pGraphBuilder = NULL;
					IMediaControl* pMediaControl = NULL;
					IMediaEvent* pMediaEvent = NULL;
					long eventCode = 0;
					IVideoWindow* pVideoWindow = NULL;
					IMediaPosition* pMediaPosition = NULL;

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
					pMediaControl->Run();

					pMediaEvent->WaitForCompletion(-1, &eventCode);

					while (1)
					{
						pMediaPosition->put_CurrentPosition(0);
						pMediaEvent->WaitForCompletion(-1, &eventCode);
					}


					pVideoWindow->Release();
					pMediaEvent->Release();
					pMediaControl->Release();
					pGraphBuilder->Release();
					pMediaPosition->Release();
					pMediaPosition = NULL;
					pVideoWindow = NULL;
					pMediaEvent = NULL;
					pMediaControl = NULL;
					pGraphBuilder = NULL;
					CoUninitialize();



				}


			}
		}

#endif

	}

	Donya::Sound::Uninit();

	pSceneManager->end();
	shutDownLibrary();
	return 0;
}
