#ifndef _EARTH_H_
#define _EARTH_H_

// local includes
#include "Planet.h"
#include "Texture.h"

class Earth : public Planet
{
public:
   // constructor / destructor
   Earth( const char * const pSurfaceImg,
          const char * const pNightSurfaceImg,
          const char * const pCloudsImg,
          const float radius,
          const double planet_tilts[2],
          const double planet_times[2],
          const double planet_major_minor_axes[3],
          const char * const pVertShader = "earth.vert",
          const char * const pFragShader = "earth.frag",
          const double slices_deg = 5.0,
          const double stacks_deg = 2.5 );
   virtual ~Earth( );

   // renders the planet
   virtual void Render( ) override;

   // updates the planet
   virtual void Update( const double & true_elapsed_time_secs,
                        const double & sim_elapsed_time_secs ) override;

protected:
   // loads the clouds image
   Texture LoadCloudsImage( const char * const pCloudsImg );

private:
   // prohibit certain actions
   Earth( );
   Earth( const Earth &);
   // prohibit certain operations
   Earth & operator = ( const Earth & );

   // indicates the offset texture lookup for the clounds
   double mCloudOffsetS;

   // night time surface image
   Texture mNightSurfaceImage;

   // clouds transparency image
   Texture mCloudsImage;

};

#endif // _EARTH_H_
