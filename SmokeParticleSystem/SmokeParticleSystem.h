#ifndef _SMOKE_PARTICLE_SYSTEM_H_
#define _SMOKE_PARTICLE_SYSTEM_H_

// local includes
#include "Matrix.h"
#include "ParticleSystem.h"

// stl includes
#include <vector>
#include <cstdint>

class SmokeParticleSystem : public ParticleSystem
{
public:
   // constructor / destructor
            SmokeParticleSystem( );
   virtual ~SmokeParticleSystem( );

   // updates the particles in the system
   virtual void Update( const SimFrame & simFrame );

   // renders the particles in the system
   virtual void Render( const SimFrame & simFrame );

   // called to release any internal attributes.
   // guaranteed to be called within a valid context.
   virtual void Release( );

   // determines if the system is active
   virtual bool IsActive( );

private:
   // private structs
   // defines the particle type
   struct Particle
   {
      float dPosition[3];
      float dVelocity[3];
      uint32_t nTextureUnit;
      float dLifeSpan;
   };

   // private typedefs
   typedef std::vector< Particle > ParticleVec;

   // prohibit copy constructor
            SmokeParticleSystem( const SmokeParticleSystem & );
   // prohibit copy operator
   SmokeParticleSystem & operator = ( const SmokeParticleSystem & );

   // reads an rgb image from disk
   void ReadRGBImage( const char * pFilename, unsigned char ** pImage,
                      unsigned int & nWidth, unsigned int & nHeight );

   // private member variables
   bool        mActive;

   double      mNextParticleDrop;

   Matrixd     mLocalRotMat;
   Matrixd     mLocalTransMat;

   ParticleVec mParticles;

};

#endif // _SMOKE_PARTICLE_SYSTEM_H_