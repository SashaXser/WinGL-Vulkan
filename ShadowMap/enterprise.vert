// defines the glsl version to be used
#version 410 core

// defines the common lighting attributes
struct lighting_base
{
   vec3     color;
   float    ambient_intensity;
   float    diffuse_intensity;
};

// defines the directional lighting attributes
struct lighting_directional
{
   lighting_base  base;
   vec3           direction_world_space;
};

// defines attenuation values for point lights
struct point_attenuation
{
   float    constant_component;
   float    linear_component;
   float    exponential_component;
};

// defines the point lighting attributes
struct lighting_point
{
   lighting_base        base;
   vec3                 position_world_space;
   point_attenuation    attenuation;
};


// defines the per vertex attributes
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_color;
layout (location = 2) in vec3 vertex_normal;
layout (location = 3) in vec2 vertex_tex_coords;
layout (location = 4) in vec3 vertex_tangent;
layout (location = 5) in vec3 vertex_bitangent;

// defines the per geometry attributes
uniform mat4 model_view_proj_mat;
uniform mat4 model_view_mat;
uniform mat4 model_view_tinv_mat;
uniform lighting_directional directional_light;

// defines the attributes passed along through the shader pipeline
smooth out vec2 frag_tex_coords;
smooth out vec3 frag_normal_eye_space;
smooth out mat3 frag_tangent_to_eye_space_mat;
smooth out vec3 frag_vertex_position_eye_space;
flat out vec3 directional_light_eye_space;

void main( )
{
   // just copy over the texture coords to be interp in the frag shader
   frag_tex_coords = vertex_tex_coords;

   // calculate the tangent, bitangent, and normal for the fragments in eye space
   //vec3 eye_normal = normalize((model_view_tinv_mat * vec4(vertex_normal, 0.0f)).xyz);
   //vec3 eye_tangent = normalize((model_view_tinv_mat * vec4(vertex_tangent, 0.0f)).xyz);
   //vec3 eye_bitangent = normalize((model_view_tinv_mat * vec4(vertex_bitangent, 0.0f)).xyz);

   // combine the tangent vectors into a matrix for the fragment shader
   //frag_tangent_to_eye_space_mat = mat3(eye_tangent, eye_bitangent, eye_normal);

   // calculate the eye space fragment normal
   frag_normal_eye_space = normalize((model_view_tinv_mat * vec4(vertex_normal, 0.0f)).xyz);

   // calculate the eye space direction for the light
   directional_light_eye_space = normalize((model_view_tinv_mat * vec4(directional_light.direction_world_space, 0.0f)).xyz);

   // project the vertex position
   gl_Position = model_view_proj_mat * vec4(vertex_position, 1.0f);
}
