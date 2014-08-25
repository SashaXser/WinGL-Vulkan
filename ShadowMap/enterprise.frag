#version 400

uniform bool light_per_pixel;
uniform vec3 light_dir;
uniform mat4 model_view_normal;
uniform mat4 model_view;

smooth in vec3 frag_color;
smooth in vec3 frag_normal;

layout (location = 0) out vec4 frag_color_dest_0;

void main( )
{
   if (light_per_pixel)
   {
      vec4 frag_normal_eye = model_view_normal * vec4(frag_normal, 0.0f);
      vec4 light_dir_eye = model_view * vec4(-1.0f * light_dir, 0.0f);
      float diffuse_intensity = max(dot(normalize(frag_normal_eye.xyz), normalize(light_dir_eye.xyz)), 0.0f);
      float ambient_intensity = 0.05f;
   
      frag_color_dest_0 = vec4(frag_color * diffuse_intensity + ambient_intensity, 1.0f);
   }
   else
   {
      frag_color_dest_0 = vec4(frag_color, 1.0f);
   }
}
