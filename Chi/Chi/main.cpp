#include <windows.h>
#include <memory>
#include <assert.h>
#include <tchar.h>
#include <thread>
#include "GameLibFunctions.h"
#include "scene.h"
#include "sceneManager.h"
#include "imgui.h"
#include "audio.h"

INT WINAPI wWinMain(HINSTANCE instance, HINSTANCE prev_instance, LPWSTR cmd_line, INT cmd_show)
{
	startUpLibrary
	(L"ぐりのライブラリ", instance, 1920,1080, false, -1, 60.0f);

	DragAcceptFiles(GameLib::getHandle(), true);
	pSceneManager->init(new sceneTitle);
	//pAudio->init();
	//指定音階再生
	/*{
		//HRESULT hr;
		//IXAudio2SourceVoice* source_voice;
		//WAVEFORMATEX format;

		//format.wFormatTag = WAVE_FORMAT_PCM;
		//format.nChannels = 1;
		//format.wBitsPerSample = 16;
		//format.nSamplesPerSec = 44100;
		//format.nBlockAlign = format.wBitsPerSample / 8 * format.nChannels;
		//format.nAvgBytesPerSec = format.nSamplesPerSec*format.nBlockAlign;

		//if (FAILED(hr = pAudio->getXAudio()->CreateSourceVoice(&source_voice, &format)))
		//	throw "CreateSourceVoice";

		//std::vector<BYTE> data(format.nAvgBytesPerSec * 1);
		//short* p = (short*)&data[0];
		//for (size_t i = 0; i < data.size() / 2; i++)
		//{
		//	float length = format.nSamplesPerSec / 481.123f;

		//	*p = (short)(32767 * sinf(i*3.1415926535f / (length / 2)));
		//	p++;
		//}

		//XAUDIO2_BUFFER  buffer = { 0 };
		//buffer.AudioBytes = data.size();
		//buffer.pAudioData = &data[0];
		//buffer.Flags = XAUDIO2_END_OF_STREAM;
		//source_voice->SubmitSourceBuffer(&buffer);

		//source_voice->Start();

		////pAudio->setFormat(440.0f);
		////pAudio->startSource();
		//MessageBox(0, L"終了します",L"",0);
		//source_voice->Stop();
		//source_voice->DestroyVoice();
		////pAudio->stopSource();
	}*/

	//waveファイル再生
	//pAudio->loadWaveFile(L"./Data/sample.wav");
	//pAudio->multiThreadPlay();

	CopyFile(L"D:\\画像\\41TQ3K3C6GL.jpg", L"./Data/sprite/test.jpg", false);

	DeleteFile(L"./Data/sprite/test.jpg");

	while (GameLib::gameLoop(true))
	{
		pSceneManager->update();

		//if (pAudio->getFin())
		//{
		//	pAudio->getSource()->Stop();
		//	pAudio->getSource()->DestroyVoice();
		//	pAudio->resetFin();
		//}

		pSceneManager->render();

		GameLib::present(0,0);
	}
	pSceneManager->end();
	shutDownLibrary();
	//pAudio->cleanup();
	return 0;
}
