#version 440

layout (location = 0) in vec3 vert_position;

void main( )
{
   gl_position = vec4(vert_position, 1.0);
}
