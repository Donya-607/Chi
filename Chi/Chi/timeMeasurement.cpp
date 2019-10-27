#include"timeMeasurement.h"

TimeMeasure::TimeMeasure()
{
	if (!QueryPerformanceFrequency(&freq))
		assert(0 && "’PˆÊK“¾‚É¸”s‚µ‚Ü‚µ‚½");
	state = 0;
}

void TimeMeasure::Start()
{
	if (!QueryPerformanceCounter(&start))
		assert(0 && "ŠJnŠÔ‚Ìæ“¾‚É¸”s‚µ‚Ü‚µ‚½");
	state++;
}

double TimeMeasure::End()
{
	if (state != 1)
		assert(0 && "StartŠÖ”‚ğŒÄ‚ñ‚Å‚¢‚Ü‚¹‚ñ");

	if (!QueryPerformanceCounter(&end))
		assert(0 && "I—¹ŠÔ‚Ìæ“¾‚É¸”s‚µ‚Ü‚µ‚½");

	double result = 0;

	result = static_cast<double>(end.QuadPart - start.QuadPart) / freq.QuadPart;
	state = 0;
	return result;

}


TimeMeasure::~TimeMeasure()
{

}