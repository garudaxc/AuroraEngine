#include "stdHeader.h"
#include "Timer.h"


namespace Aurora
{

//--------------------------------------------------------------------------------------
// Limit the current thread to one processor (the current one). This ensures that timing code 
// runs on only one processor, and will not suffer any ill effects from power management.
// See "Game Timing and Multicore Processors" for more details
//--------------------------------------------------------------------------------------
void LimitThreadAffinityToCurrentProc()
{
    HANDLE hCurrentProcess = GetCurrentProcess();
    
    // Get the processor affinity mask for this process
    DWORD_PTR dwProcessAffinityMask = 0;
    DWORD_PTR dwSystemAffinityMask = 0;
    
    if( GetProcessAffinityMask( hCurrentProcess, &dwProcessAffinityMask, &dwSystemAffinityMask ) != 0 && dwProcessAffinityMask )
    {
        // Find the lowest processor that our process is allows to run against
        DWORD_PTR dwAffinityMask = ( dwProcessAffinityMask & ((~dwProcessAffinityMask) + 1 ) );

        // Set this as the processor that our thread must always run against
        // This must be a subset of the process affinity mask
        HANDLE hCurrentThread = GetCurrentThread();
        if( INVALID_HANDLE_VALUE != hCurrentThread )
        {
            SetThreadAffinityMask( hCurrentThread, dwAffinityMask );
            CloseHandle( hCurrentThread );
        }
    }

    CloseHandle( hCurrentProcess );
}



Timer::Timer(void)
{
	LimitThreadAffinityToCurrentProc();
	// Use QueryPerformanceFrequency to get the frequency of the counter
    m_llQPFTicksPerSec			= 0;
    LARGE_INTEGER qwTicksPerSec = { 0 };
    QueryPerformanceFrequency(&qwTicksPerSec);
    m_llQPFTicksPerSec = qwTicksPerSec.QuadPart;

	m_llStopTime			= 0;
    m_llLastElapsedTime		= 0;
    m_llBaseTime			= 0;

}

Timer::~Timer(void)
{
}

void Timer::Reset()
{
	LARGE_INTEGER qwTime = { 0 };
	QueryPerformanceCounter(&qwTime);

	m_llBaseTime		= qwTime.QuadPart;
	m_llLastElapsedTime	= qwTime.QuadPart;
	m_llStopTime		= 0;
}

void Timer::Start()
{
	if (m_llStopTime != 0)
	{
		LARGE_INTEGER qwTime = { 0 };
		QueryPerformanceCounter(&qwTime);

		m_llBaseTime += qwTime.QuadPart - m_llStopTime;
		m_llLastElapsedTime = qwTime.QuadPart;
		m_llStopTime = 0;
	}
}

void Timer::Stop()
{
	if (m_llStopTime == 0)
	{
		LARGE_INTEGER qwTime = { 0 };
		QueryPerformanceCounter(&qwTime);

		m_llStopTime = qwTime.QuadPart;
		m_llLastElapsedTime = qwTime.QuadPart;
	}
}

float Timer::GetTime()
{
	LONGLONG llCurTime = GetAdjustedCurrentTime();
	double time = (double)(llCurTime - m_llBaseTime) / (double)m_llQPFTicksPerSec;
	return (float)time;
}

float Timer::GetElapsedTime()
{
	LONGLONG llCurTime = GetAdjustedCurrentTime();
	double time = (double)(llCurTime - m_llLastElapsedTime) / (double)m_llQPFTicksPerSec;
	m_llLastElapsedTime = llCurTime;

	// Clamp the timer to non-negative values to ensure the timer is accurate.
    // fElapsedTime can be outside this range if processor goes into a 
    // power save mode or we somehow get shuffled to another processor.  
    // However, the main thread should call SetThreadAffinityMask to ensure that 
    // we don't get shuffled to another processor.  Other worker threads should NOT call 
    // SetThreadAffinityMask, but use a shared copy of the timer data gathered from 
    // the main thread.
	if (time < 0.0)
		time = 0.0;
	return (float)time;
}


LONGLONG Timer::GetAdjustedCurrentTime()
{
	if (m_llStopTime != 0)
		return m_llStopTime;
	
	LARGE_INTEGER qwTime;
	QueryPerformanceCounter(&qwTime);
	return qwTime.QuadPart;
}


bool Timer::IsStopped()
{
	return m_llStopTime != 0;
}

}