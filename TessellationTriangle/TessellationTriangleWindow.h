#ifndef _SHADOW_MAP_WINDOW_H_
#define _SHADOW_MAP_WINDOW_H_

// wingl includes
#include "OpenGLWindow.h"
#include "ShaderProgram.h"
#include "VertexArrayObject.h"
#include "VertexBufferObject.h"

// gl includes
#include "GL/glew.h"
#include <GL/GL.h>

class TessellationTriangleWindow : public OpenGLWindow
{
public:
   // constructor
   // destructor is not public to make sure
   // that the class must be destroyed internally
   // through message WM_NCDESTROY...
   TessellationTriangleWindow( );

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
   virtual ~TessellationTriangleWindow( );

   // handles messages passed by the system
   virtual LRESULT MessageHandler( UINT uMsg,
                                   WPARAM wParam,
                                   LPARAM lParam );

private:
   // initialize routines
   bool InitShaders( );
   bool InitVertices( );

   // controls the amount of rotation on the triangle
   float    mTriYaw;

   // controls the amount of tess points on the edges
   float    mOuterTessDivides[3];

   // controls the amount of tess points inside the triangle
   float    mInnerTessDivides;

   // defines the simple triangle shader
   ShaderProgram        mTriShaderProg;

   // defines the triangle vertices
   VertexArrayObject    mTriVertArray;
   VertexBufferObject   mTriVertBuffer;

};

#endif // _SHADOW_MAP_WINDOW_H_
