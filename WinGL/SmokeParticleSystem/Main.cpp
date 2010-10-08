// local includes
#include "Matrix.h"
#include "SimFrame.h"
#include "MatrixHelper.h"
#include "OpenGLExtensions.h"
#include "SmokeParticleSystem.h"
#include "ParticleSystemManager.h"

// os includes
#include <windows.h>

// gl includes
#include <gl/gl.h>
#include <gl/glu.h>

// crt includes
#include <memory.h>

// library includes
#pragma comment( lib, "OpenGL32.lib" )
#pragma comment( lib, "Glu32.lib" )
#pragma comment( lib, "Winmm.lib" )

// global defines
#ifdef _DEBUG
#define VALIDATE_OPENGL_FRAME( ) \
   if (glGetError() != GL_NO_ERROR) *((int *)0xDEADBEEF) = 0;
#else
#define VALIDATE_OPENGL_FRAME( )
#endif

// function declarations
HWND CreateApplicationWindow( const char * pWndTitle );
HGLRC InitializeOpenGL( HWND hWnd, int verMajor, int verMinor, bool compatibleContext );
void ProcessMsgs( HWND hWnd, HGLRC hGLRC, bool & bContinue );
void UpdateScene( HWND hWnd, const SimFrame & rSimFrame );
void RenderScene( HWND hWnd, HGLRC hGLRC, const SimFrame & rSimFrame );
void GetSimFrameTime( SimFrame & simFrame );
void RenderFloor( const SimFrame & simFrame, const bool shutdown = false );

// main entry point of application
int WINAPI WinMain( HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine,
                    int nShowCmd )
{
   // set the min timer resolution
   timeBeginPeriod(1);

   // create the main application window
   HWND hWnd = CreateApplicationWindow("Smoke Particle System");

   // init open gl for the specified window
   if (HGLRC hGLRC = InitializeOpenGL(hWnd, 3, 2, true))
   {
      /////////////////////////////////
      // temp
      ParticleSystemManagerSingleton::Instance()->AddParticleSystem(new SmokeParticleSystem);
      /////////////////////////////////

      // make the render process current
      wglMakeCurrent(GetDC(hWnd), hGLRC);

      // vars for the loop
      SimFrame simFrame;
      bool bContinue = true;

      // init the sim frame frame time
      simFrame.dSimTimeMS = 1000.0 / 30.0;

      // initialize sim frame timer
      GetSimFrameTime(simFrame);

      // basic message pump and render frame
      while (bContinue)
      {
         // process all the messages
         ProcessMsgs(hWnd, hGLRC, bContinue);

         // update the scene
         UpdateScene(hWnd, simFrame);

         // begin rendering the scene
         RenderScene(hWnd, hGLRC, simFrame);

         // get next sim frame time
         // wait if frame time is not up
         GetSimFrameTime(simFrame);
      }

      // make the context current
      wglMakeCurrent(GetDC(hWnd), hGLRC);

      // tell the floor to release resources
      RenderFloor(simFrame, true);
      // release all the particle systems
      ParticleSystemManagerSingleton::Release();

      // remove the current render process
      wglMakeCurrent(NULL, NULL);

      // release the render context
      wglDeleteContext(hGLRC);
   }

   // destroy the window
   DestroyWindow(hWnd);

   // reset the min timer resolution
   timeEndPeriod(1);

   return 0;
}

void ProcessMsgs( HWND hWnd,
                  HGLRC hGLRC,
                  bool & bContinue )
{
   // message pump variable
   static MSG sMSG;
   // create a static to hold the dc
   static HDC sHDC = GetDC(hWnd);

   if (PeekMessage(&sMSG, NULL, 0, 0, PM_REMOVE))
   {
      // make the context current
      wglMakeCurrent(sHDC, hGLRC);

      // process all the incoming messages
      do
      {
         switch (sMSG.message)
         {
         case WM_QUIT:
            bContinue = false; 

            break;
         default:
            DispatchMessage(&sMSG);

            break;
         }
      } while (PeekMessage(&sMSG, NULL, 0, 0, PM_REMOVE));

      // release the context
      wglMakeCurrent(sHDC, NULL);
   }
}

void UpdateScene( HWND hWnd,
                  const SimFrame & rSimFrame )
{
   // update the particle systems
   ParticleSystemManagerSingleton::Instance()->UpdateParticleSystems(rSimFrame);
}

void RenderScene( HWND hWnd,
                  HGLRC hGLRC,
                  const SimFrame & rSimFrame )
{
   // create a staic to hold the dc
   static HDC hDC = GetDC(hWnd);

   // make the context current
   wglMakeCurrent(hDC, hGLRC);

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
   SwapBuffers(hDC);

   // release the current context
   wglMakeCurrent(hDC, NULL);
}

// defines the application message pump
LRESULT CALLBACK AppWndMsgPump( HWND hWnd,
                                UINT uMsg,
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
      gluPerspective(60.0,
                    (double)(lParam & 0x0000FFFF) /
                    (double)(lParam >> 16),
                     0.1, 100.0);
      // switch back to the modelview matrixt
      glMatrixMode(GL_MODELVIEW);

      break;

   case WM_KEYUP:
      switch (wParam)
      {
      case VK_ESCAPE:
         // close the application
         PostMessage(hWnd, WM_CLOSE, 0, 0);

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
         const short deltaX = curX - mouseLocX;
         const short deltaY = curY - mouseLocY;
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
         pitch = min(90.0, pitch);
         pitch = max(-90.0, pitch);
         // get the current camera world position
         modelViewMat.InverseFromOrthogonal();
         Vectord camEye = modelViewMat * Vectord(0.0, 0.0, 0.0);
         camEye *= -1.0;
         // clear the modelview matrix
         glLoadIdentity();
         // setup the new matrix
         glRotated(pitch, 1.0, 0.0, 0.0);
         glRotated(yaw, 0.0, 1.0, 0.0);
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
      const double yaw = 0, pitch = 0;
      MatrixHelper::DecomposeYawPitchRollDeg(modelViewMat,
                                             const_cast< double * >(&yaw),
                                             const_cast< double * >(&pitch),
                                            (double *)NULL);
      // obtain the view and strafe vectors
      const Vectord viewVec = MatrixHelper::GetViewVector(modelViewMat);
      const Vectord strafeVec = MatrixHelper::GetStrafeVector(modelViewMat);
      // obtain the camera's world position
      modelViewMat.InverseFromOrthogonal();
      Vectord camEye = modelViewMat * Vectord(0.0, 0.0, 0.0);
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
      glRotated(pitch, 1.0, 0.0, 0.0);
      glRotated(yaw, 0.0, 1.0, 0.0);
      glTranslated(camEye.mT[0], camEye.mT[1], camEye.mT[2]);
      }

      break;

   default:

      result = DefWindowProc(hWnd, uMsg, wParam, lParam);
   }
   
   return result;
}

// initializes OpenGL for the specified window
HGLRC InitializeOpenGL( HWND hWnd,
                        int verMajor, int verMinor,
                        bool compatibleContext )
{
   // create a pixel format descriptor
   PIXELFORMATDESCRIPTOR pfd;

   // clear out the pfd
   memset(&pfd, 0x00, sizeof(pfd));

   // modify the attributes required to create
   // a basic OpenGL 2.1 interface
   pfd.nSize = sizeof(pfd);
   pfd.nVersion = 1;
   pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
   pfd.iPixelType = PFD_TYPE_RGBA;
   pfd.cColorBits = 24;
   pfd.cAlphaBits = 8;
   pfd.cDepthBits = 32;
   pfd.iLayerType = PFD_MAIN_PLANE;

   // obtain the pixel format
   int pixelFormat = ChoosePixelFormat(GetDC(hWnd), &pfd);

   // set the pixel format
   HDC hDC = GetDC(hWnd);
   SetPixelFormat(hDC, pixelFormat, &pfd);

   // create the basic opengl context
   HGLRC basicContext = wglCreateContext(GetDC(hWnd));

   // make basic current
   wglMakeCurrent(hDC, basicContext);

   // initialize the opengl extensions
   // need to reinterpret the function sig to make x64 happy...
   OpenGLExt::InitializeOpenGLExtensions(
      reinterpret_cast< int (* (__stdcall *)( const char * ))( void ) >
      (&wglGetProcAddress));

   // release and destroy basic context
   wglMakeCurrent(hDC, NULL);
   wglDeleteContext(basicContext);

   // create a new context
   int attribList[] =
   {
      OpenGLExt::WGL_CONTEXT_MAJOR_VERSION_ARB, verMajor,
      OpenGLExt::WGL_CONTEXT_MINOR_VERSION_ARB, verMinor,
      OpenGLExt::WGL_CONTEXT_PROFILE_MASK_ARB, compatibleContext ? 
                                               OpenGLExt::WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB :
                                               OpenGLExt::WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
      0, 0
   };

   HGLRC coreContext =
      OpenGLExt::wglCreateContextAttribsARB(hDC, NULL, attribList);

   if (!coreContext)
   {
      // try creating without the profile mask
      attribList[4] = 0;
      attribList[5] = 0;

      coreContext =
         OpenGLExt::wglCreateContextAttribsARB(hDC, NULL, attribList);
   }

   // if the context is valid, then tell the window
   // to resize.  this will update the viewport and
   // projection information.
   if (coreContext)
   {
      // get the current window size
      RECT wndRect;
      GetWindowRect(hWnd, &wndRect);
      // set the current context
      wglMakeCurrent(hDC, coreContext);
      // send the message to the window
      SendMessage(hWnd, WM_SIZE, 0,
                 (wndRect.bottom - wndRect.top) << 16 |
                 (wndRect.right - wndRect.left));
      // enable basic states
      glEnable(GL_DEPTH_TEST);
      // setup the initial camera position
      glMatrixMode(GL_MODELVIEW);
      glTranslatef(0.0f, -5.0f, 0.0f);
      // release the current context
      wglMakeCurrent(hDC, NULL);
   }

   return coreContext;
}

// creats an application window
HWND CreateApplicationWindow( const char * pWndTitle )
{
   // create the class type
   WNDCLASSEX wndClassEx =
   {
      sizeof(WNDCLASSEX),
      CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
      &AppWndMsgPump,
      0, 0,
      (HINSTANCE)GetCurrentProcess(),
      NULL,
      LoadCursor(NULL, IDC_ARROW),
      NULL,
      NULL,
      pWndTitle,
      NULL
   };

   // register the class
   ATOM classAtom = RegisterClassEx(&wndClassEx);

   // create the window
   HWND hWnd =
      CreateWindowEx(WS_EX_OVERLAPPEDWINDOW,
                     (LPCSTR)classAtom,
                     pWndTitle,
                     WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_OVERLAPPEDWINDOW,
                     CW_USEDEFAULT, CW_USEDEFAULT,
                     800, 800,
                     NULL,
                     NULL,
                     (HINSTANCE)GetCurrentProcess(),
                     NULL);

   // make the window visible
   ShowWindow(hWnd, SW_SHOW);

   return hWnd;
}

void GetSimFrameTime( SimFrame & simFrame )
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

void RenderFloor( const SimFrame & simFrame,
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
      OpenGLExt::glGenBuffersARB(2, glVertBuffers);

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
      const OpenGLExt::intptr bufferDataSize = bufferSize * dataSize;
      OpenGLExt::glBindBufferARB(OpenGLExt::GL_ARRAY_BUFFER_ARB, glVertBuffers[0]);
      OpenGLExt::glBufferDataARB(OpenGLExt::GL_ARRAY_BUFFER_ARB, bufferDataSize,
                                 pVertBuffer, OpenGLExt::GL_STATIC_DRAW_ARB);

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
      const OpenGLExt::intptr indexBufDataSize = indexBufferSize * indexDataSize;
      OpenGLExt::glBindBufferARB(OpenGLExt::GL_ELEMENT_ARRAY_BUFFER_ARB, glVertBuffers[1]);
      OpenGLExt::glBufferDataARB(OpenGLExt::GL_ELEMENT_ARRAY_BUFFER_ARB, indexBufDataSize,
                                 pIndBuffer, OpenGLExt::GL_STATIC_DRAW_ARB);

      // release the indices
      delete [] pIndBuffer;
   }
   else if (shutdown)
   {
      // release the data
      OpenGLExt::glDeleteBuffersARB(2, glVertBuffers);
      // data not valid
      // early return
      return;
   }

   // bind the buffer objects
   OpenGLExt::glBindBufferARB(OpenGLExt::GL_ARRAY_BUFFER_ARB, glVertBuffers[0]);
   OpenGLExt::glBindBufferARB(OpenGLExt::GL_ELEMENT_ARRAY_BUFFER_ARB, glVertBuffers[1]);

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
   OpenGLExt::glBindBufferARB(OpenGLExt::GL_ARRAY_BUFFER_ARB, 0);
   OpenGLExt::glBindBufferARB(OpenGLExt::GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
}