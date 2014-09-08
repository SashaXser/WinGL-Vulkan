#ifndef _PLANET_H_
#define _PLANET_H_

// qgl includes
#include "AngleType.h"

// forward declarations
class ImageReader;

class Planet
{
public:
   // constructor / destructor
               Planet( const char * pSurfaceImg,
                       const float & rRadius,
                       const Angle & rSlices = Angle(AngleType::DEGREES, 10.0),
                       const Angle & rStacks = Angle(AngleType::DEGREES, 5.0) );
   virtual    ~Planet( );

   // renders the planet
   virtual void   Render( );

   // updates the planet
   virtual void   Update( const double & rElapsedTime );

   // obtains the radius of the planet
   float    Radius( ) { return m_fRadius; }

protected:
   // protected typedefs
   typedef std::vector< float >        FloatVec;
   typedef std::vector< unsigned int > UIntVec;
   typedef std::vector< UIntVec >      IndicesVec;

   // default constructor
               Planet( );

   // loads the image specified
   bool     LoadSurfaceImage( const char * pSurfaceImg );

   // constructs the vertices and the image coordinates
   bool     ConstructPlanet( Angle oSlice, Angle oStack );

   // protected member variables
   float          m_fRadius;
   unsigned int   m_nSurfaceImage;
   unsigned int   m_nDisplayList;
   ImageReader *  m_pImage;

   FloatVec    m_vVertices;
   FloatVec    m_vTexCoords;
   IndicesVec  m_vIndices;

private:
   // prohibit default and copy constructor
               Planet( const Planet & rPlanet );
   // prohibit copy operator
   Planet &    operator = ( const Planet & rPlanet );

};

#endif // _PLANET_H_