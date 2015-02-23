// local includes
#include "Earth.h"

// wingl includes
#include "ShaderProgram.h"

Earth::Earth( const char * const pSurfaceImg,
              const char * const pNightSurfaceImg,
              const float radius,
              const double slices_deg,
              const double stacks_deg ) :
Planet(pSurfaceImg, radius, slices_deg, stacks_deg),
mNightSurfaceImage   ( LoadSurfaceImage(pNightSurfaceImg) )
{
   // generate the program this time, but with earth's values
   GenerateProgram();
}

Earth::~Earth( )
{
}

void Earth::Render( )
{
   // enable the night surface image
   mNightSurfaceImage.Bind(GL_TEXTURE1);

   // enable textures on this texture unit
   glEnable(GL_TEXTURE_2D);

   // make sure to tell the shader where to get the night texture
   mPlanetPgm.SetUniformValue("night_planet_texture", static_cast< GLint >(mNightSurfaceImage.GetBoundTexUnit()));

   // call the base class to render the planet
   Planet::Render();

   // disable the night surface image
   mNightSurfaceImage.Unbind();

   // disable textures on this texture unit
   glDisable(GL_TEXTURE_2D);

   // restores to the default texture unit
   Texture::SetActiveTextureUnitToDefault();
}

void Earth::Update( const double & elapsed_time )
{
   Planet::Update(elapsed_time);
}

bool Earth::GenerateProgram( )
{
   // release the current program
   mPlanetPgm = ShaderProgram();

   // compile and link the earth shader
   mPlanetPgm.AttachFile(GL_VERTEX_SHADER, "earth.vert");
   mPlanetPgm.AttachFile(GL_FRAGMENT_SHADER, "earth.frag");

   return mPlanetPgm.Link();
}
