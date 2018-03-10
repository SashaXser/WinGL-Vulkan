#ifndef _glstudio_component_interface_impl_h_
#define _glstudio_component_interface_impl_h_

#include "component.h"

#include "Texture.h"
#include "ShaderProgram.h"
#include "VertexBufferObject.h"

namespace glsc
{

class ComponentImpl : public Component
{
public:
   virtual void Draw( );

   void Initialize( );

   Texture _texture1;
   Texture _texture2;
   Texture _texture3;

   ShaderProgram _basic_shader;

   VertexBufferObject _basic_plane_verts;
   VertexBufferObject _basic_plane_indices;
   VertexBufferObject _basic_plane_tex_coords;

};

} // namespace glsc

#endif // _glstudio_component_interface_impl_h_
