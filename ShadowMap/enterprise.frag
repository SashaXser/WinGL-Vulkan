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

// defines the per geometry attributes
uniform sampler2D diffuse_texture;
uniform sampler2D shadow_texture;
uniform lighting_directional directional_light;

// defines the attributes passed along through the shader pipeline
smooth in vec2 diffuse_tex_coords;
smooth in vec3 frag_normal_eye_space;
smooth in vec3 frag_depth_light_space;
flat in vec3 directional_light_eye_space;

// defines the location of where the color should go
layout (location = 0) out vec4 frag_color_dest_0;

void main( )
{
   // determine the fragments current depth value
   float shadow_depth_value =
      texture(shadow_texture, frag_depth_light_space.xy).r;

   // determines the amount of colored light to apply
   vec4 total_light_frag_color;

   // determine if the fragment from the light's point of view
   // is farther than what the light (in the shadow map) has recorded
   // the 0.0003f is the bias to get rid of the shadow acne
   if (frag_depth_light_space.z - 0.0003f > shadow_depth_value)
   {
      // this pixel location is in shadow
      total_light_frag_color =
         vec4(
            directional_light.base.color *
            directional_light.base.ambient_intensity,
            1.0f);
   }
   else
   {
      // determine the amount of directional light for this fragment
      total_light_frag_color =
         CalculateDirectionalLighting(
            directional_light_eye_space,
            frag_normal_eye_space,
            directional_light.base.color,
            directional_light.base.ambient_intensity,
            directional_light.base.diffuse_intensity);
   }

   // calculate the final output
   frag_color_dest_0 =
      texture(diffuse_texture, diffuse_tex_coords) *
      total_light_frag_color;
}
