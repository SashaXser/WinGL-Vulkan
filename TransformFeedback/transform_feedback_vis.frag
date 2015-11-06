// defines the glsl version to be used
#version 400 core

// defines the output color buffers
layout (location = 0) out vec4 color_buffer_0;
layout (location = 1) out uint selection_buffer_1;

// the control point is passed in by the vertex shader
flat in uint control_point_id;

void main( )
{
   // write the color to the color buffer
   color_buffer_0 = vec4(1.0f, 1.0f, 1.0f, 1.0f);

   // write the control point id to the selection buffer
   selection_buffer_1 = control_point_id;
}