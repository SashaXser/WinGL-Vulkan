#ifndef _VECTOR4_H_
#define _VECTOR4_H_

// local includes
#include "Vector3.h"
#include "WglAssert.h"

// std includes
#include <math.h>

template < typename T >
class Vector4
{
public:
   // public typedefs
   typedef T type;

   // public enums
   enum { NUM_COMPONENTS = 4 };

    // constructor / destructor
    Vector4( );
    template < typename U >
    Vector4( const U & x, const U & y, const U & z, const U & w = 1 );
    Vector4( const T & x, const T & y, const T & z, const T & w = 1 );
    template < typename U >
    Vector4( const Vector4< U > & vec );
    Vector4( const Vector4< T > & vec );
    template < typename U >
    Vector4( const U u[4] );
    Vector4( const T t[4] );
    template < typename U >
    Vector4( const Vector3< U > & vec );
    Vector4( const Vector3< T > & vec );
   ~Vector4( );

   // operator =
   template < typename U >
   Vector4< T > & operator = ( const Vector4< U > & vec );
   Vector4< T > & operator = ( const Vector4< T > & vec );
   template < typename U >
   Vector4< T > & operator = ( const Vector3< U > & vec );
   Vector4< T > & operator = ( const Vector3< T > & vec );
   template < typename U >
   Vector4< T > & operator = ( const U u[4] );
   Vector4< T > & operator = ( const T t[4] );

   // scalar operator *
   template < typename U >
   Vector4< T > operator * ( const U & u ) const;
   Vector4< T > operator * ( const T & t ) const;
   template < typename U >
   Vector4< T > & operator *= ( const U & u );
   Vector4< T > & operator *= ( const T & t );

   // operator *
   template < typename U >
   T operator * ( const Vector4< U > & vec ) const;
   T operator * ( const Vector4< T > & vec ) const;
   template < typename U >
   T operator * ( const U u[4] ) const;
   T operator * ( const T t[4] ) const;

   // operator ^ (cross product)
   template < typename U >
   Vector4< T > operator ^ ( const Vector4< U > & vec ) const;
   Vector4< T > operator ^ ( const Vector4< T > & vec ) const;
   template < typename U >
   Vector4< T > & operator ^= ( const Vector4< U > & vec );
   Vector4< T > & operator ^= ( const Vector4< T > & vec );

   // operator -
   template < typename U >
   Vector4< T > operator - ( const Vector4< U > & vec ) const;
   Vector4< T > operator - ( const Vector4< T > & vec ) const;
   template < typename U >
   Vector4< T > & operator -= ( const Vector4< U > & vec );
   Vector4< T > & operator -= ( const Vector4< T > & vec );

   // operator +
   template < typename U >
   Vector4< T > operator + ( const Vector4< U > & vec ) const;
   Vector4< T > operator + ( const Vector4< T > & vec ) const;
   template < typename U >
   Vector4< T > & operator += ( const Vector4< U > & vec );
   Vector4< T > & operator += ( const Vector4< T > & vec );

   // operator ==
   template < typename U >
   bool operator == ( const Vector4< U > & vec ) const;
   bool operator == ( const Vector4< T > & vec ) const;

   // operator !=
   template < typename U >
   bool operator != ( const Vector4< U > & vec ) const;
   bool operator != ( const Vector4< T > & vec ) const;

   // operator []
   T & operator [] ( const int i );
   const T & operator [] ( const int i ) const; 

   // basic accessors into the array
   T & X( );
   T & Y( );
   T & Z( );
   T & W( );

   const T & X( ) const;
   const T & Y( ) const;
   const T & Z( ) const;
   const T & W( ) const;

   T * Ptr( );
   const T * Ptr( ) const;

   // sets the components
   template < typename U >
   void Set( const Vector4< U > & vec );
   void Set( const Vector4< T > & vec );
   template < typename U >
   void Set( const U & x, const U & y, const U & z, const U & w = 1 );
   void Set( const T & x, const T & y, const T & z, const T & w = 1 );
   template < typename U >
   void Set( const U * const pXYZW );
   void Set( const T * const pXYZW );

   // operator T
   operator T * ( );
   operator const T * ( ) const;

   // makes a zero vector
   Vector4< T > &  MakeZeroVector( );

   // makes the vector a unit vector
   Vector4< T >    UnitVector( ) const;
   Vector4< T > &  MakeUnitVector( );

   // returns the length
   T  Length( ) const;

   // vector class should be simple and allow
   // easy access to the member variables
   T     mT[4];

};

template < typename T >
inline Vector4< T >::Vector4( )
{
   MakeZeroVector();
}

template < typename T >
template < typename U >
inline Vector4< T >::Vector4( const U & x,
                              const U & y,
                              const U & z,
                              const U & w )
{
   Set(x, y, z, w);
}

template < typename T >
inline Vector4< T >::Vector4( const T & x,
                              const T & y,
                              const T & z,
                              const T & w )
{
   Set(x, y, z, w);
}

template < typename T >
template < typename U >
inline Vector4< T >::Vector4( const Vector4< U > & vec )
{
   *this = vec;
}

template < typename T >
inline Vector4< T >::Vector4( const Vector4< T > & vec )
{
   *this = vec;
}

template < typename T >
template < typename U >
inline Vector4< T >::Vector4( const U u[4] )
{
   Set(u[0], u[1], u[2], u[3]);
}

template < typename T >
inline Vector4< T >::Vector4( const T t[4] )
{
   Set(t[0], t[1], t[2], t[3]);
}

template < typename T >
template < typename U >
inline Vector4< T >::Vector4( const Vector3< U > & vec )
{
   *this = Vector4< U >(vec.mT[0], vec.mT[1], vec.mT[2]);
}

template < typename T >
inline Vector4< T >::Vector4( const Vector3< T > & vec )
{
   *this = vec;
}

template < typename T >
inline Vector4< T >::~Vector4( )
{
}

template < typename T >
template < typename U >
inline Vector4< T > & Vector4< T >::operator = ( const Vector4< U > & vec )
{
   Set(vec.mT);

   return *this;
}

template < typename T >
inline Vector4< T > & Vector4< T >::operator = ( const Vector4< T > & vec )
{
   if (this != &vec)
   {
      Set(vec.mT);
   }

   return *this;
}

template < typename T >
template < typename U >
inline Vector4< T > & Vector4< T >::operator = ( const Vector3< U > & vec )
{
   Set(vec.mT[0], vec.mT[1], vec.mT[2], 1);

   return *this;
}

template < typename T >
inline Vector4< T > & Vector4< T >::operator = ( const Vector3< T > & vec )
{
   Set(vec.mT[0], vec.mT[1], vec.mT[2], 1);

   return *this;
}

template < typename T >
template < typename U >
inline Vector4< T > & Vector4< T >::operator = ( const U u[4] )
{
   Set(u);

   return *this;
}

template< typename T >
inline Vector4< T > & Vector4< T >::operator = ( const T t[4] )
{
   Set(t);

   return *this;
}

template < typename T >
template < typename U >
inline T Vector4< T >::operator * ( const Vector4< U > & vec ) const
{
   return *this * Vector4< T >(vec);
}

template < typename T >
inline T Vector4< T >::operator * ( const Vector4< T > & vec ) const
{
   return mT[0] * vec.mT[0] +
          mT[1] * vec.mT[1] +
          mT[2] * vec.mT[2] +
          mT[3] * vec.mT[3];
}

template < typename T >
template < typename U >
inline T Vector4< T >::operator * ( const U u[4] ) const
{
   return *this * Vector< T >(u);
}

template < typename T >
inline T Vector4< T >::operator * ( const T t[4] ) const
{
   return *this * Vector< T >(t);
}

template < typename T >
template < typename U >
inline Vector4< T > Vector4< T >::operator * ( const U & u ) const
{
   Vector4< T > v(*this);

   v *= u;

   return v;
}

template < typename T >
inline Vector4< T > Vector4< T >::operator * ( const T & t ) const
{
   Vector4< T > v(*this);

   v *= t;

   return v;
}

template < typename T >
template < typename U >
inline Vector4< T > & Vector4< T >::operator *= ( const U & u )
{
   *this *= static_cast< T >(u);

   return *this;
}

template < typename T >
inline Vector4< T > & Vector4< T >::operator *= ( const T & t )
{
   mT[0] *= t;
   mT[1] *= t;
   mT[2] *= t;
   mT[3] *= t;

   return *this;
}

template < typename T >
template < typename U >
inline Vector4< T > Vector4< T >::operator ^ ( const Vector4< U > & vec ) const
{
   Vector4< T > v(vec);

   return (*this ^ v);
}

template < typename T >
inline Vector4< T > Vector4< T >::operator ^ ( const Vector4< T > & vec ) const
{
   Vector4< T > v(*this);

   return (v ^= vec);
}

template < typename T >
template < typename U >
inline Vector4< T > & Vector4< T >::operator ^= ( const Vector4< U > & vec )
{
   Vector4< T > v(vec);

   return (*this ^= v);
}

template < typename T >
inline Vector4< T > & Vector4< T >::operator ^= ( const Vector4< T > & vec )
{
   WGL_ASSERT(mT[3] == 1 && vec.mT[3] == 1 && "Not Constrained To Homogeneous R3 Space");

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
inline Vector4< T > Vector4< T >::operator - ( const Vector4< U > & vec ) const
{
   Vector4< T > v(vec);

   return (*this - v);
}

template < typename T >
inline Vector4< T > Vector4< T >::operator - ( const Vector4< T > & vec ) const
{
   Vector4< T > v(*this);

   return v -= vec;
}

template < typename T >
template < typename U >
inline Vector4< T > & Vector4< T >::operator -= ( const Vector4< U > & vec )
{
   Vector4< T > vecT(vec);

   return (*this -= vecT);
}

template < typename T >
inline Vector4< T > & Vector4< T >::operator -= ( const Vector4< T > & vec )
{
   mT[0] -= vec.mT[0];
   mT[1] -= vec.mT[1];
   mT[2] -= vec.mT[2];
   mT[3] -= vec.mT[3];

   return *this;
}

template < typename T >
template < typename U >
inline Vector4< T > Vector4< T >::operator + ( const Vector4< U > & vec ) const
{
   Vector4< T > v(vec);

   return (*this + v);
}

template < typename T >
inline Vector4< T > Vector4< T >::operator + ( const Vector4< T > & vec ) const
{
    Vector4< T > v(*this);

   return v += vec;
}

template < typename T >
template < typename U >
inline Vector4< T > & Vector4< T >::operator += ( const Vector4< U > & vec )
{
   Vector4< T > vecT(vec);

   return (*this += vecT);
}

template < typename T >
inline Vector4< T > & Vector4< T >::operator += ( const Vector4< T > & vec )
{
   mT[0] += vec.mT[0];
   mT[1] += vec.mT[1];
   mT[2] += vec.mT[2];
   mT[3] += vec.mT[3];

   return *this;
}

template < typename T >
template < typename U >
inline bool Vector4< T >::operator == ( const Vector4< U > & vec ) const
{
   return *this == Vector4< T >(vec);
}

template < typename T >
inline bool Vector4< T >::operator == ( const Vector4< T > & vec ) const
{
   return mT[0] == vec.mT[0] &&
          mT[1] == vec.mT[1] &&
          mT[2] == vec.mT[2] &&
          mT[3] == vec.mT[3];
}

template < typename T >
template < typename U >
inline bool Vector4< T >::operator != ( const Vector4< U > & vec ) const
{
   return !(*this == Vector4< T >(vec));
}

template < typename T >
inline bool Vector4< T >::operator != ( const Vector4< T > & vec ) const
{
   return !(*this == vec);
}

template < typename T >
inline T & Vector4< T >::operator [] ( const int i )
{
   WGL_ASSERT(0 <= i && i < NUM_COMPONENTS);

   return mT[i];
}

template < typename T >
inline const T & Vector4< T >::operator [] ( const int i ) const
{
   WGL_ASSERT(0 <= i && i < NUM_COMPONENTS);

   return mT[i];
}

template < typename T >
inline T & Vector4< T >::X( )
{
   return mT[0];
}

template < typename T >
inline T & Vector4< T >::Y( )
{
   return mT[1];
}

template < typename T >
inline T & Vector4< T >::Z( )
{
   return mT[2];
}

template < typename T >
inline T & Vector4< T >::W( )
{
   return mT[3];
}

template < typename T >
inline const T & Vector4< T >::X( ) const
{
   return mT[0];
}

template < typename T >
inline const T & Vector4< T >::Y( ) const
{
   return mT[1];
}

template < typename T >
inline const T & Vector4< T >::Z( ) const
{
   return mT[2];
}

template < typename T >
inline const T & Vector4< T >::W( ) const
{
   return mT[3];
}

template < typename T >
inline T * Vector4< T >::Ptr( )
{
   return mT;
}

template < typename T >
inline const T * Vector4< T >::Ptr( ) const
{
   return mT;
}

template < typename T >
template < typename U >
inline void Vector4< T >::Set( const Vector4< U > & vec )
{
   Set(vec.mT);
}

template < typename T >
inline void Vector4< T >::Set( const Vector4< T > & vec )
{
   Set(vec.mT);
}

template < typename T >
template < typename U >
inline void Vector4< T >::Set( const U & x, const U & y, const U & z, const U & w )
{
   Set(static_cast< const T >(x),
       static_cast< const T >(y),
       static_cast< const T >(z),
       static_cast< const T >(w));
}

template < typename T >
inline void Vector4< T >::Set( const T & x, const T & y, const T & z, const T & w )
{
   mT[0] = x;
   mT[1] = y;
   mT[2] = z;
   mT[3] = w;
}

template < typename T >
template < typename U >
inline void Vector4< T >::Set( const U * const pXYZW )
{
   Set(static_cast< const T >(pXYZW[0]),
       static_cast< const T >(pXYZW[1]),
       static_cast< const T >(pXYZW[2]),
       static_cast< const T >(pXYZW[3]));
}

template < typename T >
inline void Vector4< T >::Set( const T * const pXYZW )
{
   Set(pXYZW[0], pXYZW[1], pXYZW[2], pXYZW[3]);
}

template < typename T >
inline Vector4< T >::operator T * ( )
{
   return mT;
}

template < typename T >
inline Vector4< T >::operator const T * ( ) const
{
   return mT;
}

template < typename T >
inline Vector4< T > & Vector4< T >::MakeZeroVector( )
{
   Set(0, 0, 0, 1);

   return (*this);
}

template < typename T >
inline Vector4< T > Vector4< T >::UnitVector( ) const
{
   Vector4< T > v(*this);

   v.MakeUnitVector();

   return v;
}

template < typename T >
inline Vector4< T > & Vector4< T >::MakeUnitVector( )
{
   const T t = 1 / Length();

   mT[0] *= t;
   mT[1] *= t;
   mT[2] *= t;
   mT[3] *= t;

   return (*this);
}

template < typename T >
inline T Vector4< T >::Length( ) const
{
   const T & x = mT[0];
   const T & y = mT[1];
   const T & z = mT[2];
   const T & w = mT[3];

   return sqrt(x * x + y * y + z * z + w * w);
}

typedef Vector4< float >  Vec4f;
typedef Vector4< double > Vec4d;

#endif // _VECTOR4_H_
