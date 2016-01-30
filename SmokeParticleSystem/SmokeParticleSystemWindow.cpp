// local includes
#include "SmokeParticleSystemWindow.h"
#include "Matrix.h"
#include "SimFrame.h"
#include "MatrixHelper.h"
#include "OpenGLExtensions.h"
#include "SmokeParticleSystem.h"
#include "ParticleSystemManager.h"

// gl includes
#include "GL/glew.h"
#include <gl/gl.h>
#include <gl/glu.h>

// std includes
#include <algorithm>

// global defines
#ifdef _DEBUG
#define VALIDATE_OPENGL_FRAME( ) \
   if (glGetError() != GL_NO_ERROR) *((int *)0x0) = 0;
#else
#define VALIDATE_OPENGL_FRAME( )
#endif

// library includes
#pragma comment( lib, "Glu32.lib" )

SmokeParticleSystemWindow::SmokeParticleSystemWindow( )
{
}

SmokeParticleSystemWindow::~SmokeParticleSystemWindow( )
{
}

bool SmokeParticleSystemWindow::Create( unsigned int nWidth,
                                        unsigned int nHeight,
                                        const char * pWndTitle,
                                        const void * pInitParams )
{
   // initialize 32 else nothing
   const OpenGLWindow::OpenGLInit glInit[] =
   {
      { 3, 2, true, true, false },
      { 0 }
   };

   // call base class to init
   if (OpenGLWindow::Create(nWidth, nHeight, pWndTitle, glInit))
   {
      // make the context current
      MakeCurrent();

      // setup the initial camera position
      glMatrixMode(GL_MODELVIEW);
      glTranslatef(0.0f, -2.5f, 0.0f);

      // send a message to change the size
      // this will setup the projection matrix
      SendMessage(GetHWND(), WM_SIZE, 0, nHeight << 16 | nWidth);

      /////////////////////////////////
      // temp
      ParticleSystemManagerSingleton::Instance()->AddParticleSystem(new SmokeParticleSystem);
      /////////////////////////////////

      return true;
   }

   return false;
}

int SmokeParticleSystemWindow::Run( )
{
   // vars for the loop
   SimFrame simFrame;
   int appQuitVal = 0;
   bool bQuit = false;

   // init the sim frame frame time
   simFrame.dSimTimeMS = 1000.0 / 30.0;

   // initialize sim frame timer
   GetSimFrameTime(simFrame);

   // basic message pump and render frame
   while (!bQuit)
   {
      // process all the messages
      if (!(bQuit = PeekAppMessages(appQuitVal)))
      {
         // update the scene
         UpdateScene(simFrame);

         // begin rendering the scene
         RenderScene(simFrame);

         // get next sim frame time
         // wait if frame time is not up
         GetSimFrameTime(simFrame);
      }
   }

   return appQuitVal;
}

LRESULT SmokeParticleSystemWindow::MessageHandler( UINT uMsg,
                                                   WPARAM wParam,
                                                   LPARAM lParam )
{
   LRESULT result = 0;

   // defines the current mouse locations
   static unsigned short mouseLocX = 0;
   static unsigned short mouseLocY = 0;

   switch (uMsg)
   {
   case WM_CLOSE:
      // post a quit message
      PostQuitMessage(0);

      break;

   case WM_SIZE:
      // change the size of the viewport
      // this will be ignored until a valid gl context is created
      glViewport(0, 0,
                (GLsizei)(lParam & 0x0000FFFF),
                (GLsizei)(lParam >> 16));

      // setup the projection matrix
      glMatrixMode(GL_PROJECTION);
      // clear the current matrixt
      glLoadIdentity();
      // setup the projection matrix
      gluPerspective(45.0,
                    (double)(lParam & 0x0000FFFF) /
                    (double)(lParam >> 16),
                     0.1, 100.0);
      // switch back to the modelview matrix
      glMatrixMode(GL_MODELVIEW);

      break;

   case WM_KEYUP:
      switch (wParam)
      {
      case VK_ESCAPE:
         // close the application
         PostMessage(GetHWND(), WM_CLOSE, 0, 0);

         break;
      }

      break;

   case WM_MOUSEMOVE:
      {
      // obtain the current x and y locations
      const short curX = (short)(lParam & 0x0000FFFF);
      const short curY = (short)(lParam >> 16);

      if (wParam & MK_LBUTTON)
      {
         // determine x and y deltas
         const short deltaX = mouseLocX - curX;
         const short deltaY = mouseLocY - curY;
         // obtain the modelview matrix
         Matrixd modelViewMat;
         glGetDoublev(GL_MODELVIEW_MATRIX, modelViewMat);
         // decompose matrix for yaw and pitch
         double yaw = 0, pitch = 0;
         MatrixHelper::DecomposeYawPitchRollDeg(modelViewMat,
                                                &yaw, &pitch,
                                               (double *)NULL);
         // add the deltas
         yaw += deltaX;
         pitch += deltaY;
         // cap pitch
         pitch = std::min(89.9, pitch);
         pitch = std::max(-89.9, pitch);
         // get the current camera world position
         modelViewMat.MakeInverseFromOrthogonal();
         Vec3d camEye = modelViewMat * Vec3d(0.0, 0.0, 0.0) * -1.0;
         // clear the modelview matrix
         glLoadIdentity();
         // setup the new matrix
         glRotated(pitch, -1.0, 0.0, 0.0);
         glRotated(yaw, 0.0, -1.0, 0.0);
         glTranslated(camEye.mT[0], camEye.mT[1], camEye.mT[2]); 
      }

      // save the mouse values
      mouseLocX = curX;
      mouseLocY = curY;
      }

      break;

   case WM_KEYDOWN:
      {
      // obtain the current modelview matrix
      Matrixd modelViewMat;
      glGetDoublev(GL_MODELVIEW_MATRIX, modelViewMat);
      // decompose matrix for yaw and pitch
      double yaw = 0, pitch = 0;
      MatrixHelper::DecomposeYawPitchRollDeg(modelViewMat,
                                             &yaw, &pitch,
                                             (double *)NULL);
      // obtain the view and strafe vectors
      const Vec3d viewVec = MatrixHelper::GetViewVector(modelViewMat);
      const Vec3d strafeVec = MatrixHelper::GetStrafeVector(modelViewMat);
      // obtain the camera's world position
      modelViewMat.MakeInverseFromOrthogonal();
      Vec3d camEye = modelViewMat * Vec3d(0.0, 0.0, 0.0);
      // obtain the repeat count
      const unsigned short rptCnt = (unsigned short)(lParam & 0x0000FFFF);
      const double transScale = rptCnt * 0.15;

      switch (wParam)
      {
      case 'A':
         camEye -= (strafeVec * transScale);
         break;

      case 'D':
         camEye += (strafeVec * transScale);
         break;

      case 'W':
         camEye += (viewVec * transScale);
         break;

      case 'S':
         camEye -= (viewVec * transScale);
         break;
      }

      // negate the cam eye
      camEye *= -1.0;
      // setup a new matrix
      glLoadIdentity();
      // setup the new matrix
      glRotated(pitch, -1.0, 0.0, 0.0);
      glRotated(yaw, 0.0, -1.0, 0.0);
      glTranslated(camEye.mT[0], camEye.mT[1], camEye.mT[2]);
      }

      break;

   default:
      result = OpenGLWindow::MessageHandler(uMsg, wParam, lParam);

      break;
   }
   
   return result;
}

void SmokeParticleSystemWindow::RenderScene( const SimFrame & rSimFrame )
{
   // clear the depth and color buffers
   glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // render the floor
   RenderFloor(rSimFrame);

   // render the particle systems
   ParticleSystemManagerSingleton::Instance()->RenderParticleSystems(rSimFrame);

   // make sure that there are no errors
   VALIDATE_OPENGL_FRAME();

   // swap the buffers
   SwapBuffers(GetHDC());
}

void SmokeParticleSystemWindow::RenderFloor( const SimFrame & simFrame, 
                                             const bool shutdown )
{
   // defines the buffer objects
   static unsigned int glVertBuffers[2] = { 0 };

   // constants
   const unsigned short numVertsAcross = 21;
   const unsigned short numVertsDown = 21;
   const unsigned short numCoords = 3;
   const unsigned short numIndicesPerRow = ((numVertsAcross - 2) * 4) + 6;
   const unsigned short indexDataSize = sizeof(unsigned int);

   // construct the data if not constructed
   if (!glVertBuffers[0] || !glVertBuffers[1])
   {
      // create two buffers
      glGenBuffersARB(2, glVertBuffers);

      // setup the vertex buffer data first
      const unsigned short dataSize = sizeof(float);
      const unsigned short bufferSize = numVertsAcross * numVertsDown * numCoords;

      // create a temp store for the vertices
      float * pVertBuffer = new float[bufferSize];
      float * pVertices = pVertBuffer;

      // construct the data points
      float vertZ = -10.0f;
      for (unsigned short i = 0; i < numVertsDown; ++i, vertZ += 1.0f)
      {
         float vertX = -10.0f;
         for (unsigned short j = 0;
              j < numVertsAcross;
              ++j, vertX += 1.0f, pVertices += 3)
         {
            pVertices[0] = vertX;
            pVertices[1] = 0.0f;
            pVertices[2] = vertZ;
         }
      }

      // send the data to the card
      const GLintptr bufferDataSize = bufferSize * dataSize;
      glBindBufferARB(GL_ARRAY_BUFFER_ARB, glVertBuffers[0]);
      glBufferDataARB(GL_ARRAY_BUFFER_ARB, bufferDataSize,
                      pVertBuffer, GL_STATIC_DRAW_ARB);

      // release the vertices
      delete [] pVertBuffer;

      // setup the index data object
      const unsigned short indexBufferSize = numIndicesPerRow * (numVertsDown - 1);
      
      // create a temp store for the indices
      unsigned int * pIndBuffer = new unsigned int[indexBufferSize];
      unsigned int * pIndices = pIndBuffer;

      // defines the starting index
      unsigned int curVert = 0;
      for (unsigned short i = 1;
           i < numVertsDown; ++i,
           pIndices += 2, ++curVert)
      {
         for (unsigned short j = 1;
              j < numVertsAcross;
              ++j, ++curVert, pIndices += 4)
         {
            pIndices[0] = curVert;
            pIndices[1] = curVert + numVertsAcross + 1;
            pIndices[2] = curVert + numVertsAcross;
            pIndices[3] = curVert;
         }

         // add the last two values
         pIndices[0] = curVert;
         pIndices[1] = curVert + numVertsAcross;
      }

      // send the indices to the card
      const GLintptr indexBufDataSize = indexBufferSize * indexDataSize;
      glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, glVertBuffers[1]);
      glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, indexBufDataSize,
                      pIndBuffer, GL_STATIC_DRAW_ARB);

      // release the indices
      delete [] pIndBuffer;
   }
   else if (shutdown)
   {
      // release the data
      glDeleteBuffersARB(2, glVertBuffers);
      // data not valid
      // early return
      return;
   }

   // bind the buffer objects
   glBindBufferARB(GL_ARRAY_BUFFER_ARB, glVertBuffers[0]);
   glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, glVertBuffers[1]);

   // enable the client states
   glEnableClientState(GL_VERTEX_ARRAY);

   // make sure the current color is white
   glColor3f(1.0f, 1.0f, 1.0f);

   // setup the vertex bindings
   glVertexPointer(numCoords, GL_FLOAT, 0, NULL);

   // render the lines of data
   for (unsigned short i = 0; i < numVertsDown - 1; ++i)
   {
      glDrawElements(GL_LINE_STRIP,
                     numIndicesPerRow,
                     GL_UNSIGNED_INT,
                    ((char *)NULL) + (numIndicesPerRow * indexDataSize * i));
   }
   
   // disable the client states
   glDisableClientState(GL_VERTEX_ARRAY);

   // unbind the buffer objects
   glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
   glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
}

void SmokeParticleSystemWindow::UpdateScene( const SimFrame & rSimFrame )
{
   // update the particle systems
   ParticleSystemManagerSingleton::Instance()->UpdateParticleSystems(rSimFrame);
}

void SmokeParticleSystemWindow::GetSimFrameTime( SimFrame & simFrame )
{
   // indicates first time init
   static bool bInitTime = true;
   static double dMSPerCount = 0.0;

   if (bInitTime)
   {
      // obtain the frequency counter
      LARGE_INTEGER liFreq;
      QueryPerformanceFrequency(&liFreq);

      // determine the number of ms per count
      dMSPerCount = 1.0 / liFreq.QuadPart * 1000.0;

      // initialize the current time
      QueryPerformanceCounter(
         reinterpret_cast< LARGE_INTEGER * >(&simFrame.nCurTime));

      // no longer needing to initialize
      bInitTime = false;
   }

   // move the current into the previous
   simFrame.nPrevTime = simFrame.nCurTime;

   // obtain the current sim time
   QueryPerformanceCounter(
      reinterpret_cast< LARGE_INTEGER * >(&simFrame.nCurTime));

   // determine the amount of time to sleep
   const long long nDelta = simFrame.nCurTime - simFrame.nPrevTime;

   // determine the amount of ms
   const int nWaitMS =
      static_cast< int >(simFrame.dSimTimeMS - (nDelta * dMSPerCount));

   // wait if needed
   if (nWaitMS > 0)
   {
      // finish the rest of the frame
      Sleep(nWaitMS);
      // get the current time
      QueryPerformanceCounter(
         reinterpret_cast< LARGE_INTEGER * >(&simFrame.nCurTime));
      // set the delta time
      const_cast< long long & >(nDelta) = simFrame.nCurTime - simFrame.nPrevTime;
   }

   // save delta time in the sim frame
   simFrame.nTimeDelta = nDelta;
   // convert the sim times to ms
   simFrame.dCurTimeMS = dMSPerCount * simFrame.nCurTime;
   simFrame.dPrevTimeMS = dMSPerCount * simFrame.nPrevTime;
   simFrame.dTimeDeltaMS = dMSPerCount * simFrame.nTimeDelta;
}
