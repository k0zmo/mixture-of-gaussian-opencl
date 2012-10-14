#include "QPCTimer.h"

#include <windows.h>
#undef max
#undef min

QPCTimer::QPCTimer()
{
	LARGE_INTEGER freq;
	SetThreadAffinityMask(GetCurrentThread(), 1);
	QueryPerformanceFrequency(&freq);
	periodTime = 1.0 / static_cast<double>(freq.QuadPart);
}

double QPCTimer::currentTime()
{
	LARGE_INTEGER time;
	QueryPerformanceCounter(&time);
	return static_cast<double>(time.QuadPart) * periodTime;
}