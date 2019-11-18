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
	(L"�R���N�G�X�g", instance, 1920, 1080, false, -1, 60.0f);

	DragAcceptFiles(GameLib::getHandle(), true);
	Donya::Sound::Init();

	pSceneManager->init(new sceneTitle);

	while (GameLib::gameLoop(true))
	{
		pSceneManager->update();


		pSceneManager->render();

		GameLib::present(0, 0);


		//��������p������C�ɂ��Ȃ��Ł`
#if 1
		static OPENFILENAME ofn;

		static wchar_t cfilename[MAX_PATH];
		static char filename[MAX_PATH];

		static long eventCode;

		//DirectShow�p�ϐ��錾
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
			if (!pGraphBuilder)//pGraphBuilder�ɒl�̊��蓖�Ă��Ȃ��ꍇ�ȉ������{�E���d���o�������
			{
				while (ShowCursor(TRUE) < 0);//�J�[�\���\��
			   //�t�@�C���I�[�v��
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = GameLib::getHandle();
				ofn.Flags = 0;
				ofn.lpstrFile = cfilename;
				ofn.nMaxFile = MAX_PATH;
				ofn.lpstrFilter =
					L"�S�Ẵt�@�C�� (*.*)\0"	"*.*\0"
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
					MessageBox(NULL, L"�t�@�C���̎擾�����s���܂���", L"GetOpenFileName", MB_OK);
				}
				else
				{
					IGraphBuilder* pGraphBuilder;
					IMediaControl* pMediaControl;
					IMediaEvent* pMediaEvent;
					long eventCode;
					IVideoWindow* pVideoWindow;
					IMediaPosition* pMediaPosition;

					// COM��������
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


					// Full Screen �J�n
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
			if (!pGraphBuilder)return 0;//pGraphBuilder�Ɋ��蓖�Ă��Ȃ��ꍇ�ȉ������s���Ȃ��E�ُ�I���������
			pMediaControl->Stop();//�f�����o��~

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
