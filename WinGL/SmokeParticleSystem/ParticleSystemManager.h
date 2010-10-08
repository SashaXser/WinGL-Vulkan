#ifndef _PARTICLE_SYSTEM_MANAGER_H_
#define _PARTICLE_SYSTEM_MANAGER_H_

// local includes
#include "Singleton.h"
#include "ReuseAllocator.h"

// stl includes
#include <map>
#include <list>

// forward declarations
class ParticleSystem;
struct SimFrame;

class ParticleSystemManager
{
public:
   // befriend the singleton template class
   template < typename T > friend class Singleton;

   // adds a particle system to the manager
   void  AddParticleSystem( ParticleSystem * pSystem );

   // updates all the particle systems
   void  UpdateParticleSystems( const SimFrame & simFrame );

   // renders all the particle systems
   void  RenderParticleSystems( const SimFrame & simFrame );

private:
   // constructor / destructor
   // prohibit default construction / destruction
            ParticleSystemManager( );
           ~ParticleSystemManager( );

   // prohibit copy constructor
            ParticleSystemManager( const ParticleSystemManager & );
   // prohibit copy operator
   ParticleSystemManager & operator = ( const ParticleSystemManager & );

   // releases all the particle systems.
   // must have a valid gl context active.
   void  ReleaseAllParticleSystems( );

   // private typedefs
   struct InactiveNode
   {
      const double      mSysTimeMS;
      ParticleSystem *  mpPartSys;
   };

   // private typedefs
   typedef std::list< ParticleSystem *, ReuseAllocator< ParticleSystem * > >
           ActivePartSysList;
   typedef std::list< InactiveNode, ReuseAllocator< InactiveNode > >
           InactivePartSysList;

   // private member data
   ActivePartSysList       mActivePartSys;
   InactivePartSysList     mInactivePartSys;

   // const private member data
   const double         FREE_INACTIVE_SYS_TIMEOUT_MS;

};

inline void ParticleSystemManager::AddParticleSystem( ParticleSystem * pSystem )
{
   mActivePartSys.push_back(pSystem);
}

// typedef the singleton for the particle system
typedef Singleton< ParticleSystemManager > ParticleSystemManagerSingleton;

#endif // _PARTICLE_SYSTEM_MANAGER_H_