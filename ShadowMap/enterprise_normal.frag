// defines the glsl version to be used
#version 440 core

// defines the location of where the color should go
layout (location = 0) out vec4 frag_color_dest_0;

void main( )
{
   // render the lines all white
   frag_color_dest_0 = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
