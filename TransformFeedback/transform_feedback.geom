// defines the glsl version to be used
#version 400 core

// define the input and output topology
// the input topology must match the output topology
// for transform feedback operations to work correctly
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

// defines the per geometry attributes
uniform mat4 model_view;
uniform mat4 model_view_proj_mat;

void main( )
{
   // calculate the direction of the normals
   vec3 vertex_normal = normalize(cross(gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz,
                                        gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz));

   // calculate the barycenter of the triangle to place the normal on
   vec3 triangle_center = (gl_in[0].gl_Position.xyz +
                           gl_in[1].gl_Position.xyz +
                           gl_in[2].gl_Position.xyz) / 3.0f;

   // the first point will start at the barycenter
   gl_Position = vec4(triangle_center, 1.0f);
   EmitVertex();

   // the second point will extrude from the barycenter along the normal
   gl_Position = vec4(triangle_center + vertex_normal * 0.25f, 1.0f);
   EmitVertex();

   EmitVertex();

   // no longer emitting vertices
   EndPrimitive();
}
