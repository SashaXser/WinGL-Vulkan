#ifndef _TIMER_H_
#define _TIMER_H_

// platform includes
#include <windows.h>

class Timer
{
public:
   // constructor / destructor
    Timer( );
   ~Timer( );

   // obtains the current tick
   long long   GetCurrentTick( ) const;

   // obtains the current time
   double      GetCurrentTimeSec( ) const;
   double      GetCurrentTimeMS( ) const;

   // returns the delta time
   long long   DeltaTick( const long long & rTick ) const;
   double      DeltaMS( const long long & rTick ) const;
   double      DeltaSec( const long long & rTick ) const;

   // waits a set period of time
   void  Wait( unsigned long nMS ) const;



private:
   // private static member variables
   static const double  MSEC_PER_TICK;

};

namespace details
{

double GetDeltaTimeMSecPerTick( )
{
   long long freqCPU = 0;
   QueryPerformanceFrequency(reinterpret_cast< LARGE_INTEGER * >(&freqCPU));

   return 1000.0 / freqCPU;
}

} // namespace details

const double Timer::MSEC_PER_TICK = details::GetDeltaTimeMSecPerTick();

inline Timer::Timer( )
{
}

inline Timer::~Timer( )
{
}

inline long long Timer::GetCurrentTick( ) const
{
   long long curTick = 0;
   QueryPerformanceCounter(reinterpret_cast< LARGE_INTEGER * >(&curTick));
   return curTick;
}

inline double Timer::GetCurrentTimeSec( ) const
{
   return GetCurrentTimeMS() * 0.001;
}

inline double Timer::GetCurrentTimeMS( ) const
{
   return GetCurrentTick() * MSEC_PER_TICK;
}

inline long long Timer::DeltaTick( const long long & rTick ) const
{
   return GetCurrentTick() - rTick;
}

inline double Timer::DeltaMS( const long long & rTick ) const
{
   return DeltaTick(rTick) * MSEC_PER_TICK;
}

inline double Timer::DeltaSec( const long long & rTick ) const
{
   return DeltaMS(rTick) * 0.001;
}

inline void Timer::Wait( unsigned long nMS ) const
{
   Sleep(nMS);
}

#endif // _TIMER_H_