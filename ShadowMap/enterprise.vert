#version 400

uniform bool light_per_pixel;
uniform mat4 projection;
uniform mat4 model_view;

uniform vec3 light_dir;
uniform mat4 model_view_normal;

layout (location = 0) in vec3 vert_position;
layout (location = 1) in vec3 vert_color;
layout (location = 2) in vec3 vert_normal;
layout (location = 3) in vec3 tex_coords;

smooth out vec3 frag_color;
smooth out vec3 frag_normal;
smooth out vec2 frag_tex_coords;

void main( )
{
   frag_color = vert_color;
   frag_normal = vert_normal;
   frag_tex_coords = tex_coords.xy;
   gl_Position = projection * model_view * vec4(vert_position, 1.0f);

   if (!light_per_pixel)
   {
      vec4 frag_normal_eye = model_view_normal * vec4(vert_normal, 0.0f);
      vec4 light_dir_eye = model_view * vec4(-1.0f * light_dir, 0.0f);
      float diffuse_intensity = max(dot(normalize(frag_normal_eye.xyz), normalize(light_dir_eye.xyz)), 0.0f);
      float ambient_intensity = 0.05f;

      frag_color = frag_color * diffuse_intensity + ambient_intensity;
   }
}
