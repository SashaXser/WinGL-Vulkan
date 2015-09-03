#ifndef _TRANSFORM_FEEDBACK_WINDOW_H_
#define _TRANSFORM_FEEDBACK_WINDOW_H_

// wingl includes
#include <OpenGLWindow.h>
#include <ShaderProgram.h>
#include <VertexArrayObject.h>
#include <VertexBufferObject.h>

// std includes
#include <memory>

class TransformFeedbackWindow : public OpenGLWindow
{
public:
   // constructor
   // destructor is not public to make sure
   // that the class must be destroyed internally
   // through message WM_NCDESTROY...
   TransformFeedbackWindow( );

   // creates the application
   virtual bool Create( unsigned int nWidth,
                        unsigned int nHeight,
                        const char * pWndTitle,
                        const void * pInitParams = NULL ) override;

   // basic run implementation
   // will process messages until app quit
   virtual int Run( ) override;
   
protected:
   // destructor
   virtual ~TransformFeedbackWindow( );

   // called when the window is about to be destroyed
   virtual void OnDestroy( ) override;

   // handles messages passed by the system
   virtual LRESULT MessageHandler( UINT uMsg,
                                   WPARAM wParam,
                                   LPARAM lParam ) override;

private:
   // shader program that defines the triangles
   std::shared_ptr< ShaderProgram > mpTrianglesShader;
   // shader program that renders the vertex normals
   std::shared_ptr< ShaderProgram > mpNormalsShader;

   // holds the state for the triangles
   std::shared_ptr< VertexArrayObject > mpTrianglesVAO;

};

#endif // _TRANSFORM_FEEDBACK_WINDOW_H_
