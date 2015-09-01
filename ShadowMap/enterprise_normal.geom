// defines the glsl version to be used
#version 440 core

// define the input and output topology
layout (triangles) in;
layout (line_strip, max_vertices = 2) out;

// defines the per vertex attributes
in vec3 vertex_normals[];

// defines the per geometry attributes
uniform mat4 model_view_proj_mat;

void main( )
{
   // calculate the direction of the normals
   vec3 vertex_normal = normalize((vertex_normals[0] + vertex_normals[1] + vertex_normals[2]) / 3.0f);

   // calculate the barycenter of the triangle to place the normal on
   vec3 triangle_center = (gl_in[0].gl_Position.xyz + gl_in[1].gl_Position.xyz + gl_in[2].gl_Position.xyz) / 3.0f;

   // the first point will start at the barycenter
   gl_Position = model_view_proj_mat * vec4(triangle_center, 1.0f);
   EmitVertex();

   // the second point will extrude from the barycenter along the normal
   gl_Position = model_view_proj_mat * vec4(triangle_center + vertex_normal * 0.25f, 1.0f);
   EmitVertex();

   // no longer emitting vertices
   EndPrimitive();
}
