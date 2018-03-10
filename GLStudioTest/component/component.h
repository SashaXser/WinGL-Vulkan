#ifndef _glstudio_component_interface_h_
#define _glstudio_component_interface_h_

#ifdef glstudio_component_EXPORTS
#  define GLSC_EXPORT __declspec( dllexport )
#else
#  define GLSC_EXPORT __declspec( dllimport )
#endif

namespace glsc
{

class GLSC_EXPORT Component
{
public:
   virtual ~Component( ) = 0;

   virtual void Draw( ) = 0;

   static void operator delete( void * pointer );

};

GLSC_EXPORT Component * CreateComponent( );

} // namespace glsc

#endif // _glstudio_component_interface_h_
