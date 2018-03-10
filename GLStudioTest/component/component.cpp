#include "component.h"

#include <new>
#include <cassert>

namespace glsc
{

Component::~Component( )
{
}

void Component::operator delete( void * pointer )
{
   ::operator delete(pointer);
}

} // namespace glsc
