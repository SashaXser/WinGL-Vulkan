#ifndef _DISPLACEMENT_WINDOW_H_
#define _DISPLACEMENT_WINDOW_H_

// local includes
#include "OpenGLWindow.h"

// wingl includes
#include "Texture.h"
#include "Pipeline.h"
#include "ShaderProgram.h"
#include "VertexArrayObject.h"
#include "VertexBufferObject.h"

#include "Camera.h"
#include "CameraPolicies/RoamNoRollRestrictPitch.h"

// std includes
#include <cstdint>

class DisplacementWindow : public OpenGLWindow
{
public:
   // constructor
   // destructor is not public to make sure
   // that the class must be destroyed internally
   // through message WM_NCDESTROY...
   DisplacementWindow( );

   // creates the application
   virtual bool Create( unsigned int nWidth,
                        unsigned int nHeight,
                        const char * pWndTitle,
                        const void * pInitParams = NULL );

   // basic run implementation
   // will process messages until app quit
   virtual int Run( );
   
protected:
   // destructor
   virtual ~DisplacementWindow( );

   // called when the window is about to be destroyed
   virtual void OnDestroy( ) override;

   // handles messages passed by the system
   virtual LRESULT MessageHandler( UINT uMsg,
                                   WPARAM wParam,
                                   LPARAM lParam ) override;

private:
   // generates the terrain data
   void GenerateTerrain( const bool reload_shaders );

   // displacment / normal textures for the terrain
   Texture mDispMapTex;
   Texture mNormalMap;

   // textures for the terrain
   Texture mDirtTex;
   Texture mRockTex;
   Texture mSnowTex;
   Texture mGrassTex;

   // renders the basic terrain displaced
   ShaderProgram mTerrainPgm;
   VertexArrayObject mTerrainVAO;
   VertexBufferObject mTerrainVBO;

   // show terrain's wireframe
   bool mWireframe;

   // indicates if light should be applied
   uint8_t mLighting;

   // determines the number of terrain tiles
   uint32_t mNumTiles;

   // number of patches to render
   size_t mNumOfPatches;

   // pipeline object for certain state of the application
   Pipeline mPipeline;

   // camera to control the view
   Camera< camera_policy::RoamNoRollRestrictPitch< > > mCamera;

};

#endif // _DISPLACEMENT_WINDOW_H_
