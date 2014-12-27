// license: do whatever you want to do with it ;)
#pragma once

#include "RoRPrerequisites.h"

// boost timer is awful, measures cpu time on linux only...
// thus we have to hack together some cross platform timer :(

#ifndef _WIN32
#include <sys/time.h>
#else
#include <windows.h>
#endif // _WIN32

class PrecisionTimer : public ZeroedMemoryAllocator
{

public:
    PrecisionTimer()
    {
        restart();
    }

	long long ElapsedMilliseconds()
	{
#ifdef WIN32
		LARGE_INTEGER tick;
		QueryPerformanceCounter(&tick);
		return (tick.QuadPart - m_start_timestamp) / (m_ticks_per_second / 1000);
#else
		struct timeval now;
        gettimeofday(&now, NULL); // TODO: Known to be inaccurate, use clock_gettime()
        return ((now.tv_sec - start.tv_sec)*1000) + ((now.tv_usec - start.tv_usec)/1000.0);
#endif
	}

	long long ElapsedTicks()
	{
#ifdef WIN32
		LARGE_INTEGER tick;
		QueryPerformanceCounter(&tick);
		return (tick.QuadPart - m_start_timestamp);
#else
		return 0; // TODO
#endif
	}

    double elapsed()
    {
#ifdef _WIN32
        LARGE_INTEGER tick;
        QueryPerformanceCounter(&tick);
        return ((double)tick.QuadPart - (double)m_start_timestamp) / (double)m_ticks_per_second;
#else
        struct timeval now;
        gettimeofday(&now, NULL); // TODO: Known to be inaccurate, use clock_gettime()
        return (now.tv_sec - start.tv_sec) + (now.tv_usec - start.tv_usec)/1000000.0;
#endif // _WIN32
    }

    void restart()
    {
#ifdef _WIN32
		LARGE_INTEGER start, ticks_per_second;
        QueryPerformanceCounter(&start);
		QueryPerformanceFrequency(&ticks_per_second);
		m_start_timestamp = start.QuadPart;
		m_ticks_per_second = ticks_per_second.QuadPart;
#else
        gettimeofday(&start, NULL);
#endif // _WIN32
    }

private:
#ifdef WIN32
    long long m_start_timestamp;
	long long m_ticks_per_second;
#else
    struct timeval m_start;
#endif
};
