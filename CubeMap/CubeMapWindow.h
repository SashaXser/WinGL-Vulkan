#ifndef _CUBE_MAP_WINDOW_H_
#define _CUBE_MAP_WINDOW_H_

// local includes
#include "CSphere.h"

// wingl includes
#include "Matrix.h"
#include "OpenGLWindow.h"

// std includes
#include <cstdint>

// gl includes
#include "GL/glew.h"
#include <GL/GL.h>

// forward declarations
class CFont;
class SkyBox;

class CubeMapWindow : public OpenGLWindow
{
public:
   // constructor
   // destructor is not public to make sure
   // that the class must be destroyed internally
   // through message WM_NCDESTROY...
   CubeMapWindow( );

   // creates the application
   virtual bool Create( unsigned int nWidth,
                        unsigned int nHeight,
                        const char * pWndTitle,
                        const void * pInitParams = nullptr );

   // basic run implementation
   // will process messages until app quit
   virtual int Run( );
   
protected:
   // destructor
   virtual ~CubeMapWindow( );

   // called when the window is about to be destroyed
   // the opengl context is no longer valid after this call returns
   virtual void OnDestroy( ) override;

   // handles messages passed by the system
   virtual LRESULT MessageHandler( UINT uMsg,
                                   WPARAM wParam,
                                   LPARAM lParam );

private:
   // private typedefs
   typedef void (CubeMapWindow::*SkySphereRenderOrder)( const double & rElapsedTime,
                                                        CSphere::CubeMapEnumType nFace,
                                                        float fBoxRotation,
                                                        float fOrbitRotation );

   // called to update and render the frame
   void UpdateFrame( const double & elapsed_time );
   void DrawFrame( const double elapsed_time );

   // called to handle mouse / keyboard inputs
   void OnKeyPressed( const char key );
   void OnMouseMove( const int32_t delta_x, const int32_t delta_y, const uint32_t flags );

   // functions that determine the order for rendering
   void DrawSkyboxThenSphere( const double & rElapsedTime,
                              CSphere::CubeMapEnumType nFace,
                              float fBoxRotation, float fOrbitRotation );
   void DrawSphereThenSkybox( const double & rElapsedTime,
                              CSphere::CubeMapEnumType nFace,
                              float fBoxRotation, float fOrbitRotation );

   // init and render the crate
   void InitCrate( );
   void RenderCrate( const float box_rotation, const float box_orbit_rotation );

   // private member variables
   // determines the order for sphere / skybox rendering
   SkySphereRenderOrder mpSkySphereDrawOrder;

   // pointers to the text to render
   CFont *     mpFPS;
   CFont *     mpHelp;
   CFont *     mpInfo;

   // defines the sky box to render
   SkyBox *    mpSkyBox;

   // the rendered sphere in the middle of the scene
   CSphere *   mpSphere;

   // determines the amount of time before updating the dynamic images
   double      mUpdateTimeDelta;
   double      mUpdateTimeout;

   // determines if the text should be displayed
   bool     mDisplayText;

   // crate opengl resources
   GLuint   mCrateTexID;
   GLuint   mCrateDispID;

   // crate rotation values
   float    mBoxRotation;
   float    mBoxOrbitRotation;

   // camera projection / view matrices
   Matrixf  mProjMat;
   Matrixf  mViewMat;

   // mouse coordinates
   uint32_t mMouseX;
   uint32_t mMouseY;

};

#endif // _CUBE_MAP_WINDOW_H_
