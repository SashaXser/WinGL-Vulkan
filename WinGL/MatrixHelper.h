#ifndef _MATRIX_HELPER_H_
#define _MATRIX_HELPER_H_

// crt includes
#include <cmath>

// local includes
#include "Matrix.h"
#include "Vector.h"

namespace MatrixHelper
{

template < typename T >
void DecomposeYawPitchRoll( const Matrix< T > & mat,
                            T * y, T * p, T * r )
{
   // this function needs to change as it assumes
   // a view matrix that pulls world space into eye space

   // only decomposes matrices that are
   // orthongonal and have been applied
   // in the yaw, pitch, and roll order
   if (y) *y = atan2(mat.mT[2], mat.mT[10]);
   if (p) *p = asin(-mat.mT[6]);
   if (r) *r = atan2(-mat.mT[4], mat.mT[5]);
}

template < typename T >
void DecomposeYawPitchRollDeg( const Matrix< T > & mat,
                               T * y, T * p, T * r )
{
   // only decomposes matrices that are
   // orthongonal and have been applied
   // in the yaw, pitch, and roll order
   DecomposeYawPitchRoll(mat, y, p, r);

   const double ratio = 180.0 / M_PI;
   if (y) *y = static_cast< T >(*y * ratio);
   if (p) *p = static_cast< T >(*p * ratio);
   if (r) *r = static_cast< T >(*r * ratio);
}

template < typename T >
Vector< T, 3 > DecomposeYawPitchRoll( const Matrix< T > & mat )
{
   Vector< T, 3 > ypr;

   DecomposeYawPitchRoll(mat, &ypr[0], &ypr[1], &ypr[2]);

   return ypr;
}

template < typename T >
Vector< T, 3 > DecomposeYawPitchRollDeg( const Matrix< T > & mat )
{
   Vector< T, 3 > ypr;

   DecomposeYawPitchRollDeg(mat, &ypr[0], &ypr[1], &ypr[2]);

   return ypr;
}

template < typename T >
Vector< T, 3 > GetStrafeVector( const Matrix< T > & mat )
{
   Vector< T, 3 > v;

   v.mT[0] = mat.mT[0];
   v.mT[1] = mat.mT[4];
   v.mT[2] = mat.mT[8];

   return v;
}

template < typename T >
Vector< T, 3 > GetViewVector( const Matrix< T > & mat )
{
   Vector< T, 3 > v;

   v.mT[0] = mat.mT[2];
   v.mT[1] = mat.mT[6];
   v.mT[2] = mat.mT[10];

   v *= -1;

   return v;
}

template < typename T >
Vector< T, 3 > GetUpVector( const Matrix< T > & mat )
{
   Vector< T, 3 > v;

   v.mT[0] = mat.mT[1];
   v.mT[1] = mat.mT[5];
   v.mT[2] = mat.mT[9];

   return v;
}

} // namespace MatrixHelper

#endif // _MATRIX_HELPER_H_
