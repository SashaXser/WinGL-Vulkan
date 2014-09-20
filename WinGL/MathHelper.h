#ifndef _MATH_HELPER_H_
#define _MATH_HELPER_H_

#include <cmath>

namespace MathHelper
{

template < typename T >
inline T DegToRad( const T & deg )
{
   return static_cast< T >(deg * M_PI / 180.0);
}

template < typename T >
inline T RadToDeg( const T & rad )
{
   return static_cast< T >(rad * 180.0 / M_PI);
}

template < typename T >
inline T Clamp( const T & value, const T & min, const T & max )
{
   return value < min ? min : value > max ? max : value;
}

} // namespace MathHelper

#endif // _MATH_HELPER_H_
