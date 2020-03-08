#ifndef GATIMER_H
#define GATIMER_H

#include <windows.h>

namespace Aurora
{

class Timer
{
public:
	Timer(void);
	~Timer(void);

	void			Reset();				// reset and start, call to start first time
	void			Start();
	void			Stop();
	float			GetTime();				// get time from last reset call
	float			GetElapsedTime();		// get time from last GetElapsedTime call
	bool			IsStopped();

private:
    LONGLONG		GetAdjustedCurrentTime();

    LONGLONG		m_llQPFTicksPerSec;

    LONGLONG		m_llStopTime;
    LONGLONG		m_llLastElapsedTime;
    LONGLONG		m_llBaseTime;

};

}


#endif