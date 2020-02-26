#ifndef _VKL_CONTEXT_DATA_H_
#define _VKL_CONTEXT_DATA_H_

// todo: need to change from size_t * to uint64_t *

#include <type_traits>

namespace vkl::internal
{

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
   size_t INDEX,
   size_t CONTEXT_SIZE,
   typename T >
inline void SetContextData(
   size_t * const context,
   const T data )
{
   if (context)
   {
      *(context + (CONTEXT_SIZE - INDEX - 1)) =
         details::reinterpret_static_cast< size_t >(
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
            reinterpret_cast< const size_t * >(
               context) - INDEX);
   }

   return data;
}

} // namespace vkl::internal

#endif // _VKL_CONTEXT_DATA_H_
