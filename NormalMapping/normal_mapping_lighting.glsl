// defines the glsl version to be used
#version 400

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
vec4 CalculateDiffuseLighting( const in vec3 light_direction_eye_space,
                               const in vec3 surface_normal_eye_space,
                               const in vec3 light_color,
                               const in float diffuse_intensity )
{
   // the diffuse color computed
   vec4 diffuse_color = vec4(0.0f, 0.0f, 0.0f, 0.0f);

   // determine how much of the diffuse component to use
   // determine the angle between the light a the surface normal, making sure to reverse the direction of the light
   float diffuse_component_factor = dot(surface_normal_eye_space, -1.0f * light_direction_eye_space);

   // we only need acute angles, as those are the ones that produce light and facing the eye
   if (diffuse_component_factor > 0.0f)
   {
      diffuse_color = vec4(light_color, 1.0f) * diffuse_intensity * diffuse_component_factor;
   }

   return diffuse_color;
}

// function that combines the ambient and diffuse colors
vec4 CalculateLighting( const in vec3 light_direction_eye_space,
                        const in vec3 surface_normal_eye_space,
                        const in vec3 light_color,
                        const in float ambient_intensity,
                        const in float diffuse_intensity )
{
   // calculate the ambient color based on color of the light and the intensity [0.0f, 1.0f]
   vec4 ambient_color = vec4(light_color, 1.0f) * ambient_intensity;

   // determine how much of the diffuse component is used
   vec4 diffuse_color = CalculateDiffuseLighting(light_direction_eye_space,
                                                 surface_normal_eye_space,
                                                 light_color,
                                                 diffuse_intensity);

   return ambient_color + diffuse_color;
}

// function that computes the color for a directional light
vec4 CalculateDirectionalLighting( const in vec3 light_direction_eye_space,
                                   const in vec3 surface_normal_eye_space,
                                   const in vec3 light_color,
                                   const in float ambient_intensity,
                                   const in float diffuse_intensity )
{
   return CalculateLighting(light_direction_eye_space,
                            surface_normal_eye_space,
                            light_color,
                            ambient_intensity,
                            diffuse_intensity);
}

// function that computes the color for a point light
vec4 CalculatePointLighting( const in vec3 light_position_eye_space,
                             const in vec3 vertex_position_eye_space,
                             const in vec3 surface_normal_eye_space,
                             const in vec3 light_color,
                             const in float ambient_intensity,
                             const in float diffuse_intensity,
                             const in float attenuation_constant_component,
                             const in float attenuation_linear_component,
                             const in float attenuation_exponential_component )
{
   // determine the light direction to the vertex position
   vec3 light_direction_eye_space = vertex_position_eye_space - light_position_eye_space;

   // determine the distance between the eye and vertex
   float light_direction_distance = length(light_direction_eye_space);

   // calculate the color of the point light
   vec4 color = CalculateLighting(normalize(light_direction_eye_space),
                                  surface_normal_eye_space,
                                  light_color,
                                  ambient_intensity,
                                  diffuse_intensity);

   // determine the attenuation factor
   float attenuation_factor = attenuation_constant_component +
                              attenuation_linear_component * light_direction_distance +
                              attenuation_exponential_component * light_direction_distance * light_direction_distance;

   return color / attenuation_factor;
}
