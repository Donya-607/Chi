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

#define	FILENAME "./Data/�m�C�W�[�K�[��_-_2.mpg"
#define	WFILENAME L"./Data/�m�C�W�[�K�[��_-_2.mpg" 


/*
	DirectShow�œ�����Đ�����
*/


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
	// When memory leak detected, if you assign the output number to _crtBreakAlloc,
	// program will be stop in that memory allocate place.
	//_crtBreakAlloc = ;
#endif

	//����(�ʃE�B���h�E)
#if 0
	static OPENFILENAME ofn;

	static wchar_t cfilename[MAX_PATH];
	static char filename[MAX_PATH];

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = GameLib::getHandle();
	ofn.Flags = 0;
	ofn.lpstrFile = cfilename;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter =
		L"�S�Ẵt�@�C�� (*.*)\0"	"*.*\0"
		;
	ofn.nFilterIndex = 1;

	ofn.lpstrFile = cfilename;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrTitle = L"File Open";

	if (!GetOpenFileName(&ofn))
	{
		MessageBox(NULL, L"�t�@�C���̎擾�����s���܂���", L"GetOpenFileName", MB_OK);
	}

	SetCurrentDirectory(L"..");

	IGraphBuilder* pGraphBuilder;
	IMediaControl* pMediaControl;
	IMediaEvent* pMediaEvent;
	long eventCode;
	IVideoWindow* pVideoWindow;

	HRESULT hr;
	// COM��������
	hr = CoInitialize(NULL);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// FilterGraph�𐶐�
	hr = CoCreateInstance(CLSID_FilterGraph,
		NULL,
		CLSCTX_INPROC,
		IID_IGraphBuilder,
		(LPVOID*)&pGraphBuilder);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// MediaControl�C���^�[�t�F�[�X�擾
	pGraphBuilder->QueryInterface(IID_IMediaControl,
		(LPVOID*)&pMediaControl);

	// MediaEvent �C���^�[�t�F�C�X�擾
	pGraphBuilder->QueryInterface(IID_IMediaEvent,
		(LPVOID*)&pMediaEvent);

	// Graph�𐶐�
	wstring file_name = cfilename;
	hr = pMediaControl->RenderFile(SysAllocString(file_name.c_str()));
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// VideoWindow �C���^�[�t�F�C�X�擾
	pGraphBuilder->QueryInterface(IID_IVideoWindow,
		(LPVOID*)&pVideoWindow);

	//�t���X�N���[���ɕύX
	//pVideoWindow->put_FullScreenMode(OATRUE);

	// �Đ��J�n
	pMediaControl->Run();
	
	//mp4��pMediaEvent�Ŏ~�܂�Ȃ�
	MessageBox(NULL, L"������Đ����Ă��܂��B?r?nOK�������ƏI�����܂��B", L"���m�点", MB_OK);

	// �Đ����Ƀv���O�������I����Ă��܂�Ȃ��悤��
	//pMediaEvent->WaitForCompletion(-1, &eventCode);

	//�t���X�N���[���I��
	//pVideoWindow->put_FullScreenMode(OAFALSE);

	// ���������
	pVideoWindow->Release();
	pMediaEvent->Release();
	pMediaControl->Release();
	pGraphBuilder->Release();

	// COM�I��
	CoUninitialize();
#endif//0
#if 0
	int _tmain
	(
		int argc
		, _TCHAR * argv[]
	);
	{
		// std::wcout�̃��P�[����ݒ�
		std::wcout.imbue(std::locale("", std::locale::ctype));

		// �G���[�R�[�h
		HRESULT hResult = S_OK;

		// COM��������
		::CoInitialize(NULL);

		// �Đ����铮��t�@�C��
		std::wstring strFilePath = L"./Data/WANTED-GIRL.mp4";

		// IGraphBuilder
		IGraphBuilder* pGraphBuilder = NULL;

		// IMediaControl
		IMediaControl* pMediaControl = NULL;

		// IGraphBuilder���擾
		hResult = ::CoCreateInstance(
			CLSID_FilterGraph
			, NULL
			, CLSCTX_INPROC
			, IID_IGraphBuilder
			, (LPVOID*)&pGraphBuilder
		);
		if (!SUCCEEDED(hResult)) {

			// �G���[
			goto err;
		}

		// IMediaControl�̎擾
		hResult = pGraphBuilder->QueryInterface(
			IID_IMediaControl
			, (void**)&pMediaControl
		);
		if (!SUCCEEDED(hResult)) {

			// �G���[
			goto err;
		}

		// FilterGraph�̐���
		hResult = pMediaControl->RenderFile((BSTR)strFilePath.c_str());
		if (!SUCCEEDED(hResult)) {

			// �G���[
			goto err;
		}

		// ����̍Đ�
		hResult = pMediaControl->Run();
		if (!SUCCEEDED(hResult)) {

			// �G���[
			goto err;
		}

		// ����҂�
		::MessageBox(NULL, L"������Đ����Ă��܂��B?r?nOK�������ƏI�����܂��B", L"���m�点", MB_OK);


	err:
		// IMediaControl�̔j��
		if (NULL != pMediaControl) {
			pMediaControl->Release();
		}

		// IGraphBuilder�̔j��
		if (NULL != pGraphBuilder) {
			pGraphBuilder->Release();
		}

		// COM�̏I��
		::CoUninitialize();

		// �������ʂ�Ԃ�
		return(0);
	}
#endif
	startUpLibrary
	(L"�R���N�G�X�g", instance, 1920, 1080, false, -1, 60.0f);

	DragAcceptFiles(GameLib::getHandle(), true);
	pSceneManager->init(new sceneTitle);

	Donya::Sound::Init();

	while (GameLib::gameLoop(true))
	{
		pSceneManager->update();


		pSceneManager->render();


		GameLib::present(0, 0);
#if 1
		static OPENFILENAME ofn;

		static wchar_t cfilename[MAX_PATH];
		static char filename[MAX_PATH];

		static long eventCode;

		//DirectShow�p�ϐ��錾
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
					//�������ɕK�{
					//�P�f���t�@�C���ɂ����̏��������s��
					HRESULT hr;
					hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
					_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
					hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (LPVOID*)&pGraphBuilder);
					_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

					hr = CoCreateInstance(CLSID_VideoMixingRenderer9, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pVmr9);// VMR9�t�B���^��p��
					_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
					pGraphBuilder->AddFilter(pVmr9, L"VMR9");// VMR9�t�B���^��Graph�ɒǉ�


					//VMR9���E�B���h�E���X���[�h�ɂ���
					hr = pVmr9->QueryInterface(IID_IVMRFilterConfig9, (void**)&pVMRCfg);
					_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
					hr = pVMRCfg->SetRenderingMode(VMRMode_Windowless);
					_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
					pVMRCfg->Release();

					//�`��E�B���h�E�̎w��
					hr = pVmr9->QueryInterface(IID_IVMRWindowlessControl9, (void**)&pVMRWndCont);
					_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
					pFilterState = new long;
					//��̓���t�@�C���̓ǂݍ��݂ɑΉ�
					HRESULT hResult;
					//VMR9
					hr = pVMRWndCont->SetVideoClippingWindow(GameLib::getHandle());

					//�Đ�����t�@�C����SourceFilter�Ƃ��ėp�ӁE���������ꍇ��pSource������
					hResult = pGraphBuilder->AddSourceFilter(cfilename, cfilename, &pSource);
					//VMR9
					if (hResult != S_OK)
					{
						MessageBox(NULL, cfilename, L"AddSourceFilter���s", MB_OK);
						SendMessage(GameLib::getHandle(), WM_KEYDOWN, VK_END, NULL);//���s�����̂Ŏ擾�����|�C���^�͉������

					}
					else
					{
						// CaptureGraphBuilder2�Ƃ����L���v�`���pGraphBuilder�𐶐�����
						hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, (void**)&pCaptureGraphBuilder2);
						_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

						pCaptureGraphBuilder2->SetFiltergraph(pGraphBuilder);// FilterGraph���Z�b�g����

						//�r�f�I�ڑ�
						hr = pCaptureGraphBuilder2->RenderStream(NULL, NULL, pSource, 0, NULL);
						_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
						//�I�[�f�B�I�ڑ�
						hr = pCaptureGraphBuilder2->RenderStream(0, &MEDIATYPE_Audio, pSource, 0, 0);
						_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
						pSource->Release();//AddSourceFilter�����������ꍇ���
						pSource = NULL;
						//VMR9

						//�E�B���h�E�̕ω��ɓK��
						LONG W, H;
						RECT SrcR, DestR;
						hr = pVMRWndCont->GetNativeVideoSize(&W, &H, NULL, NULL);
						_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
						SetRect(&SrcR, 0, 0, W, H);
						GetClientRect(GameLib::getHandle(), &DestR);
						hr = pVMRWndCont->SetVideoPosition(&SrcR, &DestR);
						_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

						pVMRWndCont->Release();

						pGraphBuilder->QueryInterface(IID_IMediaControl, (LPVOID*)&pMediaControl);
						pGraphBuilder->QueryInterface(IID_IMediaEventEx, (LPVOID*)&pMediaEventEx);



						hr = pMediaEventEx->SetNotifyWindow((long)GameLib::getHandle(), WM_APP + 1, 0);//DirectShow�ł̃C�x���g�̎擾��ݒ�
						_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
						hr = pMediaControl->Run();//�f�����o�J�n
						//pMediaEventEx->WaitForCompletion(-1, &eventCode);


					}
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

#endif //1


	}

	Donya::Sound::Uninit();

	pSceneManager->end();
	shutDownLibrary();
	return 0;
}
