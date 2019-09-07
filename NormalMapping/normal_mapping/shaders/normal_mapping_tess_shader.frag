// defines the glsl version to be used
// leave commented out, as the lighting shader source declares it
//#version 410 core

// defines the per geometry attributes
uniform sampler2D diffuse_texture;
uniform lighting_directional directional_light;
uniform lighting_point point_light;

// defines the attributes passed along through the shader pipeline
in TEvalData
{
   smooth vec2 frag_tex_coords;
   smooth vec3 frag_normal;
   smooth vec3 vertex_position_eye_space;
   flat vec3 directional_light_eye_space;
   flat vec3 point_light_position_eye_space;
} teval_in;

// defines the location of where the color should go
layout (location = 0) out vec4 frag_color_dest_0;

void main( )
{
   // determine the amount of directional light for this fragment
   vec4 total_light_frag_color =
      CalculateDirectionalLighting(normalize(teval_in.directional_light_eye_space),
                                   normalize(teval_in.frag_normal),
                                   directional_light.base.color,
                                   directional_light.base.ambient_intensity,
                                   directional_light.base.diffuse_intensity);

   // determine the amount of point light for this fragment
   total_light_frag_color +=
      CalculatePointLighting(teval_in.point_light_position_eye_space,
                             teval_in.vertex_position_eye_space,
                             normalize(teval_in.frag_normal),
                             point_light.base.color,
                             point_light.base.ambient_intensity,
                             point_light.base.diffuse_intensity,
                             point_light.attenuation.constant_component,
                             point_light.attenuation.linear_component,
                             point_light.attenuation.exponential_component);

   // calculate the final output
   frag_color_dest_0 =
      texture(diffuse_texture, teval_in.frag_tex_coords) *
      total_light_frag_color;
}
