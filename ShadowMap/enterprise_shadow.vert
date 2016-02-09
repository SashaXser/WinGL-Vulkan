// defines the glsl version to be used
#version 410 core

// defines the per vertex attributes
layout (location = 0) in vec3 vertex_position;

// defines the per geometry attributes
uniform mat4 model_view_proj_mat;

void main( )
{
   // project the vertex position
   gl_Position = model_view_proj_mat * vec4(vertex_position, 1.0f);
}
