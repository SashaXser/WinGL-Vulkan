#version 400

uniform vec3 light_dir;
uniform mat4 model_view_normal;

in vec3 frag_color;
in vec3 frag_normal;

layout (location = 0) out vec4 frag_color_dest_0;

void main( )
{
   vec3 frag_normal_eye = normalize(model_view_normal * vec4(frag_normal, 1.0f)).xyz;
   vec3 light_dir_eye = normalize(model_view_normal * vec4(light_dir.xyz, 1.0f)).xyz;
   float diffuse_intensity = max(dot(frag_normal_eye, light_dir_eye), 0.0f);
   vec3 global_ambient = vec3(0.1f, 0.1f, 0.1f);

   frag_color_dest_0 = vec4(frag_color * diffuse_intensity + global_ambient, 1.0f);
}
