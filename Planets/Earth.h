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
          const float radius,
          const double slices_deg = 5.0,
          const double stacks_deg = 2.5 );
   virtual ~Earth( );

   // renders the planet
   virtual void Render( ) override;

   // updates the planet
   virtual void Update( const double & elapsed_time ) override;

protected:
   // constructs the shader program
   virtual bool GenerateProgram( ) override;

private:
   // prohibit certain actions
   Earth( );
   Earth( const Earth &);
   // prohibit certain operations
   Earth & operator = ( const Earth & );

   // night time surface image
   Texture mNightSurfaceImage;

};

#endif // _EARTH_H_
