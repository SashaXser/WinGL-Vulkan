#ifndef _PLANET_H_
#define _PLANET_H_

// wingl includes
#include "Matrix.h"
#include "Vector.h"
#include "Texture.h"
#include "GeomHelper.h"
#include "ShaderProgram.h"
#include "VertexBufferObject.h"

// std includes
#include <array>
#include <vector>
#include <cstdint>

class Planet
{
public:
   // constructor / destructor
   Planet( const char * const pSurfaceImg,
           const float radius,
           const double planet_tilts[2],
           const double planet_times[2],
           const double planet_major_minor_axes[3],
           const char * const pVertShader = "planet.vert",
           const char * const pFragShader = "planet.frag",
           const double slices_deg = 5.0,
           const double stacks_deg = 2.5 );
   virtual ~Planet( );

   // renders the planet
   virtual void Render( );

   // updates the planet
   virtual void Update( const double & true_elapsed_time_secs,
                        const double & sim_elapsed_time_secs );

   // obtains the radius of the planet
   float Radius( ) { return mRadius; }

   // sets the world to eye space lighting matrix
   void UpdateWorldToEyeSpaceLighting( const Matrixf & world_to_eye_light );

   // sets the suns position in world space
   void UpdateSunWorldPosition( const Vec3f & world_position );

   // gets the current position of the planet
   Vec3f GetWorldPosition( ) const;

   // gets the planet's orbital tilt matrix
   const Matrixf & GetOrbitalTiltMatrix( ) const { return mPlanetaryMatrix[0]; }

protected:
   // protected typedefs
   typedef std::vector< float >        FloatVec;
   typedef std::vector< unsigned int > UIntVec;
   typedef std::vector< UIntVec >      IndicesVec;

   // loads the image specified
   Texture LoadSurfaceImage( const char * const pSurfaceImg );

   // constructs the vertices and the image coordinates
   void ConstructPlanet( const double slice_deg, const double stack_deg );

   // constructs the shader program
   bool GenerateProgram( const char * const pVertShader,
                         const char * const pFragShader );

   // protected member variables
   // radius of the planet
   const float mRadius;

   // surface texture of the planet
   Texture mSurfaceImage;

   // buffers to store the renderable data
   VertexBufferObject mVertexArray;
   VertexBufferObject mTexCoordArray;
   VertexBufferObject mNormalArray;
   VertexBufferObject mIndexArray;

   // program used to manipulate and display the renderable data
   ShaderProgram mPlanetPgm;

   // the sphere shape that represents the planet
   GeomHelper::Shape mSphereShape;

   // defines the position of the planet based on time
   double mPlanetaryTime;

   // defines the matrices for each of the nine planets
   // first value indicates the planets ecliptic
   // second value indicates the planets tilt
   // third value indicates the planets rotation on axis
   // forth value indicates the planets position
   Matrixf mPlanetaryMatrix[4];

   // defines the planetary ecliptics and any axial tilts
   // first value is the ecliptic and the second is the axial
   // all values given in degrees
   const std::array< double, 2 > PLANETARY_TILTS;

   // defines all the planets time values
   // first values indicates the revolution for the planet
   // second values indicates the revolutions around the sun
   const std::array< double, 2 > PLANETARY_TIME;

   // defines all the major and minor axes for the planets
   const std::array< double, 3 > MAJ_MIN_AXES;

private:
   // prohibit default and copy constructor
   Planet( );
   Planet( const Planet & rPlanet );
   // prohibit copy operator
   Planet & operator = ( const Planet & rPlanet );

};

#endif // _PLANET_H_
