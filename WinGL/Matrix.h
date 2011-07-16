#ifndef _MATRIX_H_
#define _MATRIX_H_

// std includes
#include <complex>

// crt includes
#define _USE_MATH_DEFINES
#include <math.h>
#include <memory.h>

// local includes
#include "Vector.h"
#include "WglAssert.h"

template < typename T >
class Matrix
{
public:
   // basic type of the class
   typedef T type;

   // constructor / destructor
            Matrix( );
            Matrix( const T t[16] );
            Matrix( const Matrix< T > & mat );
            template < typename U >
            Matrix( const Matrix< U > & mat );
            Matrix( const Vector< T > & col1, const Vector< T > & col2,
                    const Vector< T > & col3, const Vector< T > & col4 );
            template < typename U >
            Matrix( const Vector< U > & col1, const Vector< U > & col2,
                    const Vector< U > & col3, const Vector< U > & col4 );
            Matrix( const T & col1_x, const T & col1_y, const T & col1_z, const T & col1_w,
                    const T & col2_x, const T & col2_y, const T & col2_z, const T & col2_w,
                    const T & col3_x, const T & col3_y, const T & col3_z, const T & col3_w,
                    const T & col4_x, const T & col4_y, const T & col4_z, const T & col4_w );
            template < typename U >
            Matrix( const U & col1_x, const U & col1_y, const U & col1_z, const U & col1_w,
                    const U & col2_x, const U & col2_y, const U & col2_z, const U & col2_w,
                    const U & col3_x, const U & col3_y, const U & col3_z, const U & col3_w,
                    const U & col4_x, const U & col4_y, const U & col4_z, const U & col4_w );
           ~Matrix( );

   // operator =
   template < typename U >
   Matrix< T > & operator = ( const Matrix< U > & mat );
   Matrix< T > & operator = ( const Matrix< T > & mat );
   template < typename U >
   Matrix< T > & operator = ( const U u[16] );
   Matrix< T > & operator = ( const T t[16] );

   // operator *
   template < typename U >
   Matrix< T > operator * ( const Matrix< U > & mat ) const;
   Matrix< T > operator * ( const Matrix< T > & mat ) const;
   template < typename U >
   Matrix< T > & operator *= ( const Matrix< U > & mat );
   Matrix< T > & operator *= ( const Matrix< T > & mat );

   template < typename U >
   Vector< T > operator * ( const Vector< U > & vec ) const;
   Vector< T > operator * ( const Vector< T > & vec ) const;

   // operator ==
   template < typename U >
   bool operator == ( const Matrix< U > & mat ) const;
   bool operator == ( const Matrix< T > & mat ) const;

   // operator !=
   template < typename U >
   bool operator != ( const Matrix< U > & mat ) const;
   bool operator != ( const Matrix< T > & mat ) const;

   // operator T
   operator T * ( );
   operator const T * ( ) const;

   // creates an identity matrix
   void  MakeIdentity( );

   // creates a rotation matrix
   void  MakeRotation( const T & degrees, const T & x, const T & y, const T & z );
   void  MakeRotation( const T & degrees, const Vector< T > & vec );

   // creates a translation matrix
   void  MakeTranslation( const T & x, const T & y, const T & z );
   void  MakeTranslation( const Vector< T > & vec );

   // create a projection matrix
   template < typename U >
   void  MakeOrtho( const U & rLeft, const U & rRight,
                    const U & rBottom, const U & rTop,
                    const U & rNear, const U & rFar );
   void  MakeOrtho( const T & rLeft, const T & rRight,
                    const T & rBottom, const T & rTop,
                    const T & rNear, const T & rFar );

   template < typename U >
   void  MakeFrustum( const U & rLeft, const U & rRight,
                      const U & rBottom, const U & rTop,
                      const U & rNear, const U & rFar );
   void  MakeFrustum( const T & rLeft, const T & rRight,
                      const T & rBottom, const T & rTop,
                      const T & rNear, const T & rFar );

   template < typename U >
   void  MakePerspective( const U & rFOV,
                          const U & rAspect,
                          const U & rZNear,
                          const U & rZFar );
   void  MakePerspective( const T & rFOV,
                          const T & rAspect,
                          const T & rZNear,
                          const T & rZFar );

   // setup a viewing transformation
   template < typename U >
   void  MakeLookAt( const U & rEyeX, const U & rEyeY, const U & rEyeZ,
                     const U & rCenterX, const U & rCenterY, const U & rCenterZ,
                     const U & rUpX, const U & rUpY, const U & rUpZ );
   void  MakeLookAt( const T & rEyeX, const T & rEyeY, const T & rEyeZ,
                     const T & rCenterX, const T & rCenterY, const T & rCenterZ,
                     const T & rUpX, const T & rUpY, const T & rUpZ );

   template < typename U >
   void  MakeLookAt( const U * const pEye,
                     const U * const pCenter,
                     const U * const pUp );
   void  MakeLookAt( const T * const pEye,
                     const T * const pCenter,
                     const T * const pUp );

   template < typename U >
   void  MakeLookAt( const Vector< U > & rEye,
                     const Vector< U > & rCenter,
                     const Vector< U > & rUp );
   void  MakeLookAt( const Vector< T > & rEye,
                     const Vector< T > & rCenter,
                     const Vector< T > & rUp );

   template < typename U >
   bool GetFrustum( U & rLeft, U & rRight,
                    U & rBottom, U & rTop,
                    U & rZNear, U & rZFar ) const;
   bool GetFrustum( T & rLeft, T & rRight,
                    T & rBottom, T & rTop,
                    T & rZNear, T & rZFar ) const;
   template < typename U >
   bool GetFrustum( U pFrusVals[6] ) const;
   bool GetFrustum( T pFrusVals[6] ) const;

   template < typename U >
   bool GetPerspective( U & rFOV, U & rAspect,
                        U & rZNear, U & rZFar ) const;
   bool GetPerspective( T & rFOV, T & rAspect,
                        T & rZNear, T & rZFar ) const;
   template < typename U >
   bool GetPerspective( U pFrusVals[4] ) const;
   bool GetPerspective( T pFrusVals[4] ) const;

   // screen space tranlation matrix
   template < typename U >
   void MakeScreenSpaceMatrix( );
   void MakeScreenSpaceMatrix( );

   // transpose
   void        MakeTranspose( );
   Matrix< T > Transpose( ) const;

   // makes the inverse out of an
   // assumed orthogonal matrix
   void        MakeInverseFromOrthogonal( );
   Matrix< T > InverseFromOrthogonal( ) const;

   // makes the inverse out of an
   // assumed affine matrix
   void        MakeInverse( );
   Matrix< T > Inverse( ) const;

   // returns the determinant
   T  Determinant( ) const;

   // matrix class should be simple and allow
   // for easy acess to the member variables
   T     mT[16];

};

template < typename T >
inline Matrix< T >::Matrix( )
{
   // set the identity matrix
   MakeIdentity();
}

template < typename T >
inline Matrix< T >::Matrix( const T t[16] )
{
   memcpy(mT, t, sizeof(mT));
}

template < typename T >
inline Matrix< T >::Matrix( const Matrix< T > & mat )
{
   *this = mat;
}

template < typename T >
template < typename U >
inline Matrix< T >::Matrix( const Matrix< U > & mat )
{
   *this = mat;
}

template < typename T >
inline Matrix< T >::Matrix( const Vector< T > & col1,
                            const Vector< T > & col2,
                            const Vector< T > & col3,
                            const Vector< T > & col4 )
{
   memcpy(mT, col1.mT, sizeof(col1.mT));
   memcpy(mT + 4, col2.mT, sizeof(col2.mT));
   memcpy(mT + 8, col3.mT, sizeof(col3.mT));
   memcpy(mT + 12, col4.mT, sizeof(col4.mT));
}

template < typename T >
template < typename U >
inline Matrix< T >::Matrix( const Vector< U > & col1,
                            const Vector< U > & col2,
                            const Vector< U > & col3,
                            const Vector< U > & col4 )
{
   *this = Matrix< T >(Vector< T >(col1),
                       Vector< T >(col2),
                       Vector< T >(col3),
                       Vector< T >(col4));
}

template < typename T >
inline Matrix< T >::Matrix( const T & col1_x, const T & col1_y, const T & col1_z, const T & col1_w,
                            const T & col2_x, const T & col2_y, const T & col2_z, const T & col2_w,
                            const T & col3_x, const T & col3_y, const T & col3_z, const T & col3_w,
                            const T & col4_x, const T & col4_y, const T & col4_z, const T & col4_w )
{
   mT[0] = col1_x;   mT[4] = col2_x;   mT[8]  = col3_x;   mT[12] = col4_x;
   mT[1] = col1_y;   mT[5] = col2_y;   mT[9]  = col3_y;   mT[13] = col4_y;
   mT[2] = col1_z;   mT[6] = col2_z;   mT[10] = col3_z;   mT[14] = col4_z;
   mT[3] = col1_w;   mT[7] = col2_w;   mT[11] = col3_w;   mT[15] = col4_w;
}

template < typename T >
template < typename U >
inline Matrix< T >::Matrix( const U & col1_x, const U & col1_y, const U & col1_z, const U & col1_w,
                            const U & col2_x, const U & col2_y, const U & col2_z, const U & col2_w,
                            const U & col3_x, const U & col3_y, const U & col3_z, const U & col3_w,
                            const U & col4_x, const U & col4_y, const U & col4_z, const U & col4_w )
{
   *this = Matrix< T >(Vector< T >(col1_x, col1_y, col1_z, col1_w),
                       Vector< T >(col2_x, col2_y, col2_z, col2_w),
                       Vector< T >(col3_x, col3_y, col3_z, col3_w),
                       Vector< T >(col4_x, col4_y, col4_z, col4_w));
}

template < typename T >
inline Matrix< T >::~Matrix( )
{
}

template < typename T >
template < typename U >
inline Matrix< T > & Matrix< T >::operator = ( const Matrix< U > & mat )
{
   for (int i = 0; i < 16; ++i)
      mT[i] = mat.mT[i];

   return *this;
}

template < typename T >
inline Matrix< T > & Matrix< T >::operator = ( const Matrix< T > & mat )
{
   if (this != &mat)
      memcpy(mT, mat.mT, sizeof(mT));

   return *this;
}

template < typename T >
template < typename U >
inline Matrix< T > & Matrix< T >::operator = ( const U u[16] )
{
   Matrix< U > localMat(u);

   *this = localMat;

   return *this;
}

template < typename T >
inline Matrix< T > & Matrix< T >::operator = ( const T t[16] )
{
   memcpy(mT, t, sizeof(mT));

   return *this;
}

template < typename T >
template < typename U >
inline Matrix< T > Matrix< T >::operator * ( const Matrix< U > & mat ) const
{
   Matrix< T > localMat(mat);

   return *this * localMat;
}

template < typename T >
inline Matrix< T > Matrix< T >::operator * ( const Matrix< T > & mat ) const
{
   // note: post multiplication used to conform to opengl

   Matrix< T > localMat;

   T * lmMT = localMat.mT;

   for (int i = 0; i < 4; ++i, lmMT += 4)
   {
      const unsigned int row = i * 4;
      const T * const mT0 = mat.mT + row + 0;
      const T * const mT1 = mat.mT + row + 1;
      const T * const mT2 = mat.mT + row + 2;
      const T * const mT3 = mat.mT + row + 3;

      *(lmMT + 0) = *(mT + 0)  * *mT0 +
                    *(mT + 4)  * *mT1 +
                    *(mT + 8)  * *mT2 +
                    *(mT + 12) * *mT3;
                              
      *(lmMT + 1) = *(mT + 1)  * *mT0 +
                    *(mT + 5)  * *mT1 +
                    *(mT + 9)  * *mT2 +
                    *(mT + 13) * *mT3;
                              
      *(lmMT + 2) = *(mT + 2)  * *mT0 +
                    *(mT + 6)  * *mT1 +
                    *(mT + 10) * *mT2 +
                    *(mT + 14) * *mT3;
                              
      *(lmMT + 3) = *(mT + 3)  * *mT0 +
                    *(mT + 7)  * *mT1 +
                    *(mT + 11) * *mT2 +
                    *(mT + 15) * *mT3;
   }

   return localMat;
}

template < typename T >
template < typename U >
Matrix< T > & Matrix< T >::operator *= ( const Matrix< U > & mat )
{
   Matrix< T > m(mat);

   return *(*this *= m);
}

template < typename T >
Matrix< T > & Matrix< T >::operator *= ( const Matrix< T > & mat )
{
   if (this != &mat)
      *this = (*this * mat);

   return *this;
}

template < typename T >
template < typename U >
inline Vector< T > Matrix< T >::operator * ( const Vector< U > & vec ) const
{
   return *this * Vector< T >(vec);
}

template < typename T >
inline Vector< T > Matrix< T >::operator * ( const Vector< T > & vec ) const
{
   // note: post multiplication used to conform to opengl

   const T * const pT = vec.mT;

   T x = *(mT + 0)  * *(pT + 0) +
         *(mT + 4)  * *(pT + 1) +
         *(mT + 8)  * *(pT + 2) +
         *(mT + 12) * *(pT + 3);

   T y = *(mT + 1)  * *(pT + 0) +
         *(mT + 5)  * *(pT + 1) +
         *(mT + 9)  * *(pT + 2) +
         *(mT + 13) * *(pT + 3);

   T z = *(mT + 2)  * *(pT + 0) +
         *(mT + 6)  * *(pT + 1) +
         *(mT + 10) * *(pT + 2) +
         *(mT + 14) * *(pT + 3);

   T w = *(mT + 3)  * *(pT + 0) +
         *(mT + 7)  * *(pT + 1) +
         *(mT + 11) * *(pT + 2) +
         *(mT + 15) * *(pT + 3);

   return Vector< T >(x, y, z, w);
}

template < typename T >
inline Matrix< T >::operator T * ( )
{
   return mT;
}

template < typename T >
inline Matrix< T >::operator const T * ( ) const
{
   return mT;
}

template < typename T >
template < typename U >
inline bool Matrix< T >::operator == ( const Matrix< U > & mat ) const
{
   return *this == Matrix< T >(mat);
}

template < typename T >
inline bool Matrix< T >::operator == ( const Matrix< T > & mat ) const
{
   // epsilon value when comparing
   const double eps = 0.0000001;

   for (int i = 0; i < 16; ++i)
      if (((mT[i] + eps) < mat.mT[i]) || (mat.mT[i] < (mT[i] - eps)))
         return false;

   return true;
}

template < typename T >
template < typename U >
inline bool Matrix< T >::operator != ( const Matrix< U > & mat ) const
{
   return !(*this == Matrix< T >(mat));
}

template < typename T >
inline bool Matrix< T >::operator != ( const Matrix< T > & mat ) const
{
   return !(*this == mat);
}

template < typename T >
inline void Matrix< T >::MakeIdentity( )
{
   memset(mT, 0x00, sizeof(mT));

   mT[0] = 1;
   mT[5] = 1;
   mT[10] = 1;
   mT[15] = 1;
}

template < typename T >
inline void Matrix< T >::MakeRotation( const T & degrees, const T & x, const T & y, const T & z )
{
   MakeRotation(degrees, Vector< T >(x, y, z));
}

template < typename T >
inline void Matrix< T >::MakeRotation( const T & degrees, const Vector< T > & vec )
{
   // validate a unit vector...
   WGL_ASSERT(0.9999999999 <= vec.Length() && vec.Length() <= 1.0000000001);

   const T radians = degrees * M_PI / 180.0;

   const T cosine = std::cos(radians);
   const T sine = std::sin(radians);
   const T oneMinCos = 1 - cosine;
   const T xy = vec.mT[0] * vec.mT[1];
   const T xz = vec.mT[0] * vec.mT[2];
   const T yz = vec.mT[1] * vec.mT[2];
   const T xsin = vec.mT[0] * sine;
   const T ysin = vec.mT[1] * sine;
   const T zsin = vec.mT[2] * sine;
   const T xx = vec.mT[0] * vec.mT[0];
   const T yy = vec.mT[1] * vec.mT[1];
   const T zz = vec.mT[2] * vec.mT[2];

   mT[0] = xx * oneMinCos + cosine;    mT[4] = xy * oneMinCos - zsin;      mT[8] = xz * oneMinCos + ysin;      mT[12] = 0;
   mT[1] = xy * oneMinCos + zsin;      mT[5] = yy * oneMinCos + cosine;    mT[9] = yz * oneMinCos - xsin;      mT[13] = 0;
   mT[2] = xz * oneMinCos - ysin;      mT[6] = yz * oneMinCos + xsin;      mT[10] = zz * oneMinCos + cosine;   mT[14] = 0;
   mT[3] = 0;                          mT[7] = 0;                          mT[11] = 0;                         mT[15] = 1;
}

template < typename T >
template < typename U >
bool Matrix< T >::GetFrustum( U & rLeft, U & rRight,
                              U & rBottom, U & rTop,
                              U & rZNear, U & rZFar ) const
{
   T l = 0, r = 0;
   T b = 0, t = 0;
   T n = 0, f = 0;

   if (GetFrustum(l, r, b, t, n, r))
   {
      rLeft = l;
      rRight = r;
      rBottom = b;
      rTop = t;
      rZNear = n;
      rZFar = f;

      return true;
   }

   return false;
}

template < typename T >
bool Matrix< T >::GetFrustum( T & rLeft, T & rRight,
                              T & rBottom, T & rTop,
                              T & rZNear, T & rZFar ) const
{
   if (mT[3]  ==  0.0 && mT[7]  == 0.0 &&
       mT[11] == -1.0 && mT[15] == 0.0)
   {
      rZNear = mT[14] / (mT[10] - 1.0);
      rZFar = mT[14] / (mT[10] + 1.0);

      rLeft = rZNear * (mT[8] - 1.0) / mT[0];
      rRight = rZNear * (mT[8] + 1.0) / mT[0];

      rTop = rZNear * (mT[9] + 1.0) / mT[5];
      rBottom = rZNear * (mT[9] - 1.0) / mT[5];

      return true;
   }

   return false;
}

template < typename T >
template < typename U >
bool Matrix< T >::GetFrustum( U pFrusVals[6] ) const
{
   return GetFrustum(pFrusVals[0], pFrusVals[1],
                     pFrusVals[2], pFrusVals[3],
                     pFrusVals[4], pFrusVals[5]);
}

template < typename T >
bool Matrix< T >::GetFrustum( T pFrusVals[6] ) const
{
   return GetFrustum(pFrusVals[0], pFrusVals[1],
                     pFrusVals[2], pFrusVals[3],
                     pFrusVals[4], pFrusVals[5]);
}

template < typename T >
template < typename U >
bool Matrix< T >::GetPerspective( U & rFOV, U & rAspect,
                                  U & rZNear, U & rZFar ) const
{
   T fov = 0, asp = 0;
   T n = 0, f = 0;

   if (GetPerspective(fov, asp, n, f))
   {
      rFOV = fov;
      rAspect = asp;
      rZNear = n;
      rZFar = f;

      return true;
   }

   return false;
}

template < typename T >
bool Matrix< T >::GetPerspective( T & rFOV, T & rAspect,
                                  T & rZNear, T & rZFar ) const
{
   T l = 0, r = 0;
   T b = 0, t = 0;

   if (GetFrustum(l, r, b, t, rZNear, rZFar))
   {
      rFOV = (atan(t / rZNear) - atan(b / rZNear)) * 180.0 / M_PI;
      rAspect = (r - l) / (t - b);

      return true;
   }

   return false;
}

template < typename T >
template < typename U >
bool Matrix< T >::GetPerspective( U pFrusVals[4] ) const
{
   return GetPerspective(pFrusVals[0], pFrusVals[1],
                         pFrusVals[2], pFrusVals[3]);
}

template < typename T >
bool Matrix< T >::GetPerspective( T pFrusVals[4] ) const
{
   return GetPerspective(pFrusVals[0], pFrusVals[1],
                         pFrusVals[2], pFrusVals[3]);
}

template < typename T >
template < typename U >
void Matrix< T >::MakeScreenSpaceMatrix( const U & x, const U & y,
                                         const U & width, const U & height )
{
   const T values[] =
   {
      x, y, width, height;
   };

   MakeScreenSpaceMatrix(values[0], values[1],
                         values[2], values[3])
}

template < typename T >
void Matrix< T >::MakeScreenSpaceMatrix( const T & x, const T & y,
                                         const T & width, const T & height )
{
   MakeIdentity();

   mT[0] = width / 2.0;
   mT[5] = height / 2.0;
   mT[10] = 0.5;
   mT[12] = width / 2.0 + x;
   mT[13] = height / 2.0 + y;
   mT[14] = 0.5;
}

template < typename T >
inline void Matrix< T >::MakeTranslation( const T & x, const T & y, const T & z )
{
   MakeIdentity();

   mT[12] = x;
   mT[13] = y;
   mT[14] = z;
}

template < typename T >
inline void Matrix< T >::MakeTranslation( const Vector< T > & vec )
{
   MakeTranslation(vec.mT[0], vec.mT[1], vec.mT[2]);
}

template < typename T >
template < typename U >
inline void Matrix< T >::MakeOrtho( const U & rLeft, const U & rRight,
                                    const U & rBottom, const U & rTop,
                                    const U & rNear, const U & rFar )
{
   const T tOValues[] = 
   {
      rLeft, rRight,
      rBottom, rTop,
      rNear, rFar
   };

   MakeOrtho(tOValues[0], tOValues[1],
             tOValues[2], tOValues[3],
             tOValues[4], tOValues[5]);
}

template < typename T >
inline void Matrix< T >::MakeOrtho( const T & rLeft, const T & rRight,
                                    const T & rBottom, const T & rTop,
                                    const T & rNear, const T & rFar )
{
   const T SX = 2.0 / (rRight - rLeft);
   const T SY = 2.0 / (rTop - rBottom);
   const T SZ = -2.0 / (rFar - rNear);
   const T TX = -(rRight + rLeft) / (rRight - rLeft);
   const T TY = -(rTop + rBottom) / (rTop - rBottom);
   const T TZ = -(rFar + rNear) / (rFar - rNear);

   mT[0] = SX;  mT[4] = 0;   mT[8]  = 0;   mT[12] = TX;
   mT[1] = 0;   mT[5] = SY;  mT[9]  = 0;   mT[13] = TY;
   mT[2] = 0;   mT[6] = 0;   mT[10] = SZ;  mT[14] = TZ;
   mT[3] = 0;   mT[7] = 0;   mT[11] = 0;   mT[15] = 1;
}

template < typename T >
template < typename U >
inline void Matrix< T >::MakeFrustum( const U & rLeft, const U & rRight,
                                      const U & rBottom, const U & rTop,
                                      const U & rNear, const U & rFar )
{
   const T tFValues[] =
   {
      rLeft, rRight,
      rBottom, rTop,
      rNear, rFar
   };

   MakeFrustum(tFValues[0], tFValues[1],
               tFValues[2], tFValues[3],
               tFValues[4], tFValues[5]);
}

template < typename T >
inline void Matrix< T >::MakeFrustum( const T & rLeft, const T & rRight,
                                      const T & rBottom, const T & rTop,
                                      const T & rNear, const T & rFar )
{
   const T SX = 2.0 * rNear / (rRight - rLeft);
   const T SY = 2.0 * rNear / (rTop - rBottom);
   const T A = (rRight + rLeft) / (rRight - rLeft);
   const T B = (rTop + rBottom) / (rTop - rBottom);
   const T C = -(rFar + rNear) / (rFar - rNear);
   const T D = -(2.0 * rFar * rNear) / (rFar - rNear);

   mT[0] = SX;  mT[4] = 0;   mT[8]  = A;   mT[12] = 0;
   mT[1] = 0;   mT[5] = SY;  mT[9]  = B;   mT[13] = 0;
   mT[2] = 0;   mT[6] = 0;   mT[10] = C;   mT[14] = D;
   mT[3] = 0;   mT[7] = 0;   mT[11] = -1;  mT[15] = 0;
}

template < typename T >
template < typename U >
inline void Matrix< T >::MakePerspective( const U & rFOV,
                                          const U & rAspect,
                                          const U & rZNear,
                                          const U & rZFar )
{
   const T tFOV = rFOV;
   const T tAspect = rAspect;
   const T tZNear = rZNear;
   const T tZFar = rZFar;

   MakePerspective(tFOV, tAspect, tZNear, tZFar);
}

template < typename T >
inline void Matrix< T >::MakePerspective( const T & rFOV,
                                          const T & rAspect,
                                          const T & rZNear,
                                          const T & rZFar )
{
   const T angle = rFOV * M_PI / 180.0 * 0.5;
   const T top = rZNear * std::tan(angle);
   const T right = rAspect * top;

   MakeFrustum(-right, right,
               -top, top,
               rZNear, rZFar);
}

template < typename T >
template < typename U >
inline void Matrix< T >::MakeLookAt( const U & rEyeX, const U & rEyeY, const U & rEyeZ,
                                     const U & rCenterX, const U & rCenterY, const U & rCenterZ,
                                     const U & rUpX, const U & rUpY, const U & rUpZ )
{
   MakeLookAt(Vector< T >(rEyeX, rEyeY, rEyeZ),
              Vector< T >(rCenterX, rCenterY, rCenterZ),
              Vector< T >(rUpX, rUpY, rUpZ));
}

template < typename T >
inline void Matrix< T >::MakeLookAt( const T & rEyeX, const T & rEyeY, const T & rEyeZ,
                                     const T & rCenterX, const T & rCenterY, const T & rCenterZ,
                                     const T & rUpX, const T & rUpY, const T & rUpZ )
{
   MakeLookAt(Vector< T >(rEyeX, rEyeY, rEyeZ),
              Vector< T >(rCenterX, rCenterY, rCenterZ),
              Vector< T >(rUpX, rUpY, rUpZ));
}

template < typename T >
template < typename U >
inline void Matrix< T >::MakeLookAt( const U * const pEye,
                                     const U * const pCenter,
                                     const U * const pUp )
{
   MakeLookAt(Vector< T >(pEye[0], pEye[1], pEye[2]),
              Vector< T >(pCenter[0], pCenter[1], pCenter[2]),
              Vector< T >(pUp[0], pUp[1], pUp[2]));
}

template < typename T >
inline void Matrix< T >::MakeLookAt( const T * const pEye,
                                     const T * const pCenter,
                                     const T * const pUp )
{
   MakeLookAt(Vector< T >(pEye[0], pEye[1], pEye[2]),
              Vector< T >(pCenter[0], pCenter[1], pCenter[2]),
              Vector< T >(pUp[0], pUp[1], pUp[2]));
}

template < typename T >
template < typename U >
inline void Matrix< T >::MakeLookAt( const Vector< U > & rEye,
                                     const Vector< U > & rCenter,
                                     const Vector< U > & rUp )
{
   MakeLookAt(Vector< T >(rEye),
              Vector< T >(rCenter),
              Vector< T >(rUp));
}

template < typename T >
inline void Matrix< T >::MakeLookAt( const Vector< T > & rEye,
                                     const Vector< T > & rCenter,
                                     const Vector< T > & rUp )
{
   // obtain the z-axis vector...
   // look at vector is in the opposite direction
   // since OpenGL z-axis is a right handed system...
   const Vector< T > vZ = (rEye - rCenter).MakeUnitVector();
   // obtain the x-axis vector...
   const Vector< T > vX = (rUp ^ vZ).MakeUnitVector();
   // obtain the y-axis vector...
   const Vector< T > vY = vZ ^ vX;

   // setup the matrix based on the new x, y, z values...
   mT[0] = vX.mT[0]; mT[4] = vX.mT[1]; mT[8]  = vX.mT[2];  mT[12] = -(rEye * vX);
   mT[1] = vY.mT[0]; mT[5] = vY.mT[1]; mT[9]  = vY.mT[2];  mT[13] = -(rEye * vY);
   mT[2] = vZ.mT[0]; mT[6] = vZ.mT[1]; mT[10] = vZ.mT[2];  mT[14] = -(rEye * vZ);
   mT[3] = 0;        mT[7] = 0;        mT[11] = 0;         mT[15] = 1;
}

template < typename T >
inline void Matrix< T >::MakeTranspose( )
{
   Matrix< T > mat(*this);

 /*mT[0] = mat.mT[0];*/ mT[4] = mat.mT[1];   mT[8]  = mat.mT[2];    mT[12] = mat.mT[3];
   mT[1] = mat.mT[4]; /*mT[5] = mat.mT[5];*/ mT[9]  = mat.mT[6];    mT[13] = mat.mT[7];
   mT[2] = mat.mT[8];   mT[6] = mat.mT[9]; /*mT[10] = mat.mT[10];*/ mT[14] = mat.mT[11];
   mT[3] = mat.mT[12];  mt[7] = mat.mT[13];  mT[11] = mat.mT[14]; /*mt[15] = mat.mT[15];*/
}

template < typename T >
inline Matrix< T > Matrix< T >::Transpose( ) const
{
   Matrix< T > mat(*this);

   mat.MakeTranspose();

   return mat;
}

template < typename T >
inline void Matrix< T >::MakeInverseFromOrthogonal( )
{
//   VALIDATE_ORTHOGONAL_MATRIX();

   const Matrix< T > matTemp(*this);

   for (int i = 0; i < 3; ++i)
      for (int j = 0; j < 3; ++j)
         *(mT + i * 4 + j) = *(matTemp.mT + j * 4 + i);

   const T * origT = &matTemp.mT[12];

   mT[12] = -(mT[0] * origT[0] + mT[4] * origT[1] + mT[8]  * origT[2]);
   mT[13] = -(mT[1] * origT[0] + mT[5] * origT[1] + mT[9]  * origT[2]);
   mT[14] = -(mT[2] * origT[0] + mT[6] * origT[1] + mT[10] * origT[2]);
}

template < typename T >
inline Matrix< T > Matrix< T >::InverseFromOrthogonal( ) const
{
   Matrix< T > mat(*this);

   mat.MakeInverseFromOrthogonal();

   return mat;
}

template < typename T >
inline void Matrix< T >::MakeInverse( )
{
   // obtain the determinant
   // determinant function is not being called as
   // there are similarities between multiplications
   // in the calculation of the determinant and the
   // multiplications of the adjoint matrix...
   const T a0 = mT[0]  * mT[5]  - mT[1]  * mT[4];
   const T a1 = mT[0]  * mT[6]  - mT[2]  * mT[4];
   const T a2 = mT[0]  * mT[7]  - mT[3]  * mT[4];
   const T a3 = mT[1]  * mT[6]  - mT[2]  * mT[5];
   const T a4 = mT[1]  * mT[7]  - mT[3]  * mT[5];
   const T a5 = mT[2]  * mT[7]  - mT[3]  * mT[6];
   const T b0 = mT[8]  * mT[13] - mT[9]  * mT[12];
   const T b1 = mT[8]  * mT[14] - mT[10] * mT[12];
   const T b2 = mT[8]  * mT[15] - mT[11] * mT[12];
   const T b3 = mT[9]  * mT[14] - mT[10] * mT[13];
   const T b4 = mT[9]  * mT[15] - mT[11] * mT[13];
   const T b5 = mT[10] * mT[15] - mT[11] * mT[14];

   const T inverseDet =
      1.0 / (a0 * b5 - a1 * b4 + a2 * b3 + 
             a3 * b2 - a4 * b1 + a5 * b0);

   // make sure there is an inverse
   WGL_ASSERT(-0.0000000001 > (1.0 / inverseDet) ||
              0.0000000001 < (1.0 / inverseDet));

   // create the inverse matrix
   Matrix< T > matInv;
   matInv.mT[0]  = + mT[5]  * b5 - mT[6]  * b4 + mT[7]  * b3;
   matInv.mT[4]  = - mT[4]  * b5 + mT[6]  * b2 - mT[7]  * b1;
   matInv.mT[8]  = + mT[4]  * b4 - mT[5]  * b2 + mT[7]  * b0;
   matInv.mT[12] = - mT[4]  * b3 + mT[5]  * b1 - mT[6]  * b0;
   matInv.mT[1]  = - mT[1]  * b5 + mT[2]  * b4 - mT[3]  * b3;
   matInv.mT[5]  = + mT[0]  * b5 - mT[2]  * b2 + mT[3]  * b1;
   matInv.mT[9]  = - mT[0]  * b4 + mT[1]  * b2 - mT[3]  * b0;
   matInv.mT[13] = + mT[0]  * b3 - mT[1]  * b1 + mT[2]  * b0;
   matInv.mT[2]  = + mT[13] * a5 - mT[14] * a4 + mT[15] * a3;
   matInv.mT[6]  = - mT[12] * a5 + mT[14] * a2 - mT[15] * a1;
   matInv.mT[10] = + mT[12] * a4 - mT[13] * a2 + mT[15] * a0;
   matInv.mT[14] = - mT[12] * a3 + mT[13] * a1 - mT[14] * a0;
   matInv.mT[3]  = - mT[9]  * a5 + mT[10] * a4 - mT[11] * a3;
   matInv.mT[7]  = + mT[8]  * a5 - mT[10] * a2 + mT[11] * a1;
   matInv.mT[11] = - mT[8]  * a4 + mT[9]  * a2 - mT[11] * a0;
   matInv.mT[15] = + mT[8]  * a3 - mT[9]  * a1 + mT[10] * a0;

   // copy the inverse properties
   *this = matInv;
}

template < typename T >
inline Matrix< T > Matrix< T >::Inverse( ) const
{
   Matrix< T > mat(*this);

   mat.MakeInverse();

   return mat;
}

template < typename T >
inline T Matrix< T >::Determinant( ) const
{
   /*
    the following determinant is calculated below... terms
    can be combined to reduce the number of similar
    multiplication requests for the cpu...
   (mT[0] * mT[5]  * (mT[10] * mT[15] - mT[11] * mT[14]) -
    mT[0] * mT[9]  * (mT[6]  * mT[15] - mT[7]  * mT[14]) +
    mT[0] * mT[13] * (mT[6]  * mT[11] - mT[7]  * mT[10])) -

   (mT[4] * mT[1]  * (mT[10] * mT[15] - mT[11] * mT[14]) -
    mT[4] * mT[9]  * (mT[2]  * mT[15] - mT[3]  * mT[14]) +
    mT[4] * mT[13] * (mT[2]  * mT[11] - mT[3]  * mT[10])) +

   (mT[8] * mT[1]  * (mT[6]  * mT[15] - mT[7]  * mT[14]) -
    mT[8] * mT[5]  * (mT[2]  * mT[15] - mT[3]  * mT[14]) +
    mT[8] * mT[13] * (mT[2]  * mT[7]  - mT[3]  * mT[6])) -

   (mT[12] * mT[1] * (mT[6]  * mT[11] - mT[7]  * mT[10]) -
    mT[12] * mT[5] * (mT[2]  * mT[11] - mT[3]  * mT[10]) +
    mT[12] * mT[9] * (mT[2]  * mT[7]  - mT[3]  * mT[6]))
    */

   const T a0 = mT[0]  * mT[5]  - mT[1]  * mT[4];
   const T a1 = mT[0]  * mT[6]  - mT[2]  * mT[4];
   const T a2 = mT[0]  * mT[7]  - mT[3]  * mT[4];
   const T a3 = mT[1]  * mT[6]  - mT[2]  * mT[5];
   const T a4 = mT[1]  * mT[7]  - mT[3]  * mT[5];
   const T a5 = mT[2]  * mT[7]  - mT[3]  * mT[6];
   const T b0 = mT[8]  * mT[13] - mT[9]  * mT[12];
   const T b1 = mT[8]  * mT[14] - mT[10] * mT[12];
   const T b2 = mT[8]  * mT[15] - mT[11] * mT[12];
   const T b3 = mT[9]  * mT[14] - mT[10] * mT[13];
   const T b4 = mT[9]  * mT[15] - mT[11] * mT[13];
   const T b5 = mT[10] * mT[15] - mT[11] * mT[14];

   return a0 * b5 - a1 * b4 + a2 * b3 + 
          a3 * b2 - a4 * b1 + a5 * b0;
}

// global typedefs
typedef Matrix< float > Matrixf;
typedef Matrix< double > Matrixd;

#endif // _MATRIX_H_