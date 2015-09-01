// defines the glsl version to be used
#version 440 core

// defines the per vertex attributes
layout (location = 0) in vec3 vertex_position;
layout (location = 2) in vec3 vertex_normal;

// defines the per vertex attribute to the geometry shader
out vec3 vertex_normals;

void main( )
{
   // copy the normal into the vertex normals
   vertex_normals = vertex_normal;

   // allow the position to just pass through
   gl_Position = vec4(vertex_position, 1.0f);
}
