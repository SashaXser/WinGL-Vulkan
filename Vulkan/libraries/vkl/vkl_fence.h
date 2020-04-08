#ifndef _VKL_FENCE_H_
#define _VKL_FENCE_H_

#include "vkl_device_fwds.h"
#include "vkl_fence_fwds.h"

#include <array>
#include <cstdint>
#include <utility>

namespace vkl
{

FenceHandle CreateFence(
   const DeviceHandle & device,
   const bool signaled );

bool IsSignaled(
   const FenceHandle & fence );

template < size_t N >
extern bool Reset(
   const std::array< const FenceHandle, N > & fences );

template < typename ... T >
bool Reset(
   T && ... fences )
{
   // todo, make sure all are fences

   return
      Reset< sizeof...(T) >(
         { std::forward< T >(fences)... });
}

template < size_t N >
extern bool Wait(
   const std::array< const FenceHandle, N > & fences,
   const bool wait_for_all,
   const uint64_t timeout );

template < typename ... T >
inline bool Wait(
   const bool wait_for_all,
   const uint64_t timeout,
   T && ... fences )
{
   // todo, make sure all types are fences

   return
      Wait< sizeof...(T) >(
         { std::forward< T >(fences)... },
         wait_for_all,
         timeout);
}

} // namespace vkl

#endif // _VKL_FENCE_H_
