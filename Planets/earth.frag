// defines the glsl version to be used
#version 400 compatibility

// defines the texture to use for rendering
uniform sampler2D day_planet_texture;
uniform sampler2D night_planet_texture;

// indicates the light and normal direction
flat in vec3 light_direction_eye_space;
varying vec3 planet_normal_eye_space;

void main( )
{
   // defines the amient intensity
   const float AMBIENT_INTENSITY = 0.2f;
   const float DUSK_INTENSITY = 0.3f;

   // determine how much light to show...
   // anything below 0.0f will be automatically clampped by gl
   float percent_light = dot(-light_direction_eye_space, planet_normal_eye_space);

   // obtain the day and night textures
   vec4 day_color = texture(day_planet_texture, gl_TexCoord[0].st);
   vec4 night_color = texture(night_planet_texture, gl_TexCoord[0].st);

   // do a 4 tap on the night color for extra intensity
   vec4 night_color_1 = textureOffset(night_planet_texture, gl_TexCoord[0].st, ivec2(1.0f, 0.0f));
   vec4 night_color_2 = textureOffset(night_planet_texture, gl_TexCoord[0].st, ivec2(-1.0f, 0.0f));
   vec4 night_color_3 = textureOffset(night_planet_texture, gl_TexCoord[0].st, ivec2(0.0f, 1.0f));
   vec4 night_color_4 = textureOffset(night_planet_texture, gl_TexCoord[0].st, ivec2(0.0f, -1.0f));

   // average the night color intensities
   float night_color_intensity = distance((night_color_1.rgb + night_color_2.rgb + night_color_3.rgb + night_color_4.rgb) / 4.0f, vec3(0.0f, 0.0f, 0.0f));

   if (percent_light >= DUSK_INTENSITY)
   {
      // use the full color of the day plus the light itensity
      gl_FragColor = day_color * percent_light;
   }
   else if(percent_light < DUSK_INTENSITY && percent_light >= 0.0f)
   {
      // if the values go below the ambient, we clamp back to the ambient
      float percent_light_ambient = clamp(percent_light, AMBIENT_INTENSITY, 1.0f);

// todo: figure out why line shows up with this blend approach...
//      // if there happens to be some lighting from the night lights
//      night_color_intensity = min(max(AMBIENT_INTENSITY, night_color_intensity), 0.5);
//
//      if (night_color_intensity > AMBIENT_INTENSITY)
//      {
//         percent_light_ambient = mix(night_color_intensity, AMBIENT_INTENSITY, percent_light / DUSK_INTENSITY);
//      }

      // entering the phase of dusk / dawn and night / morning
      gl_FragColor = mix(night_color, day_color, percent_light / DUSK_INTENSITY) * percent_light_ambient;
   }
   else
   {
      // there will always be some ambient light
//      gl_FragColor = night_color * min(max(AMBIENT_INTENSITY, night_color_intensity), 0.5);
      gl_FragColor = night_color * clamp(percent_light, AMBIENT_INTENSITY, 1.0f);
   }
}
