#ifndef _VECTOR_H_
#define _VECTOR_H_

// wgl includes
#include "WglAssert.h"

// std includes
#include <cmath>
#include <cstring>
#include <cstdint>
#include <iterator>
#include <algorithm>
#include <type_traits>

// even with the use of the enable_if_t, the compiler
// includes all the declarations of the class interface,
// even the ones considered an error.
#pragma warning( push )
#pragma warning( disable : 4521 )

template < typename T, uint32_t SIZE >
class Vector
{
public:
   // public typedefs
   typedef T type;

   // public enums
   enum { NUM_COMPONENTS = SIZE };

   // only support 2, 3, and 4 component vectors
   static_assert(SIZE == 2 || SIZE == 3 || SIZE == 4, "only support 2, 3, and 4 component vectors");

    // constructor / destructor
    Vector( );
    Vector( const T & s );
    template < typename U, typename = std::enable_if_t< SIZE == 2 > >
    Vector( const U & x, const U & y );
    template < typename = std::enable_if_t< SIZE == 2 > >
    Vector( const T & x, const T & y );
    template < typename U, typename = std::enable_if_t< SIZE == 3 > >
    Vector( const U & x, const U & y, const U & z );
    template < typename = std::enable_if_t< SIZE == 3 > >
    Vector( const T & x, const T & y, const T & z );
    template < typename U, typename = std::enable_if_t< SIZE == 4 > >
    Vector( const U & x, const U & y, const U & z, const U & w = 1 );
    template < typename = std::enable_if_t< SIZE == 4 > >
    Vector( const T & x, const T & y, const T & z, const T & w = 1 );
    template < typename U, typename = std::enable_if_t< SIZE == 4 > >
    Vector( const Vector< U, 3 > & vec, const U & w = 1 );
    template < typename = std::enable_if_t< SIZE == 4 > >
    Vector( const Vector< T, 3 > & vec, const T & w = 1 );
    template < typename U >
    Vector( const Vector< U, SIZE > & vec );
    Vector( const Vector< T, SIZE > & vec );
    template < typename U >
    Vector( const U u[SIZE] );
    Vector( const T t[SIZE] );
   ~Vector( );

   // operator =
   template < typename U >
   Vector< T, SIZE > & operator = ( const Vector< U, SIZE > & vec );
   Vector< T, SIZE > & operator = ( const Vector< T, SIZE > & vec );
   template < typename U >
   Vector< T, SIZE > & operator = ( const U u[SIZE] );
   Vector< T, SIZE > & operator = ( const T t[SIZE] );

   // scalar operator *
   template < typename U >
   Vector< T, SIZE > operator * ( const U & u ) const;
   Vector< T, SIZE > operator * ( const T & t ) const;
   template < typename U >
   Vector< T, SIZE > & operator *= ( const U & u );
   Vector< T, SIZE > & operator *= ( const T & t );

   // operator * (dot product)
   template < typename U >
   T operator * ( const Vector< U, SIZE > & vec ) const;
   T operator * ( const Vector< T, SIZE > & vec ) const;
   template < typename U >
   T operator * ( const U u[SIZE] ) const;
   T operator * ( const T t[SIZE] ) const;

   // operator ^ (cross product)
   template < typename U >
   Vector< T, SIZE > operator ^ ( const Vector< U, SIZE > & vec ) const;
   Vector< T, SIZE > operator ^ ( const Vector< T, SIZE > & vec ) const;
   template < typename U >
   Vector< T, SIZE > & operator ^= ( const Vector< U, SIZE > & vec );
   Vector< T, SIZE > & operator ^= ( const Vector< T, SIZE > & vec );

   // operator % (component multiplication)
   template < typename U >
   Vector< T, SIZE > operator % ( const Vector< U, SIZE > & vec ) const;
   Vector< T, SIZE > operator % ( const Vector< T, SIZE > & vec ) const;
   template < typename U >
   Vector< T, SIZE > & operator %= ( const Vector< U, SIZE > & vec );
   Vector< T, SIZE > & operator %= ( const Vector< T, SIZE > & vec );

   // operator -
   template < typename U >
   Vector< T, SIZE > operator - ( const Vector< U, SIZE > & vec ) const;
   Vector< T, SIZE > operator - ( const Vector< T, SIZE > & vec ) const;
   template < typename U >
   Vector< T, SIZE > & operator -= ( const Vector< U, SIZE > & vec );
   Vector< T, SIZE > & operator -= ( const Vector< T, SIZE > & vec );

   // operator +
   template < typename U >
   Vector< T, SIZE > operator + ( const Vector< U, SIZE > & vec ) const;
   Vector< T, SIZE > operator + ( const Vector< T, SIZE > & vec ) const;
   template < typename U >
   Vector< T, SIZE > & operator += ( const Vector< U, SIZE > & vec );
   Vector< T, SIZE > & operator += ( const Vector< T, SIZE > & vec );

   // operator ==
   template < typename U >
   bool operator == ( const Vector< U, SIZE > & vec ) const;
   bool operator == ( const Vector< T, SIZE > & vec ) const;

   // operator !=
   template < typename U >
   bool operator != ( const Vector< U, SIZE > & vec ) const;
   bool operator != ( const Vector< T, SIZE > & vec ) const;

   // operator []
   T & operator [] ( const ptrdiff_t i );
   const T & operator [] ( const ptrdiff_t i ) const;

   // operator T *
   operator T * ( );
   operator const T * ( ) const;

   T * Ptr( );
   const T * Ptr( ) const;

   // conversion operator
   template < typename U >
   operator Vector< U, SIZE > ( ) const;

   // basic accessors into the array
   T & X( );
   T & Y( );
   T & Z( );
   T & W( );

   const T & X( ) const;
   const T & Y( ) const;
   const T & Z( ) const;
   const T & W( ) const;

   // sets the components
   template < typename U >
   void Set( const Vector< U, SIZE > & vec );
   void Set( const Vector< T, SIZE > & vec );
   template < typename U, typename = std::enable_if_t< SIZE == 2 > >
   void Set( const U & x, const U & y );
   template < typename = std::enable_if_t< SIZE == 2 > >
   void Set( const T & x, const T & y );
   template < typename U, typename = std::enable_if_t< SIZE == 3 > >
   void Set( const U & x, const U & y, const U & z );
   template < typename = std::enable_if_t< SIZE == 3 > >
   void Set( const T & x, const T & y, const T & z );
   template < typename U, typename = std::enable_if_t< SIZE == 4 > >
   void Set( const U & x, const U & y, const U & z, const U & w = 1 );
   template < typename = std::enable_if_t< SIZE == 2 > >
   void Set( const T & x, const T & y, const T & z, const T & w = 1 );
   template < typename U >
   void Set( const U * const pXYZ );
   void Set( const T * const pXYZ );

   // makes a zero vector
   Vector< T, SIZE > &  MakeZeroVector( );

   // makes the vector a unit vector
   T  Normalize( );
   Vector< T, SIZE >    UnitVector( ) const;
   Vector< T, SIZE > &  MakeUnitVector( );

   // returns the length
   T  Length( ) const;

   // returns the size of the vector
   uint32_t Size( ) const;

   // vector class should be simple and allow
   // easy access to the member variables
   T     mT[SIZE];

};

template < typename T, uint32_t SIZE >
inline Vector< T, SIZE >::Vector( )
{
   MakeZeroVector();
}

template < typename T, uint32_t SIZE >
inline Vector< T, SIZE >::Vector( const T & s )
{
   std::for_each(mT, mT + SIZE, [ &s ] ( T & t ) { t = s; });
}

template < typename T, uint32_t SIZE >
template < typename U, typename >
inline Vector< T, SIZE >::Vector( const U & x, const U & y )
{
   mT[0] = x; mT[1] = y;
}

template < typename T, uint32_t SIZE >
template < typename >
inline Vector< T, SIZE >::Vector( const T & x, const T & y )
{
   mT[0] = x; mT[1] = y;
}

template < typename T, uint32_t SIZE >
template < typename U, typename >
inline Vector< T, SIZE >::Vector( const U & x, const U & y, const U & z )
{
   mT[0] = x; mT[1] = y; mT[2] = z;
}

template < typename T, uint32_t SIZE >
template < typename >
inline Vector< T, SIZE >::Vector( const T & x, const T & y, const T & z )
{
   mT[0] = x; mT[1] = y; mT[2] = z;
}

template < typename T, uint32_t SIZE >
template < typename U, typename >
inline Vector< T, SIZE >::Vector( const U & x, const U & y, const U & z, const U & w )
{
   mT[0] = x; mT[1] = y; mT[2] = z; mT[3] = w;
}

template < typename T, uint32_t SIZE >
template < typename >
inline Vector< T, SIZE >::Vector( const T & x, const T & y, const T & z, const T & w )
{
   mT[0] = x; mT[1] = y; mT[2] = z; mT[3] = w;
}

template < typename T, uint32_t SIZE >
template < typename U, typename >
inline Vector< T, SIZE >::Vector( const Vector< U, 3 > & vec, const U & w )
{
   mT[0] = vec.mT[0]; mT[1] = vec.mT[1]; mT[2] = vec.mT[2]; mT[3] = w;
}

template < typename T, uint32_t SIZE >
template < typename >
inline Vector< T, SIZE >::Vector( const Vector< T, 3 > & vec, const T & w )
{
   mT[0] = vec.mT[0]; mT[1] = vec.mT[1]; mT[2] = vec.mT[2]; mT[3] = w;
}

template < typename T, uint32_t SIZE >
template < typename U >
inline Vector< T, SIZE >::Vector( const Vector< U, SIZE > & vec )
{
   std::for_each(mT, mT + SIZE, [ this, &vec ] ( T & t ) { t = vec.mT[std::distance(mT, &t)]; });
}

template < typename T, uint32_t SIZE >
inline Vector< T, SIZE >::Vector( const Vector< T, SIZE > & vec )
{
   std::memcpy(mT, vec.mT, sizeof(mT));
}

template < typename T, uint32_t SIZE >
template < typename U >
inline Vector< T, SIZE >::Vector( const U u[SIZE] )
{
   std::for_each(mT, mT + SIZE, [ this, &u ] ( T & t ) { t = u[std::distance(mT, &t)]; });
}

template < typename T, uint32_t SIZE >
inline Vector< T, SIZE >::Vector( const T t[SIZE] )
{
   std::memcpy(mT, t, sizeof(mT));
}

template < typename T, uint32_t SIZE >
inline Vector< T, SIZE >::~Vector(  )
{
}

template < typename T, uint32_t SIZE >
template < typename U >
inline Vector< T, SIZE > & Vector< T, SIZE >::operator = ( const Vector< U, SIZE > & vec )
{
   std::for_each(mT, mT + SIZE, [ this, &vec ] ( T & t ) { t = vec.mT[std::distance(mT, &t)]; });

   return *this;
}

template < typename T, uint32_t SIZE >
inline Vector< T, SIZE > & Vector< T, SIZE >::operator = ( const Vector< T, SIZE > & vec )
{
   if (this != &vec)
   {
      std::memcpy(mT, vec.mT, sizeof(mT));
   }

   return *this;
}

template < typename T, uint32_t SIZE >
template < typename U >
inline Vector< T, SIZE > & Vector< T, SIZE >::operator = ( const U u[SIZE] )
{
   *this = Vector< U, SIZE >(u);

   return *this;
}

template < typename T, uint32_t SIZE >
inline Vector< T, SIZE > & Vector< T, SIZE >::operator = ( const T t[SIZE] )
{
   *this = Vector< T, SIZE >(t);

   return *this;
}

template < typename T, uint32_t SIZE >
template < typename U >
inline Vector< T, SIZE > Vector< T, SIZE >::operator * ( const U & u ) const
{
   return Vector< T, SIZE >(*this) *= u;
}

template < typename T, uint32_t SIZE >
inline Vector< T, SIZE > Vector< T, SIZE >::operator * ( const T & t ) const
{
   return Vector< T, SIZE >(*this) *= t;
}

template < typename T, uint32_t SIZE >
template < typename U >
inline Vector< T, SIZE > & Vector< T, SIZE >::operator *= ( const U & u )
{
   std::for_each(mT, mT + SIZE, [ &u ] ( T & t ) { t *= u; });

   return *this;
}

template < typename T, uint32_t SIZE >
inline Vector< T, SIZE > & Vector< T, SIZE >::operator *= ( const T & t )
{
   std::for_each(mT, mT + SIZE, [ &t ] ( T & mt ) { mt *= t; });

   return *this;
}

template < typename T, uint32_t SIZE >
template < typename U >
inline T Vector< T, SIZE >::operator * ( const Vector< U, SIZE > & vec ) const
{
   return *this * vec.mT;
}

template < typename T, uint32_t SIZE >
inline T Vector< T, SIZE >::operator * ( const Vector< T, SIZE > & vec ) const
{
   return *this * vec.mT;
}

template < typename T, uint32_t SIZE >
template < typename U >
inline T Vector< T, SIZE >::operator * ( const U u[SIZE] ) const
{
   T dot = 0;

   std::for_each(mT, mT + SIZE, [ this, &u, &dot ] ( const T & t ) { dot += (t * u[std::distance(mT, &t)]); });
   
   return dot;
}

template < typename T, uint32_t SIZE >
inline T Vector< T, SIZE >::operator * ( const T t[SIZE] ) const
{
   T dot = 0;

   std::for_each(mT, mT + SIZE, [ this, &t, &dot ] ( const T & mt ) { dot += (mt * t[std::distance(mT, &mt)]); });
   
   return dot;
}

template < typename T, uint32_t SIZE >
template < typename U >
inline Vector< T, SIZE > Vector< T, SIZE >::operator ^ ( const Vector< U, SIZE > & vec ) const
{
   return Vector< T, SIZE >(*this) ^= vec;
}

template < typename T, uint32_t SIZE >
inline Vector< T, SIZE > Vector< T, SIZE >::operator ^ ( const Vector< T, SIZE > & vec ) const
{
   return Vector< T, SIZE >(*this) ^= vec;
}

namespace details
{

template < typename T, typename U >
inline Vector< T, 2 > vector_cross( const Vector< T, 2 > & lhs, const Vector< U, 2 > & rhs )
{
   static_assert(false, "cross product not supported for 2D vectors")
}

template < typename T, typename U >
inline Vector< T, 3 > vector_cross( const Vector< T, 3 > & lhs, const Vector< U, 3 > & rhs )
{
   return Vector< T, 3 >((lhs.mT[1] * rhs.mT[2]) - (lhs.mT[2] * rhs.mT[1]),
                         (lhs.mT[2] * rhs.mT[0]) - (lhs.mT[0] * rhs.mT[2]),
                         (lhs.mT[0] * rhs.mT[1]) - (lhs.mT[1] * rhs.mT[0]));
}

template < typename T, typename U >
inline Vector< T, 4 > vector_cross( const Vector< T, 4 > & lhs, const Vector< U, 4 > & rhs )
{
   WGL_ASSERT(lhs.mT[3] == 1 && rhs.mT[3] == 1 && "Not Constrained To Homogeneous R3 Space");

   return Vector< T, 4 >(vector_cross(Vector< T, 3 >(lhs.mT), Vector< U, 3 >(rhs.mT)));
}

} // namespace details

template < typename T, uint32_t SIZE >
template < typename U >
inline Vector< T, SIZE > & Vector< T, SIZE >::operator ^= ( const Vector< U, SIZE > & vec )
{
   *this = details::vector_cross(*this, vec);

   return *this;
}

template < typename T, uint32_t SIZE >
inline Vector< T, SIZE > & Vector< T, SIZE >::operator ^= ( const Vector< T, SIZE > & vec )
{
   *this = details::vector_cross(*this, vec);

   return *this;
}

template < typename T, uint32_t SIZE >
template < typename U >
inline Vector< T, SIZE > Vector< T, SIZE >::operator % ( const Vector< U, SIZE > & vec ) const
{
   return Vector< T, SIZE >(*this) %= vec;
}

template < typename T, uint32_t SIZE >
inline Vector< T, SIZE > Vector< T, SIZE >::operator % ( const Vector< T, SIZE > & vec ) const
{
   return Vector< T, SIZE >(*this) %= vec;
}

template < typename T, uint32_t SIZE >
template < typename U >
inline Vector< T, SIZE > & Vector< T, SIZE >::operator %= ( const Vector< U, SIZE > & vec )
{
   std::for_each(mT, mT + SIZE, [ this, &vec ] ( T & t ) { t = t * vec.mT[std::distance(mT, &t)]; });

   return *this;
}

template < typename T, uint32_t SIZE >
inline Vector< T, SIZE > & Vector< T, SIZE >::operator %= ( const Vector< T, SIZE > & vec )
{
   std::for_each(mT, mT + SIZE, [ this, &vec ] ( T & t ) { t = t * vec.mT[std::distance(mT, &t)]; });

   return *this;
}

template < typename T, uint32_t SIZE >
template < typename U >
inline Vector< T, SIZE > Vector< T, SIZE >::operator - ( const Vector< U, SIZE > & vec ) const
{
   return Vector< T, SIZE >(*this) -= vec;
}

template < typename T, uint32_t SIZE >
inline Vector< T, SIZE > Vector< T, SIZE >::operator - ( const Vector< T, SIZE > & vec ) const
{
   return Vector< T, SIZE >(*this) -= vec;
}

template < typename T, uint32_t SIZE >
template < typename U >
inline Vector< T, SIZE > & Vector< T, SIZE >::operator -= ( const Vector< U, SIZE > & vec )
{
   std::for_each(mT, mT + SIZE, [ this, &vec ] ( T & t ) { t = t - vec.mT[std::distance(mT, &t)]; });

   return *this;
}

template < typename T, uint32_t SIZE >
inline Vector< T, SIZE > & Vector< T, SIZE >::operator -= ( const Vector< T, SIZE > & vec )
{
   std::for_each(mT, mT + SIZE, [ this, &vec ] ( T & t ) { t = t - vec.mT[std::distance(mT, &t)]; });

   return *this;
}

template < typename T, uint32_t SIZE >
template < typename U >
inline Vector< T, SIZE > Vector< T, SIZE >::operator + ( const Vector< U, SIZE > & vec ) const
{
   return Vector< T, SIZE >(*this) += vec;
}

template < typename T, uint32_t SIZE >
inline Vector< T, SIZE > Vector< T, SIZE >::operator + ( const Vector< T, SIZE > & vec ) const
{
   return Vector< T, SIZE >(*this) += vec;
}

template < typename T, uint32_t SIZE >
template < typename U >
inline Vector< T, SIZE > & Vector< T, SIZE >::operator += ( const Vector< U, SIZE > & vec )
{
   std::for_each(mT, mT + SIZE, [ this, &vec ] ( T & t ) { t = t + vec.mT[std::distance(mT, &t)]; });

   return *this;
}

template < typename T, uint32_t SIZE >
inline Vector< T, SIZE > & Vector< T, SIZE >::operator += ( const Vector< T, SIZE > & vec )
{
   std::for_each(mT, mT + SIZE, [ this, &vec ] ( T & t ) { t = t + vec.mT[std::distance(mT, &t)]; });

   return *this;
}

template < typename T, uint32_t SIZE >
template < typename U >
inline bool Vector< T, SIZE >::operator == ( const Vector< U, SIZE > & vec ) const
{
   return *this == Vector< T, SIZE >(vec.mT);
}

template < typename T, uint32_t SIZE >
inline bool Vector< T, SIZE >::operator == ( const Vector< T, SIZE > & vec ) const
{
   return std::memcmp(mT, vec.mT, sizeof(mT)) == 0;
}

template < typename T, uint32_t SIZE >
template < typename U >
inline bool Vector< T, SIZE >::operator != ( const Vector< U, SIZE > & vec ) const
{
   return *this != Vector< T, SIZE >(vec.mT);
}

template < typename T, uint32_t SIZE >
inline bool Vector< T, SIZE >::operator != ( const Vector< T, SIZE > & vec ) const
{
   return !(*this == vec);
}

template < typename T, uint32_t SIZE >
inline T & Vector< T, SIZE >::operator [] ( const ptrdiff_t i )
{
   WGL_ASSERT(i >= 0 && i < SIZE);

   return mT[i];
}

template < typename T, uint32_t SIZE >
inline const T & Vector< T, SIZE >::operator [] ( const ptrdiff_t i ) const
{
   WGL_ASSERT(i >= 0 && i < SIZE);

   return mT[i];
}

template < typename T, uint32_t SIZE >
inline Vector< T, SIZE >::operator T * ( )
{
   return mT;
}

template < typename T, uint32_t SIZE >
inline Vector< T, SIZE >::operator const T * ( ) const
{
   return mT;
}

template < typename T, uint32_t SIZE >
inline T * Vector< T, SIZE >::Ptr( )
{
   return mT;
}

template < typename T, uint32_t SIZE >
inline const T * Vector< T, SIZE >::Ptr( ) const
{
   return mT;
}

namespace details
{

template < typename U, typename T >
Vector< U, 2 > static_cast_op( const Vector< T, 2 > & t )
{
   Vector< U, 2 > u;

   u.mT[0] = static_cast< U >(t.mT[0]);
   u.mT[1] = static_cast< U >(t.mT[1]);

   return u;
}

template < typename U, typename T >
Vector< U, 3 > static_cast_op( const Vector< T, 3 > & t )
{
   Vector< U, 3 > u;

   u.mT[0] = static_cast< U >(t.mT[0]);
   u.mT[1] = static_cast< U >(t.mT[1]);
   u.mT[2] = static_cast< U >(t.mT[2]);

   return u;
}

template < typename U, typename T >
Vector< U, 4 > static_cast_op( const Vector< T, 4 > & t )
{
   Vector< U, 4 > u;

   u.mT[0] = static_cast< U >(t.mT[0]);
   u.mT[1] = static_cast< U >(t.mT[1]);
   u.mT[2] = static_cast< U >(t.mT[2]);
   u.mT[3] = static_cast< U >(t.mT[3]);

   return u;
}

} // namespace details

template < typename T, uint32_t SIZE >
template < typename U >
inline Vector< T, SIZE >::operator Vector< U, SIZE > ( ) const
{
   return details::static_cast_op< U >(*this);
}

template < typename T, uint32_t SIZE >
inline T & Vector< T, SIZE >::X( )
{
   return mT[0];
}

template < typename T, uint32_t SIZE >
inline T & Vector< T, SIZE >::Y( )
{
   return mT[1];
}

template < typename T, uint32_t SIZE >
inline T & Vector< T, SIZE >::Z( )
{
   static_assert(SIZE == 3 || SIZE == 4, "z component not allowed for this size vector");

   return mT[2];
}

template < typename T, uint32_t SIZE >
inline T & Vector< T, SIZE >::W( )
{
   static_assert(SIZE == 4, "w component not allowed for this size vector");

   return mT[3];
}

template < typename T, uint32_t SIZE >
inline const T & Vector< T, SIZE >::X( ) const
{
   return mT[0];
}

template < typename T, uint32_t SIZE >
inline const T & Vector< T, SIZE >::Y( ) const
{
   return mT[1];
}

template < typename T, uint32_t SIZE >
inline const T & Vector< T, SIZE >::Z( ) const
{
   static_assert(SIZE == 3 || SIZE == 4, "z component not allowed for this size vector");

   return mT[2];
}

template < typename T, uint32_t SIZE >
inline const T & Vector< T, SIZE >::W( ) const
{
   static_assert(SIZE == 4, "w component not allowed for this size vector");

   return mT[3];
}

template < typename T, uint32_t SIZE >
template < typename U >
inline void Vector< T, SIZE >::Set( const Vector< U, SIZE > & vec )
{
   *this = vec;
}

template < typename T, uint32_t SIZE >
inline void Vector< T, SIZE >::Set( const Vector< T, SIZE > & vec )
{
   *this = vec;
}

template < typename T, uint32_t SIZE >
template < typename U, typename >
inline void Vector< T, SIZE >::Set( const U & x, const U & y )
{
   Set(Vector< U, SIZE >(x, y));
}

template < typename T, uint32_t SIZE >
template < typename >
inline void Vector< T, SIZE >::Set( const T & x, const T & y )
{
   Set(Vector< T, SIZE >(x, y));
}

template < typename T, uint32_t SIZE >
template < typename U, typename >
inline void Vector< T, SIZE >::Set( const U & x, const U & y, const U & z )
{
   Set(Vector< U, SIZE >(x, y, z));
}

template < typename T, uint32_t SIZE >
template < typename >
inline void Vector< T, SIZE >::Set( const T & x, const T & y, const T & z )
{
   Set(Vector< T, SIZE >(x, y, z));
}

template < typename T, uint32_t SIZE >
template < typename U, typename >
inline void Vector< T, SIZE >::Set( const U & x, const U & y, const U & z, const U & w )
{
   Set(Vector< U, SIZE >(x, y, z, w));
}

template < typename T, uint32_t SIZE >
template < typename >
inline void Vector< T, SIZE >::Set( const T & x, const T & y, const T & z, const T & w )
{
   Set(Vector< T, SIZE >(x, y, z, w));
}

template < typename T, uint32_t SIZE >
template < typename U >
inline void Vector< T, SIZE >::Set( const U * const pCompnents )
{
   Set(Vector< U, SIZE >(pCompnents));
}

template < typename T, uint32_t SIZE >
inline void Vector< T, SIZE >::Set( const T * const pCompnents )
{
   Set(Vector< T, SIZE >(pCompnents));
}

namespace details
{

template < typename T >
inline void zero_vector( Vector< T, 2 > & vec )
{
   vec.mT[0] = 0; vec.mT[1] = 0;
}

template < typename T >
inline void zero_vector( Vector< T, 3 > & vec )
{
   vec.mT[0] = 0; vec.mT[1] = 0; vec.mT[2] = 0;
}

template < typename T >
inline void zero_vector( Vector< T, 4 > & vec )
{
   vec.mT[0] = 0; vec.mT[1] = 0; vec.mT[2] = 0; vec.mT[3] = 1;
}

} // namespace details

template < typename T, uint32_t SIZE >
Vector< T, SIZE > & Vector< T, SIZE >::MakeZeroVector( )
{
   details::zero_vector(*this);

   return *this;
}

template < typename T, uint32_t SIZE >
inline T Vector< T, SIZE >::Normalize( )
{
   const T length = Length();
   const T one_over_length = 1 / length;

   *this *= one_over_length;

   return length;
}

template < typename T, uint32_t SIZE >
inline Vector< T, SIZE > Vector< T, SIZE >::UnitVector( ) const
{
   Vector< T, SIZE > v(*this);

   v.MakeUnitVector();

   return v;
}

template < typename T, uint32_t SIZE >
inline Vector< T, SIZE > & Vector< T, SIZE >::MakeUnitVector( )
{
   Normalize();

   return *this;
}

template < typename T, uint32_t SIZE >
inline T Vector< T, SIZE >::Length( ) const
{
   T squared_sum = 0;

   std::for_each(mT, mT + SIZE, [ &squared_sum ] ( const T & t ) { squared_sum += (t * t); });

   return std::sqrt(squared_sum);
}

template < typename T, uint32_t SIZE >
inline uint32_t Vector< T, SIZE >::Size( ) const
{
   return SIZE;
}

typedef Vector< float, 2 >  Vec2f;
typedef Vector< double, 2 > Vec2d;
typedef Vector< float, 3 >  Vec3f;
typedef Vector< double, 3 > Vec3d;
typedef Vector< float, 4 >  Vec4f;
typedef Vector< double, 4 > Vec4d;

// even with the use of the enable_if_t, the compiler
// includes all the declarations of the class interface,
// even the ones considered an error.
#pragma warning( pop )

#endif // _VECTOR_H_
