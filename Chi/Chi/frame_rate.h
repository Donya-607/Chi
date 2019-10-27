#ifndef FRAME_LATE_H_
#define	FRAME_LATE_H_

#include <Windows.h>
#include "high_resolution_timer.h"
#include <string>

namespace GameLib
{
	void calculateFrameStats(HWND, high_resolution_timer*,std::wstring);
}

#endif // !FRAME_LATE_H_
