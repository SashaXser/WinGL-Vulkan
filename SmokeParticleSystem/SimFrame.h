#ifndef _SIM_FRAME_H_
#define _SIM_FRAME_H_

struct SimFrame
{
   double dSimTimeMS;
   double dCurTimeMS;
   double dPrevTimeMS;
   double dTimeDeltaMS;
   long long nCurTime;
   long long nPrevTime;
   long long nTimeDelta;
};

#endif // _SIM_FRAME_H_