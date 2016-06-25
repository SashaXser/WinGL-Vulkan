// defines the glsl version to be used
// leave commented out, as the lighting shader source declares it
#version 440 core

// defines the per vertex attributes
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec2 tex_coord;

// defines the per geometry attributes

// defines the attributes passed along through the shader pipeline
out VS_OUT
{
   smooth vec2 tex_coord;
} vs_out;

void main( )
{
   // pass the texture coordinates along
   vs_out.tex_coord = tex_coord;

   // just pass the vertex along
   gl_Position = vec4(vertex, 1.0f);
}
