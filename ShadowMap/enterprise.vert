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

// defines the per geometry attributes
uniform mat4 model_view_proj_mat;
uniform mat4 model_view_mat;
uniform mat4 model_view_tinv_mat;
uniform mat4 shadow_mvp_mat;
uniform lighting_directional directional_light;

// defines the attributes passed along through the shader pipeline
smooth out vec2 diffuse_tex_coords;
smooth out vec3 frag_normal_eye_space;
smooth out vec3 frag_depth_light_space;
flat out vec3 directional_light_eye_space;

void main( )
{
   // just copy over the texture coords to be interp in the frag shader
   diffuse_tex_coords = vertex_tex_coords;

   // calculate the eye space fragment normal
   frag_normal_eye_space =
      normalize(
         (model_view_tinv_mat *
          vec4(vertex_normal, 0.0f)).xyz);

   // calculate the eye space direction for the light
   directional_light_eye_space =
      normalize(
         (model_view_tinv_mat *
          vec4(directional_light.direction_world_space, 0.0f)).xyz);

   // calculate the project vertex from the light's point
   // of view.  the xy should provide the texture lookup
   // needed to determine what the light saw when rendering
   // the view and the z value should be the current
   // depth value of the current vertex position
   vec4 vpos_light_projected =
      shadow_mvp_mat * vec4(vertex_position, 1.0f);
   vpos_light_projected /= vpos_light_projected.w;
   frag_depth_light_space =
      (mat4(0.5f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.5f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.5f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f) *
       vpos_light_projected).xyz;

   // project the vertex position
   gl_Position =
      model_view_proj_mat *
      vec4(vertex_position, 1.0f);
}
