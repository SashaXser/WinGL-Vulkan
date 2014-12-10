// defines the glsl version to be used
// leave commented out, as the lighting shader source declares it
//#version 400

// defines the per geometry attributes
uniform sampler2D diffuse_texture;
uniform sampler2D normal_texture;
uniform sampler2D parallax_texture;
uniform lighting_directional directional_light;
uniform lighting_point point_light;
uniform bool invert_normal_texture_y_component;
uniform float parallax_bias;
uniform float parallax_scale;

// defines the attributes passed along through the shader pipeline
smooth in vec2 frag_tex_coords;
smooth in vec3 frag_normal;
smooth in vec3 frag_tangent;
smooth in vec3 frag_bitangent;
smooth in vec3 vertex_position_eye_space;
flat in vec3 directional_light_eye_space;
flat in vec3 point_light_position_eye_space;

// defines the location of where the color should go
layout (location = 0) out vec4 frag_color_dest_0;

void main( )
{
   // calculate the tbn matrix
   mat3 tbn_tangent_to_eye_space = mat3(frag_tangent, frag_bitangent, frag_normal);

   // determine the parallax height
   // parallax texture is a single component texture [0.0f, 1.0f]
   float parallax_sampled_height = texture(parallax_texture, frag_tex_coords).r;

   // scale and bias the parallax height
   float parallax_scaled_and_biased_height = parallax_sampled_height * parallax_scale + parallax_bias;

   // create a unit vector that goes from the vertex point to the eye
   // all of these calculations are being done in eye space, so
   // the vertex position to the eye should be "eye(0, 0, 0) - vertex(x, y, z)"
   // but the offset being applied needs to happen in tangent space to get the correct parallax direction offset
   vec3 parallax_eye_direction = normalize(inverse(tbn_tangent_to_eye_space) * -vertex_position_eye_space);

   // calculate the new texture coords to be used for lookup
   vec2 parallax_frag_tex_coords = frag_tex_coords + parallax_eye_direction.xy * parallax_scaled_and_biased_height;

   // obtain the normal from the texture
   // wonder if a floating point texture would be better here?
   vec3 sampled_normal_tangent_space = texture(normal_texture, parallax_frag_tex_coords).rgb;

   // sometimes the y component is backwards, invert if requested
   if (invert_normal_texture_y_component)
   {
      sampled_normal_tangent_space.g = 1.0f - sampled_normal_tangent_space.g;
   }

   // component of rgb sample is in range of [0.0f, 1.0f], must convert to [-1.0f, 1.0f]
   sampled_normal_tangent_space = sampled_normal_tangent_space * 2.0f - 1.0f;
   
   // convert the tangent space normal to eye space
   vec3 sampled_normal_eye_space = normalize(tbn_tangent_to_eye_space * sampled_normal_tangent_space);

   // determine the amount of directional light for this fragment
   vec4 total_light_frag_color =
      CalculateDirectionalLighting(directional_light_eye_space,
                                   sampled_normal_eye_space,
                                   directional_light.base.color,
                                   directional_light.base.ambient_intensity,
                                   directional_light.base.diffuse_intensity);

   // determine the amount of point light for this fragment
   total_light_frag_color +=
      CalculatePointLighting(point_light_position_eye_space,
                             vertex_position_eye_space,
                             sampled_normal_eye_space,
                             point_light.base.color,
                             point_light.base.ambient_intensity,
                             point_light.base.diffuse_intensity,
                             point_light.attenuation.constant_component,
                             point_light.attenuation.linear_component,
                             point_light.attenuation.exponential_component);

   // calculate the final output
   frag_color_dest_0 = texture(diffuse_texture, parallax_frag_tex_coords) * total_light_frag_color;
}
