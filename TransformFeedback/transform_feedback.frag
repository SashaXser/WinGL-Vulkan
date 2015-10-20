// defines the glsl version to be used
#version 400 core

layout (location = 0) out vec4 destination_color_0;
layout (location = 1) out uint destination_color_1;

void main( )
{
   destination_color_0 = vec4(1.0f, 1.0f, 1.0f, 1.0f);

   destination_color_1 = 1;
}