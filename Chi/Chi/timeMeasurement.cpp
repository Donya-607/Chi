#include"timeMeasurement.h"

TimeMeasure::TimeMeasure()
{
	if (!QueryPerformanceFrequency(&freq))
		assert(0 && "単位習得に失敗しました");
	state = 0;
}

void TimeMeasure::Start()
{
	if (!QueryPerformanceCounter(&start))
		assert(0 && "開始時間の取得に失敗しました");
	state++;
}

double TimeMeasure::End()
{
	if (state != 1)
		assert(0 && "Start関数を呼んでいません");

	if (!QueryPerformanceCounter(&end))
		assert(0 && "終了時間の取得に失敗しました");

	double result = 0;

	result = static_cast<double>(end.QuadPart - start.QuadPart) / freq.QuadPart;
	state = 0;
	return result;

}


TimeMeasure::~TimeMeasure()
{

}