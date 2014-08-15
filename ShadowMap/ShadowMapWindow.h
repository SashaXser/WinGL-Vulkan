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

   // handles messages passed by the system
   virtual LRESULT MessageHandler( UINT uMsg,
                                   WPARAM wParam,
                                   LPARAM lParam );

private:
   // defines a basic renderable object
   struct Renderable
   {
      Renderable( ) :
      mIdxBufID   ( 0 ),
      mTexBufID   ( 0 ),
      mTexID      ( 0 )
      { }

      // vao id
      VAO            mVAO;
      // vbo ids
      VBO            mVertBuf;
      GLuint         mIdxBufID;
      GLuint         mTexBufID;
      VBO            mNormBuf;
      VBO            mClrBuf;
      // texture id
      GLuint         mTexID;
      // shader program
      ShaderProgram  mProgram;
   };

   // renders the scene
   void RenderScene( );

   // generates the scene data
   void GenerateSceneData( );
   void GenerateEnterpriseE( );

   // renderable objects
   Renderable  mEnterpriseE;

};

#endif // _SHADOW_MAP_WINDOW_H_
