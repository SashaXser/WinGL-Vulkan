// defines the glsl version to be used
#version 400 compatibility

// defines the texture to use for rendering
uniform sampler2D planet_texture;

// indicates the light and normal direction
flat in vec3 light_direction_eye_space;
varying vec3 planet_normal_eye_space;

void main( )
{
   // determine how much light to show...
   // anything below 0.0f will be automatically clampped by gl
   float percent_light = dot(-light_direction_eye_space, planet_normal_eye_space);

   // there will be some ambient light
   if (percent_light < 0.035f) percent_light = 0.035f;

   // write to the color buffer
   gl_FragColor = texture(planet_texture, gl_TexCoord[0].st) * percent_light;
}
