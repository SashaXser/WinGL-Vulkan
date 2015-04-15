#ifndef _NORMAL_MAPPING_WINDOW_H_
#define _NORMAL_MAPPING_WINDOW_H_

// local includes
#include "OpenGLWindow.h"

// wingl includes
#include "ShaderProgram.h"
#include "VertexArrayObject.h"
#include "VertexBufferObject.h"

class DOFTransformWindow : public OpenGLWindow
{
public:
   // constructor
   // destructor is not public to make sure
   // that the class must be destroyed internally
   // through message WM_NCDESTROY...
   DOFTransformWindow( );

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
   virtual ~DOFTransformWindow( );

   // called when the window is about to be destroyed
   virtual void OnDestroy( ) override;

   // handles messages passed by the system
   virtual LRESULT MessageHandler( UINT uMsg,
                                   WPARAM wParam,
                                   LPARAM lParam ) override;

private:
   // intializes all the gl attributes
   void InitGLData( );

   // defines the shape of the body
   VertexArrayObject    mBodyShape;
   VertexBufferObject   mBodyShapeVerts;

   // defines the shape of the turret
   VertexArrayObject    mTurretShape;
   VertexBufferObject   mTurretShapeVerts;

   // defines a very basic shader
   ShaderProgram        mBasicShader;

};

#endif // _NORMAL_MAPPING_WINDOW_H_
