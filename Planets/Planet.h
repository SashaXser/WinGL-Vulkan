#ifndef _PLANET_H_
#define _PLANET_H_

// wingl includes
#include "GeomHelper.h"

// std includes
#include <vector>
#include <cstdint>

class Planet
{
public:
   // constructor / destructor
   Planet( const char * const pSurfaceImg,
           const float radius,
           const double slices_deg = 5.0,
           const double stacks_deg = 2.5 );
   ~Planet( );

   // renders the planet
   void Render( );

   // updates the planet
   void Update( const double & rElapsedTime );

   // obtains the radius of the planet
   float Radius( ) { return mRadius; }

protected:
   // protected typedefs
   typedef std::vector< float >        FloatVec;
   typedef std::vector< unsigned int > UIntVec;
   typedef std::vector< UIntVec >      IndicesVec;

   // loads the image specified
   bool LoadSurfaceImage( const char * pSurfaceImg );

   // constructs the vertices and the image coordinates
   void ConstructPlanet( const double slice_deg, const double stack_deg );

   // protected member variables
   float       mRadius;
   uint32_t    mSurfaceImage;
   uint32_t    mDisplayList;

   GeomHelper::Shape mSphereShape;

private:
   // prohibit default and copy constructor
   Planet( );
   Planet( const Planet & rPlanet );
   // prohibit copy operator
   Planet & operator = ( const Planet & rPlanet );

};

#endif // _PLANET_H_