#ifndef _VECTOR_H_
#define _VECTOR_H_

// crt includes
#include <math.h>
#include <memory.h>

template < typename T >
class Vector
{
public:
   // constructor / destructor
            Vector( );
            Vector( const T t[4] );
            Vector( const T & x, const T & y, const T & z, const T & w = 1 );
            Vector( const Vector< T > & vec );
            template< typename U >
            Vector( const Vector< U > & vec );
           ~Vector( );

   // operator =
   template < typename U >
   Vector< T > & operator = ( const Vector< U > & vec );
   Vector< T > & operator = ( const Vector< T > & vec );
   template < typename U >
   Vector< T > & operator = ( const U u[4] );
   Vector< T > & operator = ( const T t[4] );

   // scalar operator *
   template < typename U >
   Vector< T > operator * ( const U & u ) const;
   Vector< T > operator * ( const T & t ) const;
   template < typename U >
   Vector< T > & operator *= ( const U & u );
   Vector< T > & operator *= ( const T & t );

   // operator *
   template < typename U >
   T operator * ( const Vector< U > & vec ) const;
   T operator * ( const Vector< T > & vec ) const;
   template < typename U >
   T operator * ( const U u[4] ) const;
   T operator * ( const T t[4] ) const;

   // operator ^ (cross product)
   template < typename U >
   Vector< T > operator ^ ( const Vector< U > & vec ) const;
   Vector< T > operator ^ ( const Vector< T > & vec ) const;
   template < typename U >
   Vector< T > & operator ^= ( const Vector< U > & vec );
   Vector< T > & operator ^= ( const Vector< T > & vec );

   // operator -
   template < typename U >
   Vector< T > operator - ( const Vector< U > & vec ) const;
   Vector< T > operator - ( const Vector< T > & vec ) const;
   template < typename U >
   Vector< T > & operator -= ( const Vector< U > & vec );
   Vector< T > & operator -= ( const Vector< T > & vec );

   // operator +
   template < typename U >
   Vector< T > operator + ( const Vector< U > & vec ) const;
   Vector< T > operator + ( const Vector< T > & vec ) const;
   template < typename U >
   Vector< T > & operator += ( const Vector< U > & vec );
   Vector< T > & operator += ( const Vector< T > & vec );

   // operator ==
   template < typename U >
   bool operator == ( const Vector< U > & vec ) const;
   bool operator == ( const Vector< T > & vec ) const;

   // operator !=
   template < typename U >
   bool operator != ( const Vector< U > & vec ) const;
   bool operator != ( const Vector< T > & vec ) const;

   // basic accessor into the array
   const T & X( ) const;
   const T & Y( ) const;
   const T & Z( ) const;
   const T & W( ) const;

   // operator T
   operator T * ( );
   operator const T * ( ) const;

   // makes a zero vector
   Vector< T > &  MakeZeroVector( );

   // makes the vector a unit vector
   Vector< T >    UnitVector( ) const;
   Vector< T > &  MakeUnitVector( );

   // returns the length
   T  Length( ) const;

   // vector class should be simple and allow
   // easy access to the member variables
   T     mT[4];

};

template < typename T >
inline Vector< T >::Vector( )
{
   MakeZeroVector();
}

template < typename T >
inline Vector< T >::Vector( const T t[4] )
{
   memcpy(mT, t, sizeof(mT));
}

template < typename T >
inline Vector< T >::Vector( const T & x,
                            const T & y,
                            const T & z,
                            const T & w )
{
   mT[0] = x;
   mT[1] = y;
   mT[2] = z;
   mT[3] = w;
}

template < typename T >
inline Vector< T >::Vector( const Vector< T > & vec )
{
   *this = vec;
}

template < typename T >
template < typename U >
inline Vector< T >::Vector( const Vector< U > & vec )
{
   *this = vec;
}

template < typename T >
inline Vector< T >::~Vector( )
{
}

template < typename T >
template < typename U >
inline Vector< T > & Vector< T >::operator = ( const Vector< U > & vec )
{
   mT[0] = vec.mT[0];
   mT[1] = vec.mT[1];
   mT[2] = vec.mT[2];
   mT[3] = vec.mT[3];

   return *this;
}

template< typename T >
inline Vector< T > & Vector< T >::operator = ( const Vector< T > & vec )
{
   if (this != &vec) memcpy(mT, vec.mT, sizeof(mT));

   return *this;
}

template < typename T >
template < typename U >
inline Vector< T > & Vector< T >::operator = ( const U u[3] )
{
   Vector< U > vec(u);

   *this = vec;

   return *this;
}

template< typename T >
inline Vector< T > & Vector< T >::operator = ( const T t[3] )
{
   memcpy(mT, t, sizeof(mT));

   return *this;
}

template < typename T >
template < typename U >
inline T Vector< T >::operator * ( const Vector< U > & vec ) const
{
   return mT[0] * vec.mT[0] +
          mT[1] * vec.mT[1] +
          mT[2] * vec.mT[2];
}

template < typename T >
inline T Vector< T >::operator * ( const Vector< T > & vec ) const
{
   return mT[0] * vec.mT[0] +
          mT[1] * vec.mT[1] +
          mT[2] * vec.mT[2];
}

template < typename T >
template < typename U >
inline T Vector< T >::operator * ( const U u[3] ) const
{
   return mT[0] * u[0] +
          mT[1] * u[1] +
          mT[2] * u[2];
}

template < typename T >
inline T Vector< T >::operator * ( const T t[3] ) const
{
   return mT[0] * t[0] +
          mT[1] * t[1] +
          mT[2] * t[2];
}

template < typename T >
template < typename U >
inline Vector< T > Vector< T >::operator * ( const U & u ) const
{
   Vector< T > v(*this);

   v *= u;

   return v;
}

template < typename T >
inline Vector< T > Vector< T >::operator * ( const T & t ) const
{
   Vector< T > v(*this);

   v *= t;

   return v;
}

template < typename T >
template < typename U >
inline Vector< T > & Vector< T >::operator *= ( const U & u )
{
   mT[0] *= u;
   mT[1] *= u;
   mT[2] *= u;

   return *this;
}

template < typename T >
inline Vector< T > & Vector< T >::operator *= ( const T & t )
{
   mT[0] *= t;
   mT[1] *= t;
   mT[2] *= t;

   return *this;
}

template < typename T >
template < typename U >
inline Vector< T > Vector< T >::operator ^ ( const Vector< U > & vec ) const
{
   Vector< T > v(vec);

   return (*this ^ v);
}

template < typename T >
inline Vector< T > Vector< T >::operator ^ ( const Vector< T > & vec ) const
{
   Vector< T > v(*this);

   return (v ^= vec);
}

template < typename T >
template < typename U >
inline Vector< T > & Vector< T >::operator ^= ( const Vector< U > & vec )
{
   Vector< T > v(vec);

   return (*this ^= v);
}

template < typename T >
inline Vector< T > & Vector< T >::operator ^= ( const Vector< T > & vec )
{
   const T i = (mT[1] * vec.mT[2]) - (mT[2] * vec.mT[1]);
   const T j = (mT[2] * vec.mT[0]) - (mT[0] * vec.mT[2]);
   const T k = (mT[0] * vec.mT[1]) - (mT[1] * vec.mT[0]);

   mT[0] = i;
   mT[1] = j;
   mT[2] = k;

   return (*this);
}

template < typename T >
template < typename U >
inline Vector< T > Vector< T >::operator - ( const Vector< U > & vec ) const
{
   Vector< T > v(vec);

   return (*this - v);
}

template < typename T >
inline Vector< T > Vector< T >::operator - ( const Vector< T > & vec ) const
{
   Vector< T > v(*this);

   return v -= vec;
}

template < typename T >
template < typename U >
inline Vector< T > & Vector< T >::operator -= ( const Vector< U > & vec )
{
   Vector< T > vecT(vec);

   return (*this -= vecT);
}

template < typename T >
inline Vector< T > & Vector< T >::operator -= ( const Vector< T > & vec )
{
   mT[0] -= vec.mT[0];
   mT[1] -= vec.mT[1];
   mT[2] -= vec.mT[2];

   return *this;
}

template < typename T >
template < typename U >
inline Vector< T > Vector< T >::operator + ( const Vector< U > & vec ) const
{
   Vector< T > v(vec);

   return (*this + v);
}

template < typename T >
inline Vector< T > Vector< T >::operator + ( const Vector< T > & vec ) const
{
    Vector< T > v(*this);

   return v += vec;
}

template < typename T >
template < typename U >
inline Vector< T > & Vector< T >::operator += ( const Vector< U > & vec )
{
   Vector< T > vecT(vec);

   return (*this += vecT);
}

template < typename T >
inline Vector< T > & Vector< T >::operator += ( const Vector< T > & vec )
{
   mT[0] += vec.mT[0];
   mT[1] += vec.mT[1];
   mT[2] += vec.mT[2];

   return *this;
}

template < typename T >
template < typename U >
inline bool Vector< T >::operator == ( const Vector< U > & vec ) const
{
   return *this == Vector< T >(vec);
}

template < typename T >
inline bool Vector< T >::operator == ( const Vector< T > & vec ) const
{
   return mT[0] == vec.mT[0] &&
          mT[1] == vec.mT[1] &&
          mT[2] == vec.mT[2] &&
          mT[3] == vec.mT[3];
}

template < typename T >
template < typename U >
inline bool Vector< T >::operator != ( const Vector< U > & vec ) const
{
   return !(*this == Vector< T >(vec));
}

template < typename T >
inline bool Vector< T >::operator != ( const Vector< T > & vec ) const
{
   return !(*this == vec);
}

template < typename T >
const T & Vector< T >::X( ) const
{
   return mT[0];
}

template < typename T >
const T & Vector< T >::Y( ) const
{
   return mT[1];
}

template < typename T >
const T & Vector< T >::Z( ) const
{
   return mT[2];
}

template < typename T >
const T & Vector< T >::W( ) const
{
   return mT[3];
}

template < typename T >
inline Vector< T >::operator T * ( )
{
   return mT;
}

template < typename T >
inline Vector< T >::operator const T * ( ) const
{
   return mT;
}

template < typename T >
inline Vector< T > & Vector< T >::MakeZeroVector( )
{
   memset(mT, 0x00, sizeof(mT));

   mT[3] = 1;

   return (*this);
}

template < typename T >
inline Vector< T > Vector< T >::UnitVector( ) const
{
   Vector< T > v(*this);

   v.MakeUnitVector();

   return v;
}

template < typename T >
inline Vector< T > & Vector< T >::MakeUnitVector( )
{
   T t = 1 / Length();

   mT[0] *= t;
   mT[1] *= t;
   mT[2] *= t;
   mT[3] = 1;

   return (*this);
}

template < typename T >
inline T Vector< T >::Length( ) const
{
   T x = mT[0];
   T y = mT[1];
   T z = mT[2];

   return sqrt(x * x + y * y + z * z);
}

typedef Vector< float > Vectorf;
typedef Vector< double > Vectord;

#endif // _VECTOR_H_