#ifndef _VECTOR3_H_
#define _VECTOR3_H_

// local includes
#include "WglAssert.h"

// std includes
#include <math.h>

template < typename T >
class Vector3
{
public:
   // public typedefs
   typedef T type;

   // public enums
   enum { NUM_COMPONENTS = 3 };

    // constructor / destructor
    Vector3( );
    template < typename U >
    Vector3( const U & x, const U & y, const U & z );
    Vector3( const T & x, const T & y, const T & z );
    template < typename U >
    Vector3( const Vector3< U > & vec );
    Vector3( const Vector3< T > & vec );
    template < typename U >
    Vector3( const U u[3] );
    Vector3( const T t[3] );
   ~Vector3( );

   // operator =
   template < typename U >
   Vector3< T > & operator = ( const Vector3< U > & vec );
   Vector3< T > & operator = ( const Vector3< T > & vec );
   template < typename U >
   Vector3< T > & operator = ( const U u[3] );
   Vector3< T > & operator = ( const T t[3] );

   // scalar operator *
   template < typename U >
   Vector3< T > operator * ( const U & u ) const;
   Vector3< T > operator * ( const T & t ) const;
   template < typename U >
   Vector3< T > & operator *= ( const U & u );
   Vector3< T > & operator *= ( const T & t );

   // operator *
   template < typename U >
   T operator * ( const Vector3< U > & vec ) const;
   T operator * ( const Vector3< T > & vec ) const;
   template < typename U >
   T operator * ( const U u[3] ) const;
   T operator * ( const T t[3] ) const;

   // operator ^ (cross product)
   template < typename U >
   Vector3< T > operator ^ ( const Vector3< U > & vec ) const;
   Vector3< T > operator ^ ( const Vector3< T > & vec ) const;
   template < typename U >
   Vector3< T > & operator ^= ( const Vector3< U > & vec );
   Vector3< T > & operator ^= ( const Vector3< T > & vec );

   // operator -
   template < typename U >
   Vector3< T > operator - ( const Vector3< U > & vec ) const;
   Vector3< T > operator - ( const Vector3< T > & vec ) const;
   template < typename U >
   Vector3< T > & operator -= ( const Vector3< U > & vec );
   Vector3< T > & operator -= ( const Vector3< T > & vec );

   // operator +
   template < typename U >
   Vector3< T > operator + ( const Vector3< U > & vec ) const;
   Vector3< T > operator + ( const Vector3< T > & vec ) const;
   template < typename U >
   Vector3< T > & operator += ( const Vector3< U > & vec );
   Vector3< T > & operator += ( const Vector3< T > & vec );

   // operator ==
   template < typename U >
   bool operator == ( const Vector3< U > & vec ) const;
   bool operator == ( const Vector3< T > & vec ) const;

   // operator !=
   template < typename U >
   bool operator != ( const Vector3< U > & vec ) const;
   bool operator != ( const Vector3< T > & vec ) const;

   // operator []
   T & operator [] ( const size_t i );
   const T & operator [] ( const size_t i ) const; 

   // basic accessors into the array
   T & X( );
   T & Y( );
   T & Z( );

   const T & X( ) const;
   const T & Y( ) const;
   const T & Z( ) const;

   T * Ptr( );
   const T * Ptr( ) const;

   // sets the components
   template < typename U >
   void Set( const Vector3< U > & vec );
   void Set( const Vector3< T > & vec );
   template < typename U >
   void Set( const U & x, const U & y, const U & z );
   void Set( const T & x, const T & y, const T & z );
   template < typename U >
   void Set( const U * const pXYZ );
   void Set( const T * const pXYZ );

   // operator T
   operator T * ( );
   operator const T * ( ) const;

   // makes a zero vector
   Vector3< T > &  MakeZeroVector( );

   // makes the vector a unit vector
   Vector3< T >    UnitVector( ) const;
   Vector3< T > &  MakeUnitVector( );

   // returns the length
   T  Length( ) const;

   // vector class should be simple and allow
   // easy access to the member variables
   T     mT[3];

};

template < typename T >
inline Vector3< T >::Vector3( )
{
   MakeZeroVector();
}

template < typename T >
template < typename U >
inline Vector3< T >::Vector3( const U & x,
                              const U & y,
                              const U & z )
{
   Set(x, y, z);
}

template < typename T >
inline Vector3< T >::Vector3( const T & x,
                              const T & y,
                              const T & z )
{
   Set(x, y, z);
}

template < typename T >
template < typename U >
inline Vector3< T >::Vector3( const Vector3< U > & vec )
{
   *this = vec;
}

template < typename T >
inline Vector3< T >::Vector3( const Vector3< T > & vec )
{
   *this = vec;
}

template < typename T >
template < typename U >
inline Vector3< T >::Vector3( const U u[3] )
{
   Set(u[0], u[1], u[2]);
}

template < typename T >
inline Vector3< T >::Vector3( const T t[3] )
{
   Set(t[0], t[1], t[2]);
}

template < typename T >
inline Vector3< T >::~Vector3( )
{
}

template < typename T >
template < typename U >
inline Vector3< T > & Vector3< T >::operator = ( const Vector3< U > & vec )
{
   Set(vec);

   return *this;
}

template< typename T >
inline Vector3< T > & Vector3< T >::operator = ( const Vector3< T > & vec )
{
   if (this != &vec)
   {
      Set(vec);
   }

   return *this;
}

template < typename T >
template < typename U >
inline Vector3< T > & Vector3< T >::operator = ( const U u[3] )
{
   Set(u[0], u[1], u[2]);

   return *this;
}

template< typename T >
inline Vector3< T > & Vector3< T >::operator = ( const T t[3] )
{
   Set(t[0], t[1], t[2]);

   return *this;
}

template < typename T >
template < typename U >
inline T Vector3< T >::operator * ( const Vector3< U > & vec ) const
{
   return *this * Vector3< T >(vec);
}

template < typename T >
inline T Vector3< T >::operator * ( const Vector3< T > & vec ) const
{
   return mT[0] * vec.mT[0] +
          mT[1] * vec.mT[1] +
          mT[2] * vec.mT[2];
}

template < typename T >
template < typename U >
inline T Vector3< T >::operator * ( const U u[3] ) const
{
   return *this * Vector< T >(u);
}

template < typename T >
inline T Vector3< T >::operator * ( const T t[3] ) const
{
   return *this * Vector< T >(t);
}

template < typename T >
template < typename U >
inline Vector3< T > Vector3< T >::operator * ( const U & u ) const
{
   Vector3< T > v(*this);

   v *= u;

   return v;
}

template < typename T >
inline Vector3< T > Vector3< T >::operator * ( const T & t ) const
{
   Vector3< T > v(*this);

   v *= t;

   return v;
}

template < typename T >
template < typename U >
inline Vector3< T > & Vector3< T >::operator *= ( const U & u )
{
   *this *= static_cast< T >(u);

   return *this;
}

template < typename T >
inline Vector3< T > & Vector3< T >::operator *= ( const T & t )
{
   mT[0] *= t;
   mT[1] *= t;
   mT[2] *= t;

   return *this;
}

template < typename T >
template < typename U >
inline Vector3< T > Vector3< T >::operator ^ ( const Vector3< U > & vec ) const
{
   Vector3< T > v(vec);

   return (*this ^ v);
}

template < typename T >
inline Vector3< T > Vector3< T >::operator ^ ( const Vector3< T > & vec ) const
{
   Vector3< T > v(*this);

   return (v ^= vec);
}

template < typename T >
template < typename U >
inline Vector3< T > & Vector3< T >::operator ^= ( const Vector3< U > & vec )
{
   Vector3< T > v(vec);

   return (*this ^= v);
}

template < typename T >
inline Vector3< T > & Vector3< T >::operator ^= ( const Vector3< T > & vec )
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
inline Vector3< T > Vector3< T >::operator - ( const Vector3< U > & vec ) const
{
   Vector3< T > v(vec);

   return (*this - v);
}

template < typename T >
inline Vector3< T > Vector3< T >::operator - ( const Vector3< T > & vec ) const
{
   Vector3< T > v(*this);

   return v -= vec;
}

template < typename T >
template < typename U >
inline Vector3< T > & Vector3< T >::operator -= ( const Vector3< U > & vec )
{
   Vector3< T > vecT(vec);

   return (*this -= vecT);
}

template < typename T >
inline Vector3< T > & Vector3< T >::operator -= ( const Vector3< T > & vec )
{
   mT[0] -= vec.mT[0];
   mT[1] -= vec.mT[1];
   mT[2] -= vec.mT[2];

   return *this;
}

template < typename T >
template < typename U >
inline Vector3< T > Vector3< T >::operator + ( const Vector3< U > & vec ) const
{
   Vector3< T > v(vec);

   return (*this + v);
}

template < typename T >
inline Vector3< T > Vector3< T >::operator + ( const Vector3< T > & vec ) const
{
    Vector3< T > v(*this);

   return v += vec;
}

template < typename T >
template < typename U >
inline Vector3< T > & Vector3< T >::operator += ( const Vector3< U > & vec )
{
   Vector3< T > vecT(vec);

   return (*this += vecT);
}

template < typename T >
inline Vector3< T > & Vector3< T >::operator += ( const Vector3< T > & vec )
{
   mT[0] += vec.mT[0];
   mT[1] += vec.mT[1];
   mT[2] += vec.mT[2];

   return *this;
}

template < typename T >
template < typename U >
inline bool Vector3< T >::operator == ( const Vector3< U > & vec ) const
{
   return *this == Vector3< T >(vec);
}

template < typename T >
inline bool Vector3< T >::operator == ( const Vector3< T > & vec ) const
{
   return mT[0] == vec.mT[0] &&
          mT[1] == vec.mT[1] &&
          mT[2] == vec.mT[2];
}

template < typename T >
template < typename U >
inline bool Vector3< T >::operator != ( const Vector3< U > & vec ) const
{
   return !(*this == Vector3< T >(vec));
}

template < typename T >
inline bool Vector3< T >::operator != ( const Vector3< T > & vec ) const
{
   return !(*this == vec);
}

template < typename T >
inline T & Vector3< T >::operator [] ( const size_t i )
{
   WGL_ASSERT(0 <= i < NUM_COMPONENTS);

   return mT[i];
}

template < typename T >
inline const T & Vector3< T >::operator [] ( const size_t i ) const
{
   return (*this)[i];
}

template < typename T >
inline T & Vector3< T >::X( )
{
   return mT[0];
}

template < typename T >
inline T & Vector3< T >::Y( )
{
   return mT[1];
}

template < typename T >
inline T & Vector3< T >::Z( )
{
   return mT[2];
}

template < typename T >
inline const T & Vector3< T >::X( ) const
{
   return mT[0];
}

template < typename T >
inline const T & Vector3< T >::Y( ) const
{
   return mT[1];
}

template < typename T >
inline const T & Vector3< T >::Z( ) const
{
   return mT[2];
}

template < typename T >
inline T * Vector3< T >::Ptr( )
{
   return mT;
}

template < typename T >
inline const T * Vector3< T >::Ptr( ) const
{
   return mT;
}

template < typename T >
template < typename U >
inline void Vector3< T >::Set( const Vector3< U > & vec )
{
   Set(vec.mT[0], vec.mT[1], vec.mT[2]);
}

template < typename T >
inline void Vector3< T >::Set( const Vector3< T > & vec )
{
   Set(vec.mT[0], vec.mT[1], vec.mT[2]);
}

template < typename T >
template < typename U >
inline void Vector3< T >::Set( const U & x, const U & y, const U & z )
{
   Set(static_cast< const T >(x),
       static_cast< const T >(y),
       static_cast< const T >(z));
}

template < typename T >
inline void Vector3< T >::Set( const T & x, const T & y, const T & z )
{
   mT[0] = x;
   mT[1] = y;
   mT[2] = z;
}

template < typename T >
template < typename U >
inline void Vector3< T >::Set( const U * const pXYZ )
{
   Set(static_cast< const T >(pXYZ[0]),
       static_cast< const T >(pXYZ[1]),
       static_cast< const T >(pXYZ[2]));
}

template < typename T >
inline void Vector3< T >::Set( const T * const pXYZ )
{
   Set(pXYZ[0], pXYZ[1], pXYZ[2]);
}

template < typename T >
inline Vector3< T >::operator T * ( )
{
   return mT;
}

template < typename T >
inline Vector3< T >::operator const T * ( ) const
{
   return mT;
}

template < typename T >
inline Vector3< T > & Vector3< T >::MakeZeroVector( )
{
   Set(0, 0, 0);

   return (*this);
}

template < typename T >
inline Vector3< T > Vector3< T >::UnitVector( ) const
{
   Vector3< T > v(*this);

   v.MakeUnitVector();

   return v;
}

template < typename T >
inline Vector3< T > & Vector3< T >::MakeUnitVector( )
{
   const T t = 1 / Length();

   mT[0] *= t;
   mT[1] *= t;
   mT[2] *= t;

   return (*this);
}

template < typename T >
inline T Vector3< T >::Length( ) const
{
   const T & x = mT[0];
   const T & y = mT[1];
   const T & z = mT[2];

   return sqrt(x * x + y * y + z * z);
}

typedef Vector3< float >  Vec3f;
typedef Vector3< double > Vec3d;

#endif // _VECTOR3_H_
