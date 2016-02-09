#ifndef _SHADOW_MAP_WINDOW_H_
#define _SHADOW_MAP_WINDOW_H_

// wingl includes
#include <Camera.h>
#include <Pipeline.h>
#include <OpenGLWindow.h>
#include <CameraPolicies/RoamNoRollRestrictPitch.h>

// std includes
#include <memory>

// forward declarations
class FrameBufferObject;

class ShadowMapWindow : public OpenGLWindow
{
public:
   // constructor
   // destructor is not public to make sure
   // that the class must be destroyed internally
   // through message WM_NCDESTROY...
   ShadowMapWindow( );

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
   virtual ~ShadowMapWindow( );

   // called when the window is about to be destroyed
   virtual void OnDestroy( ) override;

   // handles messages passed by the system
   virtual LRESULT MessageHandler( UINT uMsg,
                                   WPARAM wParam,
                                   LPARAM lParam );

private:
   // forward declarations
   struct Renderable;

   // renders the scene
   void RenderScene( );

   // generates the scene data
   void GenerateSceneData( );
   void GenerateEnterpriseE( );

   // updates shader camera parameters
   void UpdateShaderCameraValues( );

   // renderable objects
   Renderable *   mpEnterpriseE;

   // defines the camera to use for traversing the space
   Camera< camera_policy::RoamNoRollRestrictPitch< > > mCamera;

   // displays the model's normals
   bool mDisplayNormals;

   // off-screen buffer to render from the lights perspective
   std::unique_ptr< FrameBufferObject > mpShadowMap;

   // graphics pipeline
   Pipeline mPipeline;

};

#endif // _SHADOW_MAP_WINDOW_H_
