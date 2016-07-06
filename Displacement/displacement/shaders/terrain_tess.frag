// defines the glsl version to be used
// leave commented out, as the lighting shader source declares it
#version 440 core

// defines the per geometry attributes
uniform sampler2D grass_texture;
uniform sampler2D dirt_texture;
uniform sampler2D rock_texture;
uniform sampler2D snow_texture;
uniform sampler2D normal_map_texture;
uniform mat4 mv_matrix;
uniform uint calculate_lighting;

// defines the attributes passed along through the shader pipeline
in GEOM_OUT
{
   flat mat3 tbn_matrix;
   smooth vec3 normal;
   smooth float height;
   smooth vec2 tex_coord;
} geom_in;

// defines the location of where the color should go
layout (location = 0) out vec4 color0;

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

void main( )
{
   // pull out all the color values now so that filtering works
   vec4 grass = texture(grass_texture, geom_in.tex_coord * 50);
   vec4 dirt = texture(dirt_texture, geom_in.tex_coord * 50);
   vec4 rock = texture(rock_texture, geom_in.tex_coord * 50);
   vec4 snow = texture(snow_texture, geom_in.tex_coord * 50);

   // pull out the normal from the tangent space
   vec3 normal = geom_in.tbn_matrix * texture(normal_map_texture, geom_in.tex_coord).xyz;

   // something more complex can be done later to give
   // the terrain a more natural feel and look...
   vec4 color;

   if (geom_in.height <= 1.0f)
      color = dirt;
   else if (geom_in.height <= 2.0f)
      color = mix(dirt, grass, geom_in.height - 1.0f);
   else if (geom_in.height <= 5.5f)
      color = grass;
   else if (geom_in.height <= 8.5f)
      color = mix(grass, rock, (geom_in.height - 5.5f) / 3.0f);
   else if (geom_in.height <= 18.0f)
      color = rock;
   else if (geom_in.height <= 30.0f)
      color = mix(rock, snow, (geom_in.height - 18.0f) / 12.0f);
   else
      color = snow;

   //const vec3 light_dir = mat3(mv_matrix) * normalize(vec3(-1.0f, -1.0f, 0.0f));
   const vec3 light_dir = mat3(mv_matrix) * normalize(vec3(0.0f, 1.0f, 0.0f));
   
   if (calculate_lighting == 1)
      color0 = color * CalculateDirectionalLighting(light_dir,
                                                    normal,
                                                    vec3(1.0f, 1.0f, 1.0f),
                                                    0.1f, 1.0f);
   else if (calculate_lighting == 2)
      color0 = color * CalculateDirectionalLighting(light_dir,
                                                    geom_in.normal,
                                                    vec3(1.0f, 1.0f, 1.0f),
                                                    0.1f, 1.0f);
   else
      color0 = color;
}
