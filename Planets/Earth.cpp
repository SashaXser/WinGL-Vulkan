// local includes
#include "Earth.h"

// wingl includes
#include "ShaderProgram.h"

Earth::Earth( const char * const pSurfaceImg,
              const char * const pNightSurfaceImg,
              const float radius,
              const double slices_deg,
              const double stacks_deg ) :
Planet(pSurfaceImg, radius, slices_deg, stacks_deg)
{
   // generate the program this time, but with earth's values
   GenerateProgram();
}

Earth::~Earth( )
{
}

bool Earth::GenerateProgram( )
{
   // destroy the current program
   mPlanetPgm = ShaderProgram();

   return Planet::GenerateProgram();
}
