// defines the glsl version to be used
#version 400 core

// defines the per vertex attributes
layout (location = 0) in vec3 vertex_position;

void main( )
{
   // allow the position to just pass through
   gl_Position = vec4(vertex_position, 1.0f);
}
