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
   flat mat3 tbn_matrix;
   flat vec3 normal;
   smooth float height;
   smooth vec2 tex_coord;
} geom_out;

void main( )
{
   // obtain the edges of the triangle
   vec3 e1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
   vec3 e2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;

   // obtain the edges of the texture coordinates
   vec2 uv1 = te_in[1].tex_coord - te_in[0].tex_coord;
   vec2 uv2 = te_in[2].tex_coord - te_in[0].tex_coord;

   // calculate the normal for the triangle
   // this will be used to transform the normal map normal to model space
   vec3 normal = normalize((mv_matrix * vec4(cross(e1, e2), 0.0)).xyz);

   // calculate the tangent vector, which should be a vector in the same direction as the texture coordinates
   const float dividend = uv1.s * uv2.t - uv2.s * uv1.t;
   const float det = dividend == 0.0f ? 1.0f : 1.0f / dividend;
   vec3 tangent = normalize((mv_matrix * vec4(det * (uv2.t * e1.x - uv1.t * e2.x),
                                              det * (uv2.t * e1.y - uv1.t * e2.y),
                                              det * (uv2.t * e1.z - uv1.t * e2.z),
                                              0.0f)).xyz);

   // calculate the bitange as just the cross of the normal and tangent
   vec3 bitangent = normalize(cross(normal, tangent));
   //vec3 bitangent = normalize((mv_matrix * vec4(det * (-uv2.s * e1.x - uv1.s * e2.x),
   //                                             det * (-uv2.s * e1.y - uv1.s * e2.y),
   //                                             det * (-uv2.s * e1.z - uv1.s * e2.z),
   //                                             0.0f)).xyz);
   
   // just pass the data through (making sure to project the position to clip space)
   geom_out.tbn_matrix = mat3(tangent, bitangent, normal);
   geom_out.normal = normal;
   geom_out.height = te_in[0].height;
   geom_out.tex_coord = te_in[0].tex_coord;
   gl_Position = mvp_matrix * gl_in[0].gl_Position;
   EmitVertex();

   geom_out.tbn_matrix = mat3(tangent, bitangent, normal);
   geom_out.normal = normal;
   geom_out.height = te_in[1].height;
   geom_out.tex_coord = te_in[1].tex_coord;
   gl_Position = mvp_matrix * gl_in[1].gl_Position;
   EmitVertex();

   geom_out.tbn_matrix = mat3(tangent, bitangent, normal);
   geom_out.normal = normal;
   geom_out.height = te_in[2].height;
   geom_out.tex_coord = te_in[2].tex_coord;
   gl_Position = mvp_matrix * gl_in[2].gl_Position;
   EmitVertex();
   
   // no longer emitting vertices
   EndPrimitive();
}
