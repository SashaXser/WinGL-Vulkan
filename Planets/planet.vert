// defines the glsl version to be used
#version 400 compatibility

// indicates the position of the sun and the planet
uniform vec3 sun_position_world_space;
uniform vec3 planet_position_world_space;
uniform mat4 light_world_to_eye_space_mat;

// indicates the light and normal direction
flat out vec3 light_direction_eye_space;
varying vec3 planet_normal_eye_space;

void main( )
{
   // determine the direction from the sun to the planet
   vec3 light_direction_world_space = planet_position_world_space - sun_position_world_space;

   // transform the light direction into eye space
   light_direction_eye_space = normalize((light_world_to_eye_space_mat * vec4(light_direction_world_space, 0.0f)).xyz);

   // transform the planets normal to eye space
   planet_normal_eye_space = normalize(gl_NormalMatrix * gl_Normal);

   // copy the texture coordinates over to the fragment for interpolation
   gl_TexCoord[0] = gl_MultiTexCoord0;

   // transform the vertex position
   gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
}
