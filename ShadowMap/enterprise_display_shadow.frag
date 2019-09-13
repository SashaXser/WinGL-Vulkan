// defines the glsl version to be used
#version 440 core

// defines the per geometry attributes
uniform sampler2D shadow_texture;

// defines the attributes passed along through the shader pipeline
smooth in vec2 frag_tex_coords;

// defines the location of where the color should go
layout (location = 0) out vec4 frag_color_dest_0;

void main( )
{
   // do the lookup and just place it in the output
   const float depth =
      texture(shadow_texture, frag_tex_coords).r;
   frag_color_dest_0 =
      vec4(depth, depth, depth, 1.0f);
}
