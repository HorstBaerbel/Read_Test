#include "Timing.h"


Timing * Timing::instance = nullptr;


Timing::Timing()
{
#if defined(WIN32) || defined(_WIN32)
	QueryPerformanceFrequency(&frequency);
#endif
}

const Timing * Timing::getInstance()
{
	if (!instance) {
		instance = new Timing;
	}
	return instance;
}

unsigned int Timing::getTimeMsui()
{
#if defined(WIN32) || defined(_WIN32)
	__declspec(align(16)) LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return ((1000 * counter.QuadPart) / instance->frequency.QuadPart);
#elif defined(__linux__)
	timespec current;
	clock_gettime(CLOCK_MONOTONIC, &current);
	return (current.tv_sec * 1000 + current.tv_nsec / 1000000);
#endif
	return 0;
}

float Timing::getTimeMsf()
{
#if defined(WIN32) || defined(_WIN32)
	__declspec(align(16)) LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return ((1000.0f * (float)counter.QuadPart) / (float)instance->frequency.QuadPart);
#elif defined(__linux__)
	timespec current;
	clock_gettime(CLOCK_MONOTONIC, &current);
	return ((float)current.tv_sec * 1000.0f + (float)current.tv_nsec / 1000000.0f);
#endif
	return 0.0f;
}

unsigned long Timing::getTimeUsul()
{
#if defined(WIN32) || defined(_WIN32)
	__declspec(align(16)) LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return ((1000000 * counter.QuadPart) / instance->frequency.QuadPart);
#elif defined(__linux__)
	timespec current;
	clock_gettime(CLOCK_MONOTONIC, &current);
	return ((unsigned long)current.tv_sec * 1000000 + (unsigned long)current.tv_nsec / 1000);
#endif
	return 0;
}

double Timing::getTimeUsd()
{
#if defined(WIN32) || defined(_WIN32)
	__declspec(align(16)) LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return ((1000000.0 * (double)counter.QuadPart) / (double)instance->frequency.QuadPart);
#elif defined(__linux__)
	timespec current;
	clock_gettime(CLOCK_MONOTONIC, &current);
	return ((double)current.tv_sec * 1000000.0 + (double)current.tv_nsec / 1000.0);
#endif
	return 0.0f;
}

Timing::~Timing()
{
	instance = nullptr;
}