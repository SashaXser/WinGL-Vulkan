// defines the glsl version to be used
// leave commented out, as the lighting shader source declares it
//#version 410 core

// defines the per vertex attributes
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec2 vertex_tex_coords;
layout (location = 2) in vec3 vertex_normal;

// defines the per geometry attributes
uniform mat4 model_view_proj_mat4;
uniform mat4 model_view_mat4;
uniform mat4 model_view_tinv_mat4;
uniform lighting_directional directional_light;
uniform lighting_point point_light;

// defines the attributes passed along through the shader pipeline
smooth out vec2 tctrl_tex_coords;
smooth out vec3 frag_normal;
smooth out vec3 vertex_position_eye_space;
flat out vec3 directional_light_eye_space;
flat out vec3 point_light_position_eye_space;

void main( )
{
   // just copy over the texture coords to be interp in the tctrl shader
   tctrl_tex_coords = vertex_tex_coords.st;

   // calculate the normal for the fragments in eye space
   frag_normal = (model_view_tinv_mat4 * vec4(vertex_normal, 0.0f)).xyz;

   // calculate the eye space direction for the light
   directional_light_eye_space = (model_view_tinv_mat4 * vec4(directional_light.direction_world_space, 0.0f)).xyz;

   // calculate the eye space location of the point light
   point_light_position_eye_space = (model_view_mat4 * vec4(point_light.position_world_space, 1.0f)).xyz;

   // calculate the eye space position of the vertex
   vertex_position_eye_space = (model_view_mat4 * vec4(vertex_position, 1.0f)).xyz;

   // project the vertex position
   gl_Position = model_view_proj_mat4 * vec4(vertex_position, 1.0f);
}
