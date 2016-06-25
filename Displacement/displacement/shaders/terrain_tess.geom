// defines the glsl version to be used
// leave commented out, as the lighting shader source declares it
#version 440 core

// define the input and output topology
// the input topology must match the output topology
// for transform feedback operations to work correctly
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

// defines the per geometry attributes
uniform mat4 mv_matrix;
uniform mat4 mvp_matrix;

// defines the attributes passed along through the shader pipeline
in TE_OUT
{
   smooth float height;
   smooth vec2 tex_coord;
} te_in[];

out GEOM_OUT
{
   flat vec3 normal;
   smooth float height;
   smooth vec2 tex_coord;
} geom_out;

void main( )
{
   // calculate the normal for the triangle
   vec3 normal =
      normalize(mv_matrix * vec4(cross(gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz,
                                       gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz), 0.0)).xyz;
   
   // just pass the data through (making sure to project the position to clip space)
   geom_out.normal = normal;
   geom_out.height = te_in[0].height;
   geom_out.tex_coord = te_in[0].tex_coord;
   gl_Position = mvp_matrix * gl_in[0].gl_Position;
   EmitVertex();

   geom_out.normal = normal;
   geom_out.height = te_in[1].height;
   geom_out.tex_coord = te_in[1].tex_coord;
   gl_Position = mvp_matrix * gl_in[1].gl_Position;
   EmitVertex();

   geom_out.normal = normal;
   geom_out.height = te_in[2].height;
   geom_out.tex_coord = te_in[2].tex_coord;
   gl_Position = mvp_matrix * gl_in[2].gl_Position;
   EmitVertex();
   
   // no longer emitting vertices
   EndPrimitive();
}
