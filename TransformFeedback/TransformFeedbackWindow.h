#ifndef _TRANSFORM_FEEDBACK_WINDOW_H_
#define _TRANSFORM_FEEDBACK_WINDOW_H_

// wingl includes
#include <Vector.h>
#include <Pipeline.h>
#include <OpenGLWindow.h>
#include <ShaderProgram.h>
#include <FrameBufferObject.h>
#include <VertexArrayObject.h>
#include <VertexBufferObject.h>
#include <TransformFeedbackObject.h>

// std includes
#include <vector>
#include <cstdint>

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
   // shader programs that generates and displays the curve
   ShaderProgram  mGenCurveShader;
   ShaderProgram  mVisCurveShader;

   // the transform feedback buffer holding the generated curve
   // the transform feedback object holding the state for the buffer
   VAO   mTFAGenCurve;
   VBO   mTFBGenCurve;
   TFO   mTFOGenCurve;

   // defines the locations of the control points
   Vec4f * mpActiveControlPoint;
   std::vector< Vec4f > mControlPoints;

   // vertex buffer object holding the actual vertices
   // should be replaced with a uniform buffer object
   VBO   mVBOControlPoints;
   VBO   mVBOControlPointsIndices;

   // the fbo where rendering takes place (color and integer)
   FBO   mFBOCanvas;

   // pipeline object for certain state of the application
   Pipeline mPipeline;

};

#endif // _TRANSFORM_FEEDBACK_WINDOW_H_
