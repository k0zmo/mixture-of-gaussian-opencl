#pragma once

#ifndef _WIN32
#include <sys/time.h>
#endif

class QPCTimer
{
public:
	QPCTimer();
	double currentTime();

private:
#ifdef _WIN32
	double periodTime;
#else
	timeval start;
#endif
};

