#ifndef _NORMAL_MAPPING_WINDOW_H_
#define _NORMAL_MAPPING_WINDOW_H_

// local includes
#include "OpenGLWindow.h"

// wingl includes
#include "Camera.h"
#include "ShaderProgram.h"
#include "VertexArrayObject.h"
#include "VertexBufferObject.h"
#include "CameraPolicies/RoamNoRollRestrictPitch.h"

class AutoScaleWindow : public OpenGLWindow
{
public:
   // constructor
   // destructor is not public to make sure
   // that the class must be destroyed internally
   // through message WM_NCDESTROY...
   AutoScaleWindow( );

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
   virtual ~AutoScaleWindow( );

   // called when the window is about to be destroyed
   virtual void OnDestroy( ) override;

   // handles messages passed by the system
   virtual LRESULT MessageHandler( UINT uMsg,
                                   WPARAM wParam,
                                   LPARAM lParam ) override;

private:
   // intializes all the gl attributes
   void InitGLData( );

   // defines the shape to be auto scaled
   VertexArrayObject    mAutoScaleShape;
   VertexBufferObject   mAutoScaleShapeVerts;

   // defines the shape of the floor
   VertexArrayObject    mFloorShape;
   VertexBufferObject   mFloorVerts;

   // defines a camera to move about the world
   Camera< camera_policy::RoamNoRollRestrictPitch< > >   mCamera;

   // defines a very basic shader
   ShaderProgram        mBasicShader;

};

#endif // _NORMAL_MAPPING_WINDOW_H_
