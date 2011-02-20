#ifndef _PARTICLE_SYSTEM_H_
#define _PARTICLE_SYSTEM_H_

// forward declarations
struct SimFrame;

class ParticleSystem
{
public:
   // constructor / destructor
            ParticleSystem( );
   virtual ~ParticleSystem( );

   // updates the particles in the system
   virtual void Update( const SimFrame & simFrame ) = 0;

   // renders the particles in the system
   virtual void Render( const SimFrame & simFrame ) = 0;

   // called to release any internal attributes.
   // guaranteed to be called within a valid context.
   virtual void Release( ) = 0;

   // determines if the system is active
   virtual bool IsActive( ) = 0;

private:
   // prohibit copy constructor
            ParticleSystem( const ParticleSystem & );
   // prohibit copy operation
   ParticleSystem &  operator = ( const ParticleSystem & );

};

#endif // _PARTICLE_SYSTEM_H_