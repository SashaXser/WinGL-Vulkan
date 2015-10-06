// local includes
#include "Earth.h"

// wingl includes
#include "MathHelper.h"
#include "ShaderProgram.h"

// std includes
#include <cstdint>

Earth::Earth( const char * const pSurfaceImg,
              const char * const pNightSurfaceImg,
              const char * const pCloudsImg,
              const float radius,
              const double planet_tilts[2],
              const double planet_times[2],
              const double planet_major_minor_axes[3],
              const char * const pVertShader,
              const char * const pFragShader,
              const double slices_deg,
              const double stacks_deg ) :
Planet(pSurfaceImg, radius, planet_tilts, planet_times, planet_major_minor_axes, pVertShader, pFragShader, slices_deg, stacks_deg),
mCloudOffsetS        ( 0.0 ),
mNightSurfaceImage   ( LoadSurfaceImage(pNightSurfaceImg) ),
mCloudsImage         ( LoadCloudsImage(pCloudsImg) )
{
}

Earth::~Earth( )
{
}

void Earth::Render( )
{
   // enable the program
   mPlanetPgm.Enable();

   // enable the night surface image
   // enable textures on the night texture unit
   mNightSurfaceImage.Bind(GL_TEXTURE1);
   glEnable(GL_TEXTURE_2D);

   // make sure to tell the shader where to get the night texture
   mPlanetPgm.SetUniformValue("night_planet_texture", static_cast< GLint >(mNightSurfaceImage.GetBoundSamplerID()));

   // enable the clouds image
   // enable textures on the clouds texture unit
   mCloudsImage.Bind(GL_TEXTURE2);
   glEnable(GL_TEXTURE_2D);

   // make sure to tell the shader where to get the clouds texture
   mPlanetPgm.SetUniformValue("clouds_planet_texture", static_cast< GLint >(mCloudsImage.GetBoundSamplerID()));

   // update the clouds offset value
   mPlanetPgm.SetUniformValue("clouds_offset_s", static_cast< float >(mCloudOffsetS));

   // call the base class to render the planet
   Planet::Render();

   // disable the cloud image
   // disable textures on the clouds texture unit
   mCloudsImage.Unbind();
   glDisable(GL_TEXTURE_2D);

   // disable the night surface image
   // disable textures on the night texture unit
   mNightSurfaceImage.Unbind();
   glDisable(GL_TEXTURE_2D);

   // restores to the default texture unit
   Texture::SetActiveTextureUnitToDefault();

   // disable the program
   mPlanetPgm.Disable();
}

void Earth::Update( const double & true_elapsed_time_secs,
                    const double & sim_elapsed_time_secs )
{
   // update the amount of offset applied to the clouds texture
   // since the texture is attached to the sphere and the sphere is spinning,
   // any extra offset applied to the clouds will only increase the speed at
   // which the texture appears to be moving.  without the scaled value below,
   // the texture would make a full revolution in half the time than the planet
   // earth.
   mCloudOffsetS += (PLANETARY_TIME[0] * sim_elapsed_time_secs * 0.25) / math::pi_2< double >();

   // need to renormalize the offset value if it gets too large
   static const double MAX_S_OFFSET = 10.0;

   if (mCloudOffsetS >= MAX_S_OFFSET)
   {
      mCloudOffsetS = mCloudOffsetS - static_cast< uint32_t >(mCloudOffsetS);
   }

   // allow the base to do any updating
   Planet::Update(true_elapsed_time_secs, sim_elapsed_time_secs);
}

Texture Earth::LoadCloudsImage( const char * const pCloudsImg )
{
   Texture texture;

   // load the specified image
   if (texture.Load2D(pCloudsImg, GL_RGB, GL_COMPRESSED_RED, true))
   {
      // bind the texture
      texture.Bind();

      // set extra texture attributes
      texture.SetParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
      texture.SetParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

      // no longer modifying this texture
      texture.Unbind();
   }

   return texture;
}
