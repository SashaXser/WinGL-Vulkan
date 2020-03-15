#ifndef _VKL_CONTEXT_DATA_H_
#define _VKL_CONTEXT_DATA_H_

#include <cstdint>
#include <new>

namespace vkl::internal
{

using context_t = std::uintptr_t;
using context_ptr_t = context_t *;

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
