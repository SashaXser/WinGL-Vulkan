// defines the glsl version to be used
#version 400 core

layout (location = 0) out vec4 destination_color_0;
layout (location = 1) out uint destination_color_1;

flat in uint control_point_id;

uniform bool allow_recording;

void main( )
{
   destination_color_0 = vec4(1.0f, 1.0f, 1.0f, 1.0f);

   destination_color_1 = control_point_id;
}