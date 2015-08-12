#ifndef _TIMER_H_
#define _TIMER_H_

// platform includes
#include <windows.h>

// std includes
#include <ratio>
#include <chrono>
#include <cstdint>

class Timer
{
public:
   // constructor / destructor
    Timer( );
   ~Timer( );

   // obtains the current tick
   int64_t GetCurrentTick( ) const;

   // obtains the current time
   double GetCurrentTimeSec( ) const;
   double GetCurrentTimeMS( ) const;

   // returns the delta time
   int64_t DeltaTick( const int64_t rTick ) const;
   double  DeltaMS( const int64_t rTick ) const;
   double  DeltaSec( const int64_t rTick ) const;

   // waits a set period of time
   void Wait( const uint32_t nMS ) const;

   template < typename T, typename P >
   void Wait( const std::chrono::duration< T, P > duration ) const;

private:
   // private static member variables
   static const double  MSEC_PER_TICK;

};

namespace details
{

double GetDeltaTimeMSecPerTick( )
{
   uint64_t freqCPU = 0;
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

inline int64_t Timer::GetCurrentTick( ) const
{
   int64_t curTick = 0;
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

inline int64_t Timer::DeltaTick( const int64_t rTick ) const
{
   return GetCurrentTick() - rTick;
}

inline double Timer::DeltaMS( const int64_t rTick ) const
{
   return DeltaTick(rTick) * MSEC_PER_TICK;
}

inline double Timer::DeltaSec( const int64_t rTick ) const
{
   return DeltaMS(rTick) * 0.001;
}

inline void Timer::Wait( const uint32_t nMS ) const
{
   Sleep(nMS);
}

template < typename T, typename P >
inline void Timer::Wait( const std::chrono::duration< T, P > duration ) const
{
   Wait(std::chrono::duration_cast< std::chrono::duration< uint32_t, std::milli > >(duration).count());
}

#endif // _TIMER_H_