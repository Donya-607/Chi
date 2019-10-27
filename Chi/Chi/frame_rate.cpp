#include "frame_rate.h"
#include <sstream>

namespace GameLib
{
	void calculateFrameStats(HWND hWnd, high_resolution_timer *timer,std::wstring _pName)
	{

		// Code computes the average frames per second, and also the 
		// average time it takes to render one frame.  These stats 
		// are appended to the window caption bar.
		static int frames = 0;
		static float time_tlapsed = 0.0f;

		frames++;

		// Compute averages over one second period.
		if ((timer->time_stamp() - time_tlapsed) >= 1.0f)
		{
			float fps = static_cast<float>(frames); // fps = frameCnt / 1
			float mspf = 1000.0f / fps;
			std::string outs;
			outs = "  FPS : " + std::to_string(fps) + " / " + "Frame Time : " + std::to_string(mspf) + " (ms)";
			std::wstring name = _pName + std::wstring(outs.begin(), outs.end());
			SetWindowTextW(hWnd, name.c_str());

			// Reset for next average.
			frames = 0;
			time_tlapsed += 1.0f;
		}
	}
}