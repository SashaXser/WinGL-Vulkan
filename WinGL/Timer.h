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

   // returns the delta time
   long long   DeltaTick( const long long & rTick ) const;
   double      DeltaMS( const long long & rTick ) const;
   double      DeltaSec( const long long & rTick ) const;

   // waits a set period of time
   void  Wait( unsigned long nMS ) const;

private:
   // private member variables
   const double   mMSecPerTick;

};

inline Timer::Timer( ) :
mMSecPerTick   ( 0.0 )
{
   long long freqCPU = 0;
   QueryPerformanceFrequency(reinterpret_cast< LARGE_INTEGER * >(&freqCPU));
   const_cast< double & >(mMSecPerTick) = 1000.0 / freqCPU;
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

inline long long Timer::DeltaTick( const long long & rTick ) const
{
   return GetCurrentTick() - rTick;
}

inline double Timer::DeltaMS( const long long & rTick ) const
{
   return DeltaTick(rTick) * mMSecPerTick;
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