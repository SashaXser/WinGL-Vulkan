// defines the glsl version to be used
#version 400 compatibility

// defines the texture to use for rendering
uniform sampler2D day_planet_texture;
uniform sampler2D night_planet_texture;
uniform sampler2D clouds_planet_texture;

// indicates the amount of offset in the clouds to apply in s
uniform float clouds_offset_s;

// indicates the light and normal direction
flat in vec3 light_direction_eye_space;
varying vec3 planet_normal_eye_space;

void main( )
{
   // defines the amient intensity
   const float AMBIENT_INTENSITY = 0.05f;
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

   // average the night colors
   vec3 night_color_average = (night_color.rgb + night_color_1.rgb + night_color_2.rgb + night_color_3.rgb + night_color_4.rgb) / 5.0f;

   // determine the intensity of the night color
   float night_color_intensity = 0.0f;

   if (all(greaterThanEqual(night_color_average, vec3(0.2f, 0.2f, 0.2f))))
   {
      night_color_intensity = 0.5f;
   }

   // determines the final output
   vec4 frag_color = vec4(0.0f, 0.0f, 0.0f, 0.0f);

   if (percent_light >= DUSK_INTENSITY)
   {
      // use the full color of the day plus the light itensity
      frag_color = day_color;
   }
   else if(percent_light < DUSK_INTENSITY && percent_light >= 0.0f)
   {
      // if the values go below the ambient, we clamp back to the ambient
      float percent_light_ambient = clamp(percent_light, AMBIENT_INTENSITY, 1.0f);

      // if there happens to be some lighting from the night lights
      night_color_intensity = max(AMBIENT_INTENSITY, night_color_intensity);

      if (night_color_intensity > percent_light_ambient)
      {
         percent_light_ambient = mix(night_color_intensity, percent_light_ambient, percent_light / DUSK_INTENSITY);
      }

      // entering the phase of dusk / dawn and night / morning
      frag_color = mix(night_color, day_color, percent_light / DUSK_INTENSITY);

      // use the calculated ambient intensity for the light's inensity
      percent_light = percent_light_ambient;
   }
   else
   {
      // there will always be some ambient light
      frag_color = night_color;

      // use the calculated ambient intensity for the light's inensity
      percent_light = max(AMBIENT_INTENSITY, night_color_intensity);
   }

   // obtain the color of the clouds
   // texture coordinates mapped to the sphere are from 1.0f ---> 0.0f
   vec4 cloud_color = texture(clouds_planet_texture, vec2(gl_TexCoord[0].s - clouds_offset_s, gl_TexCoord[0].t));

   // look at only the red color component to determine mixing values
   gl_FragColor = mix(frag_color, vec4(1.0f, 1.0f, 1.0f, 1.0f), cloud_color.r) * percent_light;
}
