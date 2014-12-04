// defines the glsl version to be used
#version 400

// defines the attributes passed along through the shader pipeline
smooth in vec3 frag_color;

// defines the location of where the color should go
layout (location = 0) out vec4 frag_color_dest_0;

void main( )
{
   // calculate the final output
   frag_color_dest_0 = vec4(frag_color, 1.0f);
}
