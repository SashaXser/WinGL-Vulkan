#ifndef _PROJECTIVE_TEXTURE_WINDOW_H_
#define _PROJECTIVE_TEXTURE_WINDOW_H_

// wingl includes
#include "Matrix.h"
#include "Texture.h"
#include "OpenGLWindow.h"
#include "ShaderProgram.h"

#include "Camera.h"
#include "CameraPolicies/RoamNoRollRestrictPitch.h"

class ProjectiveTextureWindow : public OpenGLWindow
{
public:
   // constructor
   // destructor is not public to make sure
   // that the class must be destroyed internally
   // through message WM_NCDESTROY...
            ProjectiveTextureWindow( );

   // creates the application
   virtual bool Create( unsigned int nWidth,
                        unsigned int nHeight,
                        const char * pWndTitle,
                        const void * pInitParams = NULL );

   // basic run implementation
   // will process messages until app quit
   virtual int Run( );

protected:
   // destructor...
   virtual ~ProjectiveTextureWindow( );

   // handles messages passed by the system
   virtual LRESULT MessageHandler( UINT uMsg,
                                   WPARAM wParam,
                                   LPARAM lParam );

private:
   // private structs
   struct CameraVariables
   {
      Matrixd     mProjMat;
      Matrixd     mMViewMat;
   };

   // private typedefs
   typedef void (ProjectiveTextureWindow::*RenderModeFuncPtr)( void );
   typedef void (ProjectiveTextureWindow::*SetupModeFuncPtr)( void );

   // prohibit copy constructor
            ProjectiveTextureWindow( const ProjectiveTextureWindow & );
   // prohibit copy operator
   ProjectiveTextureWindow &  operator = ( const ProjectiveTextureWindow & );

   // init common gl attributes
   void InitGLState( int vpWidth, int vpHeight );

   // renders the scene
   void RenderScene( );
   
   // render the scene using immediate mode calls
   void RenderSceneImmediateMode( );
   void RenderWallsImmediateMode( );
   void RenderSpotLightImmediateMode( );

   // render the scene using the shader
   void RenderSceneWithShader( );

   // setup the scene for the specified mode
   void SetupRenderSceneImmediateModeEyeSpace( );
   void SetupRenderSceneImmediateModeObjectSpace( );

   // updates an immediate mode light model
   void UpdateImmediateModeLightModel( );

   // loads a texture
   void LoadTexture( );

   // private member variables
   Texture  mLogoTex;
   Texture  mDepthTex;

   // indicates the depth bias of the shadow map
   float    mDepthBias;

   // indicates if the shadow map should be rendered
   bool     mRenderShadowMap;

   // todo: turn framebuffer into an object
   GLuint   mDepthFrameBuffer;

   Matrixd *      mpActiveMViewMat;

   CameraVariables      mLightVariables;
   CameraVariables      mCameraVariables;

   SetupModeFuncPtr     mpSetupModeFuncPtr;
   RenderModeFuncPtr    mpRenderModeFuncPtr;

   ShaderProgram        mProjTexProg;
   ShaderProgram        mVizDepthTex;

};

#endif // _PROJECTIVE_TEXTURE_WINDOW_H_