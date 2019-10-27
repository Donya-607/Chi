#ifndef TIMEMEASUREMENT_H_
#define TIMEMEASUREMENT_H_
#include <windows.h>
#include <cassert>

class TimeMeasure
{
private:
	LARGE_INTEGER freq, start, end;
	int state;
public:
	TimeMeasure();
	void Start();
	double End();
	~TimeMeasure();

};

#endif // !TIMEMEASUREMENT_H_
