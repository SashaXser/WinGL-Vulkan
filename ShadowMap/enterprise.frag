#version 400

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
