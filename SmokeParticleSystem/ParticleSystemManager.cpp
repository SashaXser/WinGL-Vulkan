// local includes
#include "ParticleSystemManager.h"

#include "SimFrame.h"
#include "ParticleSystem.h"

ParticleSystemManager::ParticleSystemManager( ) :
FREE_INACTIVE_SYS_TIMEOUT_MS     ( 300000.0 )
{
}

ParticleSystemManager::~ParticleSystemManager( )
{
   ReleaseAllParticleSystems();
}

void ParticleSystemManager::UpdateParticleSystems( const SimFrame & simFrame )
{
   ActivePartSysList::iterator itCur = mActivePartSys.begin();
   ActivePartSysList::iterator itEnd = mActivePartSys.end();

   for (; itCur != itEnd;)
   {
      // obtain the particle system
      ParticleSystem * pPartSys = *itCur;
      // make sure the particle system is active
      if (!pPartSys->IsActive())
      {
         // remove the particle system
         itCur = mActivePartSys.erase(itCur);
         // create an inactive node
         InactiveNode inNode =
         {
            simFrame.dCurTimeMS + FREE_INACTIVE_SYS_TIMEOUT_MS,
            pPartSys
         };
         // add system to inactive list
         mInactivePartSys.push_back(inNode);
         // continue the process
         continue;
      }
      else
      {
         // update the particle system
         pPartSys->Update(simFrame);
      }

      // increase the iterator
      ++itCur;
   }
}

void ParticleSystemManager::RenderParticleSystems( const SimFrame & simFrame )
{
   // render all the active particle systems
   ActivePartSysList::iterator itCur = mActivePartSys.begin();
   ActivePartSysList::iterator itEnd = mActivePartSys.end();

   for (; itCur != itEnd; ++itCur)
   {
      (*itCur)->Render(simFrame);
   }

   // remove inactive systems
   if (!mInactivePartSys.empty())
   {
      // obtain the front system
      InactivePartSysList::iterator itNode = mInactivePartSys.begin();;
      // only remove one particle system per frame
      if (itNode->mSysTimeMS <= simFrame.dCurTimeMS)
      {
         // release the system
         itNode->mpPartSys->Release();
         // delete the system
         delete itNode->mpPartSys;
         // remove the front from the system
         mInactivePartSys.erase(itNode);
      }
   }
}

void ParticleSystemManager::ReleaseAllParticleSystems( )
{
   {
   ActivePartSysList::iterator itCur = mActivePartSys.begin();
   ActivePartSysList::iterator itEnd = mActivePartSys.end();

   for (; itCur != itEnd; ++itCur)
   {
      delete *itCur;
   }

   }

   {
   InactivePartSysList::iterator itCur = mInactivePartSys.begin();
   InactivePartSysList::iterator itEnd = mInactivePartSys.begin();

   for (; itCur != itEnd; ++itCur)
   {
      delete itCur->mpPartSys;
   }

   }
}

namespace CRTInit
{
// allow the crt to initialize the singleton
ParticleSystemManager * pParticleSysMgr = ParticleSystemManagerSingleton::Instance();
}