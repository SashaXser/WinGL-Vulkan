#ifndef _SLIDER_CONTROL_WINDOW_H_
#define _SLIDER_CONTROL_WINDOW_H_

// local includes
#include "OpenGLWindow.h"

// wingl includes
#include "Camera.h"
#include "Texture.h"
#include "Pipeline.h"
#include "ShaderProgram.h"
#include "VertexArrayObject.h"
#include "VertexBufferObject.h"
#include "CameraPolicies/RoamNoRollRestrictPitch.h"

// std includes

class SliderControlWindow : public OpenGLWindow
{
public:
   // constructor
   // destructor is not public to make sure
   // that the class must be destroyed internally
   // through message WM_NCDESTROY...
   SliderControlWindow( );

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
   virtual ~SliderControlWindow( );

   // called when the window is about to be destroyed
   virtual void OnDestroy( ) override;

   // handles messages passed by the system
   virtual LRESULT MessageHandler( UINT uMsg,
                                   WPARAM wParam,
                                   LPARAM lParam ) override;

private:
   // intializes all the gl attributes
   bool InitGLData( );

   // defines the groove members
   ShaderProgram mGrooveShader;
   VertexArrayObject mGrooveVAO;
   
   // defines the handle members
   ShaderProgram mHandleShader;
   VertexArrayObject mHandleVAO;
   VertexBufferObject mHandleVBO;

   // defines the two textures to display
   Texture  mYokeTex;
   Texture  mPedalTex;

   // defines a camera to move about the world
   Camera< camera_policy::RoamNoRollRestrictPitch< > >   mCamera;

   // pipeline object for certain state of the application
   Pipeline mPipeline;

   // current position and rotation of handle
   float mHandlePos;
   float mHandleRot;

   // current time since last update
   int64_t mCurrentTimeTick;

};

#endif // _SLIDER_CONTROL_WINDOW_H_
