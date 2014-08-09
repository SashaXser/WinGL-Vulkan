#version 410
layout (location = 0) in vec3 vert_position;
uniform mat4 mvp;
out vec4 vert_world_pos_cs;
void main( )
{
gl_Position = mvp * vec4(vert_position, 1.0f);
}
