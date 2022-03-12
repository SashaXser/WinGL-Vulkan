#ifndef _QUATERNION_H_
#define _QUATERNION_H_

// local includes
#include "Matrix.h"
#include "Vector.h"
#include "WglAssert.h"
#include "MathHelper.h"

// std includes
#include <cmath>
#include <limits>
#include <cstdint>
#include <utility>

template < typename T >
class Quaternion
{
public:
   // public typedefs
   typedef T type;

   // constructor / destructor
    Quaternion( );
    template < typename U >
    Quaternion( const U & x, const U & y, const U & z, const U & w );
    Quaternion( const T & x, const T & y, const T & z, const T & w );
    template < typename U >
    Quaternion( const Quaternion< U > & quat );
    Quaternion( const Quaternion< T > & quat );
    template < typename U >
    Quaternion( const Matrix< U > & mat );
    Quaternion( const Matrix< T > & mat );
   ~Quaternion( );

   // operator =
   template < typename U >
   Quaternion< T > & operator = ( const Quaternion< U > & quat );
   Quaternion< T > & operator = ( const Quaternion< T > & quat );

   // operator ==
   template < typename U >
   bool operator == ( const Quaternion< U > & quat ) const;
   bool operator == ( const Quaternion< T > & quat ) const;

   // operator !=
   template < typename U >
   bool operator != ( const Quaternion< U > & quat ) const;
   bool operator != ( const Quaternion< T > & quat ) const;

   // type cast operator from one quaternion type to another
   template < typename U >
   operator Quaternion< U > ( ) const
   {
      return Quaternion< U >(static_cast< U >(mT[0]),
                             static_cast< U >(mT[1]),
                             static_cast< U >(mT[2]),
                             static_cast< U >(mT[3]));
   }

   // type cast operator to get a matrix
   operator Matrix< T > ( ) const
   {
      return Matrix< T > { };
   }

   // returns a matrix representation of the quat
   Matrix< T > ToMatrix( ) const;

   // operator * (dot product)
   template < typename U >
   Quaternion< T > operator * ( const Quaternion< U > & quat ) const;
   Quaternion< T > operator * ( const Quaternion< T > & quat ) const;
   template < typename U >
   Quaternion< T > & operator *= ( const Quaternion< U > & quat );
   Quaternion< T > & operator *= ( const Quaternion< T > & quat );

   // operator % (multiplication)
   template < typename U >
   Quaternion< T > operator % ( const Quaternion< U > & quat ) const;
   Quaternion< T > operator % ( const Quaternion< T > & quat ) const;
   template < typename U >
   Quaternion< T > & operator %= ( const Quaternion< U > & quat );
   Quaternion< T > & operator %= ( const Quaternion< T > & quat );

   template < typename U >
   Quaternion< T > operator % ( const Vector< U, 3 > & vec ) const;
   Quaternion< T > operator % ( const Vector< T, 3 > & vec ) const;
   template < typename U >
   Quaternion< T > & operator %= ( const Vector< U, 3 > & vec );
   Quaternion< T > & operator %= ( const Vector< T, 3 > & vec );

   // operator + (addition)
   template < typename U >
   Quaternion< T > operator + ( const Quaternion< U > & quat ) const;
   Quaternion< T > operator + ( const Quaternion< T > & quat ) const;
   template < typename U >
   Quaternion< T > & operator + ( const Quaternion< U > & quat );
   Quaternion< T > & operator + ( const Quaternion< T > & quat );

   // operator [ ]
   T & operator [ ] ( const uint32_t i ) { WGL_ASSERT(3 >= i); return mT[i]; }
   const T & operator [ ] ( const uint32_t i ) const { WGL_ASSERT(3 >= i); return mT[i]; }

   // pointer operator to components
   operator T * ( ) { return mT; }
   operator const T * ( ) const { return mT; }

   // rotation operation
   template < typename U >
   Quaternion< T > Rotate( const U & degrees, const Vector< U, 3 > & vec );
   Quaternion< T > Rotate( const T & degrees, const Vector< T, 3 > & vec );
   template < typename U >
   Quaternion< T > Rotate( const Quaternion< U > & quat );
   Quaternion< T > Rotate( const Quaternion< T > & quat );

   // basic access to the components
   T & X( ) { return mT[0]; }
   T & Y( ) { return mT[1]; }
   T & Z( ) { return mT[2]; }
   T & W( ) { return mT[3]; }

   const T & X( ) const { return mT[0]; }
   const T & Y( ) const { return mT[1]; }
   const T & Z( ) const { return mT[2]; }
   const T & W( ) const { return mT[3]; }

   T RealPart( ) const { return mT[3]; }
   Vector< T, 3 > ImaginaryPart( ) const { return Vector< T, 3 >(mT[0], mT[1], mT[2]); }

   // normalizes the quat
   Quaternion< T > & Normalize( );
   Quaternion< T > UnitQuaternion( ) const;

   // calculates the conjugate
   Quaternion< T > & Conjugate( );
   Quaternion< T > ConjugateQuaternion( ) const;

   // calculates the inverse
   Quaternion< T > & Inverse( );
   Quaternion< T > InverseQuaternion( ) const;

   // calculates the length
   T Length( ) const
   {
      return std::sqrt(mT[0] * mT[0] + mT[1] * mT[1] + mT[2] * mT[2] + mT[3] * mT[3]);
   }

   // static helper function to convert matrix to quat
   static Quaternion< T > ToQuaternion( const Matrix< T > & mat );

   // static helper function to create rotation quat from two vectors
   static Quaternion< T > Rotation( const T & degrees, const Vector< T, 3 > & axis );
   static Quaternion< T > Rotation( const T & degrees, const Vector< T, 4 > & axis );
   static Quaternion< T > Rotation( const Vector< T, 3 > & origin, const Vector< T, 3 > & destination );
   static Quaternion< T > Rotation( const Vector< T, 4 > & origin, const Vector< T, 4 > & destination );

private:
   // components that define the quat
   T     mT[4];

};

template < typename T >
Quaternion< T >::Quaternion( )
{
   mT[0] = 0;
   mT[1] = 0;
   mT[2] = 0;
   mT[3] = 1;
}

template < typename T >
template < typename U >
Quaternion< T >::Quaternion( const U & x, const U & y, const U & z, const U & w )
{
   mT[0] = x;
   mT[1] = y;
   mT[2] = z;
   mT[3] = w;
}

template < typename T >
Quaternion< T >::Quaternion( const T & x, const T & y, const T & z, const T & w )
{
   mT[0] = x;
   mT[1] = y;
   mT[2] = z;
   mT[3] = w;
}

template < typename T >
template < typename U >
Quaternion< T >::Quaternion( const Quaternion< U > & quat )
{
   mT[0] = quat.mT[0];
   mT[1] = quat.mT[1];
   mT[2] = quat.mT[2];
   mT[3] = quat.mT[3];
}

template < typename T >
Quaternion< T >::Quaternion( const Quaternion< T > & quat )
{
   mT[0] = quat.mT[0];
   mT[1] = quat.mT[1];
   mT[2] = quat.mT[2];
   mT[3] = quat.mT[3];
}

template < typename T >
template < typename U >
Quaternion< T >::Quaternion( const Matrix< U > & mat )
{
   const Quaternion< U > quat = Quaternion< U >::ToQuaternion(mat);

   mT[0] = quat.mT[0];
   mT[1] = quat.mT[1];
   mT[2] = quat.mT[2];
   mT[3] = quat.mT[3];
}

template < typename T >
Quaternion< T >::Quaternion( const Matrix< T > & mat )
{
   const Quaternion< T > quat = Quaternion< T >::ToQuaternion(mat);

   mT[0] = quat.mT[0];
   mT[1] = quat.mT[1];
   mT[2] = quat.mT[2];
   mT[3] = quat.mT[3];
}

template < typename T >
Quaternion< T >::~Quaternion( )
{
}

template < typename T >
template < typename U >
Quaternion< T > & Quaternion< T >::operator = ( const Quaternion< U > & quat )
{
   mT[0] = quat.mT[0];
   mT[1] = quat.mT[1];
   mT[2] = quat.mT[2];
   mT[3] = quat.mT[3];

   return *this;
}

template < typename T >
Quaternion< T > & Quaternion< T >::operator = ( const Quaternion< T > & quat )
{
   mT[0] = quat.mT[0];
   mT[1] = quat.mT[1];
   mT[2] = quat.mT[2];
   mT[3] = quat.mT[3];

   return *this;
}

template < typename T >
template < typename U >
bool Quaternion< T >::operator == ( const Quaternion< U > & quat ) const
{
   return mT[0] == quat.mT[0] &&
          mT[1] == quat.mT[1] &&
          mT[2] == quat.mT[2] &&
          mT[3] == quat.mT[3];
}

template < typename T >
bool Quaternion< T >::operator == ( const Quaternion< T > & quat ) const
{
   return mT[0] == quat.mT[0] &&
          mT[1] == quat.mT[1] &&
          mT[2] == quat.mT[2] &&
          mT[3] == quat.mT[3];
}

template < typename T >
template < typename U >
bool Quaternion< T >::operator != ( const Quaternion< U > & quat ) const
{
   return mT[0] != quat.mT[0] ||
          mT[1] != quat.mT[1] ||
          mT[2] != quat.mT[2] ||
          mT[3] != quat.mT[3];
}

template < typename T >
bool Quaternion< T >::operator != ( const Quaternion< T > & quat ) const
{
   return mT[0] != quat.mT[0] ||
          mT[1] != quat.mT[1] ||
          mT[2] != quat.mT[2] ||
          mT[3] != quat.mT[3];
}

template < typename T >
Matrix< T > Quaternion< T >::ToMatrix( ) const
{
   const Vector< T, 4 > x(T(1) - T(2) * mT[1] * mT[1] - T(2) * mT[2] * mT[2],
                          T(2) * mT[0] * mT[1] + T(2) * mT[3] * mT[2],
                          T(2) * mT[0] * mT[2] - T(2) * mT[3] * mT[1],
                          T(0));
   const Vector< T, 4 > y(T(2) * mT[0] * mT[1] - T(2) * mT[3] * mT[2],
                          T(1) - T(2) * mT[0] * mT[0] - T(2) * mT[2] * mT[2],
                          T(2) * mT[1] * mT[2] + T(2) * mT[3] * mT[0],
                          T(0));
   const Vector< T, 4 > z(T(2) * mT[0] * mT[2] + T(2) * mT[3] * mT[1],
                          T(2) * mT[1] * mT[2] - T(2) * mT[3] * mT[0],
                          T(1) - T(2) * mT[0] * mT[0] - T(2) * mT[1] * mT[1],
                          T(0));
   const Vector< T, 4 > w(T(0), T(0), T(0), T(1));

   return Matrix< T >(x, y, z, w);
}

template < typename T >
template < typename U >
Quaternion< T > Quaternion< T >::operator * ( const Quaternion< U > & quat ) const
{
   const Vector< T, 3 > i1 = ImaginaryPart();
   const Vector< T, 3 > i2 = quat.ImaginaryPart();

   const Vector< T, 3 > i = i2 * mT[3] + i1 * quat.mT[3] + i1 ^ i2;

   const T x = i.mT[0];
   const T y = i.mT[1];
   const T z = i.mT[2];
   const T w = (mT[3] * quat.mT[3]) - (i1 * i2);

   return Quaternion< T >(x, y, z, w);
}

template < typename T >
Quaternion< T > Quaternion< T >::operator * ( const Quaternion< T > & quat ) const
{
   const Vector< T, 3 > i1 = ImaginaryPart();
   const Vector< T, 3 > i2 = quat.ImaginaryPart();

   const Vector< T, 3 > i = i2 * mT[3] + i1 * quat.mT[3] + i1 ^ i2;

   const T x = i.mT[0];
   const T y = i.mT[1];
   const T z = i.mT[2];
   const T w = (mT[3] * quat.mT[3]) - (i1 * i2);

   return Quaternion< T >(x, y, z, w);
}

template < typename T >
template < typename U >
Quaternion< T > & Quaternion< T >::operator *= ( const Quaternion< U > & quat )
{
   *this = *this * quat;

   return *this;
}

template < typename T >
Quaternion< T > & Quaternion< T >::operator *= ( const Quaternion< T > & quat )
{
   *this = *this * quat;

   return *this;
}

template < typename T >
template < typename U >
Quaternion< T > Quaternion< T >::operator % ( const Quaternion< U > & quat ) const
{
   const T x = mT[3] * quat.mT[0] + mT[0] * quat.mT[3] + mT[1] * quat.mT[2] - mT[2] * quat.mT[1];
   const T y = mT[3] * quat.mT[1] - mT[0] * quat.mT[2] + mT[1] * quat.mT[3] + mT[2] * quat.mT[0];
   const T z = mT[3] * quat.mT[2] + mT[1] * quat.mT[2] - mT[2] * quat.mT[1] + mT[2] * quat.mT[3];
   const T w = mT[3] * quat.mT[3] - mT[0] * quat.mT[0] - mT[1] * quat.mT[1] - mT[2] * quat.mT[2];

   return Quaternion< T >(x, y, z, w);
}

template < typename T >
Quaternion< T > Quaternion< T >::operator % ( const Quaternion< T > & quat ) const
{
   const T x = mT[3] * quat.mT[0] + mT[0] * quat.mT[3] + mT[1] * quat.mT[2] - mT[2] * quat.mT[1];
   const T y = mT[3] * quat.mT[1] - mT[0] * quat.mT[2] + mT[1] * quat.mT[3] + mT[2] * quat.mT[0];
   const T z = mT[3] * quat.mT[2] + mT[0] * quat.mT[1] - mT[1] * quat.mT[0] + mT[2] * quat.mT[3];
   const T w = mT[3] * quat.mT[3] - mT[0] * quat.mT[0] - mT[1] * quat.mT[1] - mT[2] * quat.mT[2];

   return Quaternion< T >(x, y, z, w);
}

template < typename T >
template < typename U >
Quaternion< T > & Quaternion< T >::operator %= ( const Quaternion< U > & quat )
{
   *this = *this % quat;

   return *this;
}

template < typename T >
Quaternion< T > & Quaternion< T >::operator %= ( const Quaternion< T > & quat )
{
   *this = *this % quat;

   return *this;
}

template < typename T >
template < typename U >
Quaternion< T > Quaternion< T >::operator + ( const Quaternion< U > & quat ) const
{
   const T x = mT[0] + quat.mT[0];
   const T y = mT[1] + quat.mT[1];
   const T z = mT[2] + quat.mT[2];
   const T w = mT[3] + quat.mT[3];

   return Quaternion< T >(x, y, z, w);
}

template < typename T >
Quaternion< T > Quaternion< T >::operator + ( const Quaternion< T > & quat ) const
{
   const T x = mT[0] + quat.mT[0];
   const T y = mT[1] + quat.mT[1];
   const T z = mT[2] + quat.mT[2];
   const T w = mT[3] + quat.mT[3];

   return Quaternion< T >(x, y, z, w);
}

template < typename T >
template < typename U >
Quaternion< T > & Quaternion< T >::operator + ( const Quaternion< U > & quat )
{
   *this = *this + quat;

   return *this;
}

template < typename T >
Quaternion< T > & Quaternion< T >::operator + ( const Quaternion< T > & quat )
{
   *this = *this + quat;

   return *this;
}

template < typename T >
template < typename U >
Quaternion< T > Quaternion< T >::operator % ( const Vector< U, 3 > & vec ) const
{
   T x =   mT[3] * vec.mT[0] + mT[1] * vec.mT[2] - mT[2] * vec.mT[1];
   T y =   mT[3] * vec.mT[1] - mT[0] * vec.mT[2] + mT[2] * vec.mT[0];
   T z =   mT[3] * vec.mT[2] + mT[0] * vec.mT[1] - mT[1] * vec.mT[0];
   T w = - mT[0] * vec.mT[0] - mT[1] * vec.mT[1] - mT[2] * vec.mT[2];

   return Quaternion< T >(x, y, z, w);
}

template < typename T >
Quaternion< T > Quaternion< T >::operator % ( const Vector< T, 3 > & vec ) const
{
   T x =   mT[3] * vec.mT[0] + mT[1] * vec.mT[2] - mT[2] * vec.mT[1];
   T y =   mT[3] * vec.mT[1] - mT[0] * vec.mT[2] + mT[2] * vec.mT[0];
   T z =   mT[3] * vec.mT[2] + mT[0] * vec.mT[1] - mT[1] * vec.mT[0];
   T w = - mT[0] * vec.mT[0] - mT[1] * vec.mT[1] - mT[2] * vec.mT[2];

   return Quaternion< T >(x, y, z, w);
}

template < typename T >
template < typename U >
Quaternion< T > & Quaternion< T >::operator %= ( const Vector< U, 3 > & vec )
{
   *this = *this % vec;

   return *this;
}

template < typename T >
Quaternion< T > & Quaternion< T >::operator %= ( const Vector< T, 3 > & vec )
{
   *this = *this % vec;

   return *this;
}

template < typename T >
template < typename U >
Quaternion< T > Quaternion< T >::Rotate( const U & degrees, const Vector< U, 3 > & vec )
{
   return *this % Quaternion< T >::Rotation(degrees, vec);
}

template < typename T >
Quaternion< T > Quaternion< T >::Rotate( const T & degrees, const Vector< T, 3 > & vec )
{
   return *this % Quaternion< T >::Rotation(degrees, vec);
}

template < typename T >
template < typename U >
Quaternion< T > Quaternion< T >::Rotate( const Quaternion< U > & quat )
{
   return *this % quat;
}

template < typename T >
Quaternion< T > Quaternion< T >::Rotate( const Quaternion< T > & quat )
{
   return *this % quat;
}

template < typename T >
Quaternion< T > & Quaternion< T >::Normalize( )
{
   const T one_over_length = 1 / Length();

   mT[0] *= one_over_length;
   mT[1] *= one_over_length;
   mT[2] *= one_over_length;
   mT[3] *= one_over_length;

   return *this;
}

template < typename T >
Quaternion< T > Quaternion< T >::UnitQuaternion( ) const
{
   Quaternion< T > quat(*this);
   quat.Normalize();

   return quat;
}

template < typename T >
Quaternion< T > & Quaternion< T >::Conjugate( )
{
   mT[0] *= -1;
   mT[1] *= -1;
   mT[2] *= -1;

   return *this;
}

template < typename T >
Quaternion< T > Quaternion< T >::ConjugateQuaternion( ) const
{
   Quaternion< T > quat(*this);
   quat.Conjugate();

   return quat;
}

template < typename T >
Quaternion< T > & Quaternion< T >::Inverse( )
{
   const T one_over_length_squared = 1 / std::pow(Length(), 2);

   mT[0] *= one_over_length_squared;
   mT[1] *= one_over_length_squared;
   mT[2] *= one_over_length_squared;
   mT[3] *= one_over_length_squared;

   return *this;
}

template < typename T >
Quaternion< T > Quaternion< T >::InverseQuaternion( ) const
{
   Quaternion< T > quat(*this);
   quat.Inverse();

   return quat;
}

template < typename T >
Quaternion< T > Quaternion< T >::ToQuaternion( const Matrix< T > & mat )
{
   const T four_x_squared_minus_1 = mat.mT[0]  - mat.mT[5] - mat.mT[10];
   const T four_y_squared_minus_1 = mat.mT[5]  - mat.mT[0] - mat.mT[10];
   const T four_z_squared_minus_1 = mat.mT[10] - mat.mT[0] - mat.mT[5];
   const T four_w_squared_minus_1 = mat.mT[0]  + mat.mT[5] + mat.mT[10];

   const uint8_t W_IS_LARGEST = 0;
   const uint8_t X_IS_LARGEST = 1;
   const uint8_t Y_IS_LARGEST = 2;
   const uint8_t Z_IS_LARGEST = 3;

   const std::pair< const uint8_t, const T > largest_index_and_value =
   [ & ] ( ) -> std::pair< uint8_t, T >
   {
      std::pair< uint8_t, T > idx_and_val(W_IS_LARGEST, four_w_squared_minus_1);

      if (four_x_squared_minus_1 > idx_and_val.second)
      {
         idx_and_val.first = X_IS_LARGEST;
         idx_and_val.second = four_x_squared_minus_1;
      }

      if (four_y_squared_minus_1 > idx_and_val.second)
      {
         idx_and_val.first = Y_IS_LARGEST;
         idx_and_val.second = four_y_squared_minus_1;
      }

      if (four_z_squared_minus_1 > idx_and_val.second)
      {
         idx_and_val.first = Z_IS_LARGEST;
         idx_and_val.second = four_z_squared_minus_1;
      }

      return idx_and_val;
   }();

   const T largest_value = std::sqrt(largest_index_and_value.second + static_cast< T >(1)) * static_cast< T >(0.5);
   const T mult = static_cast< T >(0.25) / largest_value;

   Quaternion< T > quat;

   switch (largest_index_and_value.first)
   {
   case W_IS_LARGEST:
      quat.mT[3] = largest_index_and_value.second;
      quat.mT[0] = (mat.mT[6] - mat.mT[9]) * mult;
      quat.mT[1] = (mat.mT[8] - mat.mT[2]) * mult;
      quat.mT[2] = (mat.mT[1] - mat.mT[4]) * mult;

      break;

   case X_IS_LARGEST:
      quat.mT[3] = (mat.mT[6] - mat.mT[9]) * mult;
      quat.mT[0] = largest_index_and_value.second;
      quat.mT[1] = (mat.mT[1] + mat.mT[4]) * mult;
      quat.mT[2] = (mat.mT[8] + mat.mT[2]) * mult;

      break;

   case Y_IS_LARGEST:
      quat.mT[3] = (mat.mT[8] - mat.mT[2]) * mult;
      quat.mT[0] = (mat.mT[1] + mat.mT[4]) * mult;
      quat.mT[1] = largest_index_and_value.second;
      quat.mT[2] = (mat.mT[6] + mat.mT[9]) * mult;

      break;

   case Z_IS_LARGEST:
      quat.mT[3] = (mat.mT[1] - mat.mT[4]) * mult;
      quat.mT[0] = (mat.mT[8] + mat.mT[2]) * mult;
      quat.mT[1] = (mat.mT[6] + mat.mT[9]) * mult;
      quat.mT[2] = largest_index_and_value.second;

      break;

   default:
      // should never get here
      WGL_ASSERT(false);

      break;
   }

   return quat;
}

template < typename T >
Quaternion< T > Quaternion< T >::Rotation( const T & degrees, const Vector< T, 3 > & axis )
{
   WGL_ASSERT(1 - std::numeric_limits< T >::epsilon() <= axis.Length() &&
              axis.Length() <= 1 + std::numeric_limits< T >::epsilon());

   const T radians = math::DegToRad(degrees);
   const T sin = std::sin(radians * static_cast< T >(0.5));

   return Quaternion< T >(axis.mT[0] * sin,
                          axis.mT[1] * sin,
                          axis.mT[2] * sin,
                          std::cos(radians * static_cast< T >(0.5)));
}

template < typename T >
Quaternion< T > Quaternion< T >::Rotation( const T & degrees, const Vector< T, 4 > & axis )
{
   WGL_ASSERT(axis.W() == 0);
   WGL_ASSERT(1 - std::numeric_limits< T >::epsilon() <= axis.Length() &&
              axis.Length() <= 1 + std::numeric_limits< T >::epsilon());

   return Quaternion< T >::Rotation(degrees, Vector< T, 3 >(axis));
}

template < typename T >
Quaternion< T > Quaternion< T >::Rotation( const Vector< T, 3 > & origin, const Vector< T, 3 > & destination )
{
   WGL_ASSERT(1 - std::numeric_limits< T >::epsilon() <= origin.Length() &&
              origin.Length() <= 1 + std::numeric_limits< T >::epsilon());
   WGL_ASSERT(1 - std::numeric_limits< T >::epsilon() <= destination.Length() &&
              destination.Length() <= 1 + std::numeric_limits< T >::epsilon());

   const T cos_theta = origin * destination;

   Vector< T, 3 > rotation_axis;

   if (cos_theta < static_cast< T >(-1) + std::numeric_limits< T >::epsilon())
   {
      // special case when vectors in opposite directions :
      // there is no "ideal" rotation axis
      // So guess one; any will do as long as it's perpendicular to start
      // This implementation favors a rotation around the Up axis (Y),
      // since it's often what you want to do.
      rotation_axis = Vector< T, 3 >(T(0), T(0), T(1)) ^ origin;

      if (std::pow(rotation_axis.Length(), 2) < std::numeric_limits< T >::epsilon())
      {
         // try again as they were parallel
         rotation_axis = Vector< T, 3 >(T(1), T(0), T(0)) ^ origin;
      }

      return Quaternion< T >::Rotation(math::pi< T >(), rotation_axis.UnitVector());
   }

   rotation_axis = origin ^ destination;

   const T s = std::sqrt((static_cast< T >(1) + cos_theta) * static_cast< T >(2));
   const T invs = static_cast< T >(1) / s;

   return Quaternion< T >(rotation_axis.mT[0] * invs,
                          rotation_axis.mT[1] * invs,
                          rotation_axis.mT[2] * invs,
                          s * static_cast< T >(0.5));
}

template < typename T >
Quaternion< T > Quaternion< T >::Rotation( const Vector< T, 4 > & origin, const Vector< T, 4 > & destination )
{
   WGL_ASSERT(origin.W() == 0 && destination.W() == 0);
   WGL_ASSERT(1 - std::numeric_limits< T >::epsilon() <= origin.Length() &&
              origin.Length() <= 1 + std::numeric_limits< T >::epsilon());
   WGL_ASSERT(1 - std::numeric_limits< T >::epsilon() <= destination.Length() &&
              destination.Length() <= 1 + std::numeric_limits< T >::epsilon());

   return Quaternion< T >::Rotation(Vector3< T >(origin), Vector3< T >(destination));
}

typedef Quaternion< float >  Quatf;
typedef Quaternion< double > Quatd;

#endif // _QUATERNION_H_
