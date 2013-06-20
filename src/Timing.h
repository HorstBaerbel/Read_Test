#pragma once

//C++11 chrono would really shine here...

#if defined(WIN32) || defined(_WIN32)
	#include <Windows.h>
#elif defined(__linux__)
	#include <time.h>
#endif


class Timing
{
private:
#if defined(WIN32) || defined(_WIN32)
	__declspec(align(16)) LARGE_INTEGER frequency;
#endif

	static Timing * instance;

	Timing();
	~Timing();

public:
	static const Timing * getInstance();

	static unsigned int getTimeMsui();
	static float getTimeMsf();

	static unsigned long getTimeUsul();
	static double getTimeUsd();
};