#version 400

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec2 tex_coords;

uniform mat4 proj_model_view_mat;

smooth out vec2 frag_tex_coords;

void main( )
{
   frag_tex_coords = tex_coords.xy;
   gl_Position = proj_model_view_mat * vec4(vertex_position, 1.0f);
}
