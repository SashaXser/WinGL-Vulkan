#ifndef _INSTANCING_WINDOW_H_
#define _INSTANCING_WINDOW_H_

// local includes
#include "OpenGLWindow.h"

// gl incudes
#include <GL/glew.h>

class InstancingWindow : public OpenGLWindow
{
public:
   // constructor...
   // destructor is not public to make sure
   // that the class must be destroyed internally
   // through message WM_NCDESTROY...
   InstancingWindow( );

   // creates the application
   virtual bool Create( unsigned int nWidth, unsigned int nHeight,
                        const char * pWndTitle, const void ** pInitParams = nullptr );

   // basic run implementation
   // will process messages until app quit
   virtual int Run( );

protected:
   // destructor...
   virtual ~InstancingWindow( );

private:
   // prohibit copy construction
   InstancingWindow( const InstancingWindow & );
   // prohibit copy operator
   InstancingWindow & operator = ( const InstancingWindow & );

   // generates all the required items for rendering
   void CreateInstances( );

   // renders the scene
   void RenderScene( );

   // vao ids
   GLuint   mVertArrayID;

   // gl buffer ids
   GLuint   mVertexBufferID;
   GLuint   mIndexBufferID;

   // shader ids
   GLuint   mVertShaderID;
   GLuint   mFragShaderID;

};

#endif // _INSTANCING_WINDOW_H_
