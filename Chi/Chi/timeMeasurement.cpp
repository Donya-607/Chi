#include"timeMeasurement.h"

TimeMeasure::TimeMeasure()
{
	if (!QueryPerformanceFrequency(&freq))
		assert(0 && "�P�ʏK���Ɏ��s���܂���");
	state = 0;
}

void TimeMeasure::Start()
{
	if (!QueryPerformanceCounter(&start))
		assert(0 && "�J�n���Ԃ̎擾�Ɏ��s���܂���");
	state++;
}

double TimeMeasure::End()
{
	if (state != 1)
		assert(0 && "Start�֐����Ă�ł��܂���");

	if (!QueryPerformanceCounter(&end))
		assert(0 && "�I�����Ԃ̎擾�Ɏ��s���܂���");

	double result = 0;

	result = static_cast<double>(end.QuadPart - start.QuadPart) / freq.QuadPart;
	state = 0;
	return result;

}


TimeMeasure::~TimeMeasure()
{

}