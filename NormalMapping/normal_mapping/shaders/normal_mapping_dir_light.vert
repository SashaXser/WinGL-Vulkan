// defines the glsl version to be used
#version 400

// defines the per vertex attributes
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_color;

// defines the per geometry attributes
uniform mat4 model_view_proj_mat4;

// defines the attributes passed along through the shader pipeline
smooth out vec3 frag_color;

void main( )
{
   // just copy the color for the fragment shader
   frag_color = vertex_color;

   // project the vertex position
   gl_Position = model_view_proj_mat4 * vec4(vertex_position, 1.0f);
}
