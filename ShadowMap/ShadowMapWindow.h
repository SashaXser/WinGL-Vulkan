#ifndef _SHADOW_MAP_WINDOW_H_
#define _SHADOW_MAP_WINDOW_H_

// wingl includes
#include "OpenGLWindow.h"

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
      // vao id
      GLuint   mVAO;
      // vbo ids
      GLuint   mVertBufID;
      GLuint   mIdxBufID;
      GLuint   mTexBufID;
      GLuint   mNormBufID;
      // texture id
      GLuint   mTexID;
   };

   // renders the scene
   void RenderScene( );

   // generates the scene data
   void GenerateSceneData( );
   void GenerateFloor( );
   void GenerateCube( );
   void GenerateSphere( );
   void GeneratePyramid( );

   // renderable objects
   Renderable  mFloor;
   Renderable  mCube;
   Renderable  mSphere;
   Renderable  mPyramid;

};

#endif // _SHADOW_MAP_WINDOW_H_
