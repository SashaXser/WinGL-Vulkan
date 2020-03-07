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
[[deprecated]]
inline context_ptr_t AllocateContext( )
{
   return
      new (std::nothrow) context_t[CONTEXT_SIZE] { };
}

template <
   typename ContextT,
   typename ContextDataT,
   typename ... ContextDataArgsT >
inline ContextT * AllocateContext(
   ContextDataArgsT && ... args )
{
   context_t * context =
      new (std::nothrow) context_t[3] { };

   if (context)
   {
      ContextDataT * const context_data =
         new (std::nothrow) ContextDataT {
            std::forward< ContextDataArgsT >(args)...
         };

      if (!context_data)
      {
         delete [] context; context = nullptr;
      }
      else
      {
         *reinterpret_cast< ContextDataT ** >(
            context) = context_data;

         const auto delete_context_data =
            [ ] ( const ContextDataT * const context_data )
            {
               delete context_data;
            };

         *reinterpret_cast<
            void (**) ( const ContextDataT * const ) >(
               context + 1) = delete_context_data;
      }
   }

   return
      context ?
      reinterpret_cast< ContextT * >(context + 2) :
      nullptr;
}

template <
   size_t CONTEXT_SIZE,
   typename T >
[[deprecated]]
inline void DeallocateContext(
   const T * const context )
{
   delete []
      (reinterpret_cast< const context_t * >(
         context) - (CONTEXT_SIZE - 1));
}

inline void DeallocateContext(
   const void * const context )
{
   if (context)
   {
      const auto context_data =
         reinterpret_cast< const context_t * >(
            context) - 2;

      const auto delete_context_data =
         reinterpret_cast<
            void (* const) ( const void * const ) >(
               *(context_data + 1));

      if (delete_context_data)
      {
         delete_context_data(
            reinterpret_cast< void * >(*context_data));
      }

      delete [] context_data;
   }
}

template<
   size_t CONTEXT_SIZE,
   typename T >
[[deprecated]]
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
[[deprecated]]
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
[[deprecated]]
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

template <
   typename ContextDataT,
   typename ContextT >
inline ContextDataT * GetContextData(
   ContextT * const context )
{
   ContextDataT * context_store { };

   if (context && *context && *(context - 2))
   {
      context_store =
         reinterpret_cast< ContextDataT * >(
            *(context - 2));
   }

   return context_store;
}

template <
   typename ContextT,
   typename ContextDataT,
   typename MemberVariableT >
inline MemberVariableT GetContextData(
   const ContextT * const context,
   MemberVariableT ContextDataT::* const context_data )
{
   MemberVariableT data { };

   if (context_data)
   {
      const auto * const context_store =
         GetContextData< ContextDataT >(
            context);

      if (context_store)
      {
         data = context_store->*context_data;
      }
   }

   return data;
}

} // namespace vkl::internal

#endif // _VKL_CONTEXT_DATA_H_
