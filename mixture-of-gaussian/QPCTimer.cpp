#include "QPCTimer.h"

#ifdef _WIN32
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
#else
QPCTimer::QPCTimer()
{
	gettimeofday(&start, nullptr);
}

double QPCTimer::currentTime()
{
	timeval current;
	gettimeofday(&current, nullptr);
	return current.tv_sec - start.tv_sec + 0.000001f * (current.tv_usec - start.tv_usec);
}
#endif