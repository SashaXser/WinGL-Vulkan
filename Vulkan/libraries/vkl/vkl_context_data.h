#ifndef _VKL_CONTEXT_DATA_H_
#define _VKL_CONTEXT_DATA_H_

#include <cstddef>
#include <cstdint>
#include <new>
#include <type_traits>

namespace vkl::internal
{

using context_t = uint64_t;
using context_ptr_t = context_t *;

namespace details
{

template <
   typename T,
   typename F >
inline
std::enable_if_t<
   !std::is_convertible_v< F, T >,
   T >
reinterpret_static_cast( const F f )
{
   return
      reinterpret_cast< T >(f);
}

template <
   typename T,
   typename F >
inline
std::enable_if_t<
   std::is_convertible_v< F, T >,
   T >
reinterpret_static_cast( const F f )
{
   return
      static_cast< T >(f);
}

} // namespace details

template <
   size_t CONTEXT_SIZE >
inline context_ptr_t AllocateContext( )
{
   return
      new (std::nothrow) context_t[CONTEXT_SIZE] { };
}

template <
   size_t CONTEXT_SIZE,
   typename T >
inline void DeallocateContext(
   const T * const context )
{
   delete []
      (reinterpret_cast< const context_t * >(
         context) - (CONTEXT_SIZE - 1));
}

template<
   size_t CONTEXT_SIZE,
   typename T >
inline T * GetContextPointer(
   const context_ptr_t context )
{
   return
      reinterpret_cast< T * >(
         context + (CONTEXT_SIZE - 1));
}

template <
   size_t INDEX,
   size_t CONTEXT_SIZE,
   typename T >
inline void SetContextData(
   const context_ptr_t context,
   const T data )
{
   static_assert(sizeof(T) <= sizeof(context_t));

   if (context)
   {
      *(context + (CONTEXT_SIZE - INDEX - 1)) =
         details::reinterpret_static_cast< context_t >(
            data);
   }
}

template <
   typename T,
   size_t INDEX,
   typename C >
inline T GetContextData(
   const C * const context )
{
   T data { };

   if (context && *context)
   {
      data =
         *reinterpret_cast< const T * >(
            reinterpret_cast< const context_t * >(
               context) - INDEX);
   }

   return data;
}

} // namespace vkl::internal

#endif // _VKL_CONTEXT_DATA_H_
