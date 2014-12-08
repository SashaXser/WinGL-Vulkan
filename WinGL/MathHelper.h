#ifndef _MATH_HELPER_H_
#define _MATH_HELPER_H_

#include <cmath>
#include <limits>
#include <complex>

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

template < typename T >
inline bool Equals( const T & v1, const T & v2, const T & epsilon = std::numeric_limits< T >::epsilon() )
{
   return epsilon >= std::abs(v1 - v2);
}

template < typename T > T pi( ) { /* empty so as to error out */ }
template < > inline double pi( ) { return 3.14159265358979323846; }
template < > inline float pi( ) { return static_cast< float >(3.14159265358979323846); }

} // namespace MathHelper

#endif // _MATH_HELPER_H_
