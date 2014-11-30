#version 400

uniform sampler2D tex_unit_0;
uniform sampler2D tex_unit_1;

smooth in vec2 frag_tex_coords;

layout (location = 0) out vec4 frag_color_dest_0;

void main( )
{
   if (frag_tex_coords.s >= 0.5)
   {
      frag_color_dest_0 = texture(tex_unit_1, frag_tex_coords);
   }
   else
   {
      frag_color_dest_0 = texture(tex_unit_0, frag_tex_coords);
   }
}
