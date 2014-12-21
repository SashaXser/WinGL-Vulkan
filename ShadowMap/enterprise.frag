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

// function that returns the diffuse color
vec4 CalculateDiffuseLighting( const in vec3 light_direction,
                               const in vec3 surface_normal,
                               const in vec3 light_color,
                               const in float diffuse_intensity )
{
   // the diffuse color computed
   vec4 diffuse_color = vec4(0.0f, 0.0f, 0.0f, 0.0f);

   // determine how much of the diffuse component to use
   // determine the angle between the light a the surface normal, making sure to reverse the direction of the light
   float diffuse_component_factor = dot(surface_normal, -1.0f * light_direction);

   // we only need acute angles, as those are the ones that produce light and facing the eye
   if (diffuse_component_factor > 0.0f)
   {
      diffuse_color = vec4(light_color, 1.0f) * diffuse_intensity * diffuse_component_factor;
   }

   return diffuse_color;
}

// function that combines the ambient and diffuse colors
vec4 CalculateLighting( const in vec3 light_direction,
                        const in vec3 surface_normal,
                        const in vec3 light_color,
                        const in float ambient_intensity,
                        const in float diffuse_intensity )
{
   // calculate the ambient color based on color of the light and the intensity [0.0f, 1.0f]
   vec4 ambient_color = vec4(light_color, 1.0f) * ambient_intensity;

   // determine how much of the diffuse component is used
   vec4 diffuse_color = CalculateDiffuseLighting(light_direction,
                                                 surface_normal,
                                                 light_color,
                                                 diffuse_intensity);

   return ambient_color + diffuse_color;
}

// function that computes the color for a directional light
vec4 CalculateDirectionalLighting( const in vec3 light_direction,
                                   const in vec3 surface_normal,
                                   const in vec3 light_color,
                                   const in float ambient_intensity,
                                   const in float diffuse_intensity )
{
   return CalculateLighting(light_direction,
                            surface_normal,
                            light_color,
                            ambient_intensity,
                            diffuse_intensity);
}

// function that computes the color for a point light
vec4 CalculatePointLighting( const in vec3 light_position,
                             const in vec3 vertex_position,
                             const in vec3 surface_normal,
                             const in vec3 light_color,
                             const in float ambient_intensity,
                             const in float diffuse_intensity,
                             const in float attenuation_constant_component,
                             const in float attenuation_linear_component,
                             const in float attenuation_exponential_component )
{
   // determine the light direction to the vertex position
   vec3 light_direction = vertex_position - light_position;

   // determine the distance between the eye and vertex
   float light_direction_distance = length(light_direction);

   // calculate the color of the point light
   vec4 color = CalculateLighting(normalize(light_direction),
                                  surface_normal,
                                  light_color,
                                  ambient_intensity,
                                  diffuse_intensity);

   // determine the attenuation factor
   float attenuation_factor = attenuation_constant_component +
                              attenuation_linear_component * light_direction_distance +
                              attenuation_exponential_component * light_direction_distance * light_direction_distance;

   return color / attenuation_factor;
}


uniform bool light_per_pixel;
uniform vec3 light_dir;
uniform mat4 model_view_normal;
uniform mat4 model_view;
uniform sampler2D tex_unit_0;
uniform bool tex_unit_0_active;

smooth in vec3 frag_color;
smooth in vec3 frag_normal;
smooth in vec2 frag_tex_coords;

layout (location = 0) out vec4 frag_color_dest_0;

void main( )
{
   vec3 input_frag_color = frag_color;

   if (tex_unit_0_active)
   {
      input_frag_color = texture(tex_unit_0, frag_tex_coords).xyz;     
   }
   
   if (light_per_pixel)
   {
      vec4 frag_normal_eye = model_view_normal * vec4(frag_normal, 0.0f);
      vec4 light_dir_eye = model_view * vec4(-1.0f * light_dir, 0.0f);
      float diffuse_intensity = max(dot(normalize(frag_normal_eye.xyz), normalize(light_dir_eye.xyz)), 0.0f);
      float ambient_intensity = 0.05f;
   
      frag_color_dest_0 = vec4(input_frag_color * diffuse_intensity + ambient_intensity, 1.0f);
   }
   else
   {
      frag_color_dest_0 = vec4(input_frag_color, 1.0f);
   }
}
