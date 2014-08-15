#version 400

uniform mat4 projection;
uniform mat4 model_view;

layout (location = 0) in vec3 vert_position;
layout (location = 1) in vec3 vert_color;
layout (location = 2) in vec3 vert_normal;

out vec3 frag_color;
out vec3 frag_normal;

void main( )
{
   frag_color = vert_color;
   frag_normal = vert_normal;
   gl_Position = projection * model_view * vec4(vert_position, 1.0f);
}
