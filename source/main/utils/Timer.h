/*
	This source file is part of Rigs of Rods
	Copyright 2005-2012 Pierre-Michel Ricordel
	Copyright 2007-2012 Thomas Fischer
	Copyright 2013-2015 Petr Ohlidal

	For more information, see http://www.rigsofrods.com/

	Rigs of Rods is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License version 3, as
	published by the Free Software Foundation.

	Rigs of Rods is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Rigs of Rods. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "RoRPrerequisites.h"

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
