// local includes
#include "WindowlessRenderWindow.h"

// wingl includes
#include "Timer.h"
#include "Matrix.h"
#include "Shaders.h"

// gl includes
#include <GL/glew.h>
#include <GL/wglew.h>
#include <GL/GL.h>

// platform includes
#include <process.h>

// slow the application down
#define LET_APP_RUN_WILD 0
#define LET_SERVER_RUN_WILD 0

WindowlessRenderWindow::WindowlessRenderWindow( ) :
mServerThread     ( 0 )
{
}

WindowlessRenderWindow::~WindowlessRenderWindow( )
{
   PostThreadMessage(GetThreadId(reinterpret_cast< HANDLE >(mServerThread)), WM_QUIT, 0, 0);
   WaitForSingleObject(reinterpret_cast< HANDLE >(mServerThread), INFINITE);
   CloseHandle(reinterpret_cast< HANDLE >(mServerThread));
}

bool WindowlessRenderWindow::Create( unsigned int nWidth, unsigned int nHeight,
                                     const char * pWndTitle, const void ** pInitParams )
{
   // initialize gl context params
   const OpenGLWindow::OpenGLInit glInit41D =
   {
      4, 1, true, true, false
   };

   const OpenGLWindow::OpenGLInit glInit41 =
   {
      4, 1, true, false, false
   };

   const OpenGLWindow::OpenGLInit glInit40D =
   {
      4, 0, true, true, false
   };

   const OpenGLWindow::OpenGLInit glInit40 =
   {
      4, 0, true, false, false
   };

   const OpenGLWindow::OpenGLInit glInit33D =
   {
      3, 3, true, true, false
   };

   const OpenGLWindow::OpenGLInit glInit33 =
   {
      3, 3, true, false, false
   };

   const OpenGLWindow::OpenGLInit * glInit[] =
   {
      &glInit41D, &glInit41, &glInit40D, &glInit40, &glInit33D, &glInit33, nullptr
   };

   if (OpenGLWindow::Create(nWidth, nHeight, pWndTitle,
                            reinterpret_cast< const void ** >(glInit)))
   {
      // make the gl context active
      MakeCurrent();

      // create a new thread
      mServerThread = _beginthreadex(nullptr, 0, &WindowlessRenderWindow::ServerThreadEntry, this, 0, nullptr);

      return true;
   }

   return false;
}

int WindowlessRenderWindow::Run( )
{
   // create a local timer object...
   Timer localTimer;

   // setup the amount of timer per frame
#if LET_APP_RUN_WILD
   const double MS_PER_FRAME = 0.0;
#else
   const double MS_PER_FRAME = 1000.0 / 60.0;
#endif

   // app quit variables
   int appQuitVal = 0;
   bool bQuit = false;

   while (!bQuit)
   {
      // obtain the current tick count
      const long long begTick = localTimer.GetCurrentTick();

      // process all the app messages and then render the scene
      if (!(bQuit = PeekAppMessages(appQuitVal)))
      {
         // render the scene
         RenderScene();

         // if there is time left, then do some waiting
         if (const double deltaMS = localTimer.DeltaMS(begTick) <= MS_PER_FRAME)
         {
            // wait for the remainder of the time
            localTimer.Wait(static_cast< unsigned long >(MS_PER_FRAME - deltaMS));
         }

         // determine the framrate of the application
         const double frame_rate = 1.0 / localTimer.DeltaSec(begTick);
      }
   }

   return appQuitVal;
}

LRESULT WindowlessRenderWindow::MessageHandler( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
   //switch (uMsg)
   //{
   //case WM_SIZE:
   //   {
   //   // update the viewport
   //   glViewport(0, 0, lParam & 0xFFFF, lParam >> 16);
   //   // update the perspective matrix
   //   float perspValues[4] = { 0 };
   //   mPerspective.GetPerspective(perspValues);
   //   mPerspective.MakePerspective(perspValues[0],
   //                                static_cast< float >(lParam & 0xFFFF) / static_cast< float >(lParam >> 16),
   //                                perspValues[2], perspValues[3]);
   //   }

   //   break;

   //case WM_KEYDOWN:
   //   {
   //   // get the view and strafe vector
   //   const Vec3f strafe(mCamera[0], mCamera[4], mCamera[8]);
   //   const Vec3f view(mCamera[2] * -1.0f, mCamera[6] * -1.0f, mCamera[10] * -1.0f);
   //   
   //   switch (wParam)
   //   {
   //   case 'A': mCamera = mCamera * Matrixf::Translate(strafe * 0.25f); break;
   //   case 'D': mCamera = mCamera * Matrixf::Translate(strafe * -0.25f); break;
   //   case 'W': mCamera = mCamera * Matrixf::Translate(view * -0.25f); break;
   //   case 'S': mCamera = mCamera * Matrixf::Translate(view * 0.25f); break;

   //   case VK_OEM_PLUS:
   //   case VK_OEM_MINUS:
   //      // release all the instance data
   //      for (int i = 0; i < NUM_BUILDING_TYPES; ++i)
   //      {
   //         // release the texture
   //         glDeleteTextures(1, &mBuildingInstances[i].mTexID);
   //         // release the buffer data
   //         glDeleteBuffers(1, &mBuildingInstances[i].mIdxBufferID);
   //         glDeleteBuffers(1, &mBuildingInstances[i].mTexBufferID);
   //         glDeleteBuffers(1, &mBuildingInstances[i].mVertBufferID);
   //         glDeleteBuffers(1, &mBuildingInstances[i].mWorldBufferID);
   //         // release the vertex array
   //         glDeleteVertexArrays(1, &mBuildingInstances[i].mVertArrayID);
   //      }

   //      for (int i = 0; i < NUM_TREE_TYPES; ++i)
   //      {
   //         // release the texture
   //         glDeleteTextures(1, &mTreeInstances[i].mTexID);
   //         // release the buffer data
   //         glDeleteBuffers(1, &mTreeInstances[i].mVertBufferID);
   //      }

   //      // clear the instances
   //      memset(mBuildingInstances, 0x00, sizeof(mBuildingInstances));
   //      memset(mTreeInstances, 0x00, sizeof(mTreeInstances));

   //      // update the number of instances
   //      switch (wParam)
   //      {
   //      case VK_OEM_PLUS:
   //         // increase by half
   //         mNumBuildingInstances = static_cast< uint32_t >(mNumBuildingInstances * 1.5);
   //         
   //         break;
   //      case VK_OEM_MINUS:
   //         // reduce by half and cap at 100
   //         mNumBuildingInstances = std::max< uint32_t >(static_cast< uint32_t >(mNumBuildingInstances * 0.5), 100);
   //         
   //         break;
   //      }

   //      mNumTreeInstances = mNumBuildingInstances;

   //      // create the instances
   //      CreateInstances();

   //      break;
   //   }
   //   }

   //   break;

   //case WM_MOUSEMOVE:
   //   {
   //   // obtain the current x and y locations
   //   const short curMouseX = (short)(lParam & 0x0000FFFF);
   //   const short curMouseY = (short)(lParam >> 16);

   //   if (wParam & MK_LBUTTON)
   //   {
   //      // delta values
   //      const short deltaX = curMouseX - mPrevMouseX;
   //      const short deltaY = curMouseY - mPrevMouseY;

   //      // decompose the camera into yaw and pitch
   //      float yaw = 0.0f, pitch = 0.0f;
   //      MatrixHelper::DecomposeYawPitchRollDeg< float >(mCamera, &yaw, &pitch, nullptr);

   //      // go from eye space to world space
   //      // make sure to multiply by -1 as the final matrix translates world to eye
   //      Vec3f eye = mCamera.InverseFromOrthogonal() * Vec3f(0.0f, 0.0f, 0.0f) * -1.0f;

   //      // construct the camera matrix
   //      mCamera = Matrixf::Rotate(pitch + deltaY, 1.0f, 0.0f, 0.0f) *
   //                Matrixf::Rotate(yaw + deltaX, 0.0f, 1.0f, 0.0f) *
   //                Matrixf::Translate(eye);
   //   }

   //   mPrevMouseX = curMouseX;
   //   mPrevMouseY = curMouseY;
   //   }

   //   break;
   //}

   return OpenGLWindow::MessageHandler(uMsg, wParam, lParam);
}

void WindowlessRenderWindow::RenderScene( )
{
   //// create the projview matrix
   //const Matrixf projview = mPerspective * mCamera;

   //// clear the buffers
   //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   //// enable textures
   //glEnable(GL_TEXTURE_2D);

   //// enable instanced building shaders
   //glUseProgram(mBuildingsProgID);

   //{
   //// update the location of the camera
   //const GLint pvm_loc = glGetUniformLocation(mBuildingsProgID, "ProjViewMat");
   //glUniformMatrix4fv(pvm_loc, 1, GL_FALSE, projview);

   //// render the instances
   //for (uint32_t i = 0; i < NUM_BUILDING_TYPES; ++i)
   //{
   //   // get the instance
   //   const BuildingInstance & instance = mBuildingInstances[i];

   //   // bind the texture to location 0
   //   glActiveTexture(GL_TEXTURE0);
   //   glBindTexture(GL_TEXTURE_2D, instance.mTexID);

   //   // setup buffered data
   //   glBindVertexArray(instance.mVertArrayID);

   //   // draw the instanced buildings
   //   glDrawElementsInstanced(GL_QUADS, instance.mIdxBufferSize, GL_UNSIGNED_INT, nullptr, instance.mNumInstances);

   //   // unbind buffered data
   //   glBindVertexArray(0);
   //}
   //}

   //// enable instanced tree shaders
   //glUseProgram(mTreesProgID);

   //{
   //// update the location of the camera
   //const GLint pvm_loc = glGetUniformLocation(mTreesProgID, "ProjViewMat");
   //glUniformMatrix4fv(pvm_loc, 1, GL_FALSE, projview);

   //// update the camera location
   //const GLint cam_loc = glGetUniformLocation(mTreesProgID, "CameraPos");
   //glUniform3fv(cam_loc, 1, mCamera.InverseFromOrthogonal() * Vec3f(0.0f, 0.0f, 0.0f));

   //// get the texture coordinates and size locations
   //const GLint tex_loc = glGetUniformLocation(mTreesProgID, "TexCoords");
   //const GLint size_loc = glGetUniformLocation(mTreesProgID, "Size");

   //// render the instances
   //for (uint32_t i = 0; i < NUM_TREE_TYPES; ++i)
   //{
   //   // get the instance
   //   const TreeInstance & instance = mTreeInstances[i];

   //   // update the tex coords and size
   //   glUniform2fv(size_loc, 1, instance.mSize);
   //   glUniform2fv(tex_loc, 4, instance.mTexCoords);

   //   // bind the texture to location 0
   //   glActiveTexture(GL_TEXTURE0);
   //   glBindTexture(GL_TEXTURE_2D, instance.mTexID);

   //   // bind the vertex data and setup the stream of data
   //   glBindBuffer(GL_ARRAY_BUFFER, instance.mVertBufferID);
   //   glEnableVertexAttribArray(0);
   //   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

   //   // render the data
   //   glDrawArrays(GL_POINTS, 0, instance.mNumInstances);

   //   // unbind the buffered data
   //   glBindBuffer(GL_ARRAY_BUFFER, 0);
   //}
   //}

   //// disable instanced tree shaders
   //glUseProgram(0);

   //// disable texturing
   //glDisable(GL_TEXTURE_2D);

   //// swap the buffers to display
   //SwapBuffers(GetHDC());
}

uint32_t WindowlessRenderWindow::RunServerThread( )
{
   const HWND wnd = ([ ] ( ) -> HWND
   {
      WNDCLASSEX wnd_class_ex;
      wnd_class_ex.cbSize = sizeof(wnd_class_ex);
      wnd_class_ex.style = CS_HREDRAW | CS_OWNDC | CS_VREDRAW;
      wnd_class_ex.lpfnWndProc = &DefWindowProc;
      wnd_class_ex.cbClsExtra = 0;
      wnd_class_ex.cbWndExtra = 0;
      wnd_class_ex.hInstance = GetModuleHandle(nullptr);
      wnd_class_ex.hIcon = nullptr;
      wnd_class_ex.hCursor = nullptr;
      wnd_class_ex.hbrBackground = nullptr;
      wnd_class_ex.lpszMenuName = nullptr;
      wnd_class_ex.lpszClassName = "gl server window";
      wnd_class_ex.hIconSm = nullptr;

      const ATOM atom = RegisterClassEx(&wnd_class_ex);

      return CreateWindowEx(0,
                            wnd_class_ex.lpszClassName,
                            wnd_class_ex.lpszClassName,
                            WS_VISIBLE,
                            1, 1, 500, 500,
                            nullptr, nullptr,
                            wnd_class_ex.hInstance,
                            nullptr);
   })();

   const int32_t iAttribs[] =
   {
      WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
      WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,
      WGL_SWAP_METHOD_ARB,    WGL_SWAP_UNDEFINED_ARB,
      WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
      WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
      WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
      WGL_COLOR_BITS_ARB,     32,
      WGL_DEPTH_BITS_ARB,     24,
      WGL_STENCIL_BITS_ARB,   0,
      0,                      0
   };

   int32_t format = 0;
   uint32_t numFormats = 0;
   wglChoosePixelFormatARB(GetDC(wnd), iAttribs, nullptr, 1, &format, &numFormats);

   PIXELFORMATDESCRIPTOR pfd = { 0 };
   DescribePixelFormat(GetDC(wnd), format, sizeof(pfd), &pfd);
   pfd.dwFlags |= PFD_SUPPORT_COMPOSITION;
   SetPixelFormat(GetDC(wnd), format, &pfd);

   const int32_t attribList[] =
   {
      WGL_CONTEXT_MAJOR_VERSION_ARB,   4,
      WGL_CONTEXT_MINOR_VERSION_ARB,   0,
      WGL_CONTEXT_PROFILE_MASK_ARB,    WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
      WGL_CONTEXT_FLAGS_ARB,           0,
      0,                               0
   };

   const HGLRC glContext = wglCreateContextAttribsARB(GetDC(wnd), nullptr, attribList);

   // should be able to do the following line below to make sure that context
   // creation doesn't get a default frame and depth buffers, but windows
   // does not allow for this due to opengl32's implementation... for now
   // just create it with the associated dc's and eat the cost...
   //wglMakeContextCurrentARB(nullptr, nullptr, glContext);

   wglMakeContextCurrentARB(GetDC(wnd), GetDC(wnd), glContext);

   Timer localTimer;

   // helper function for processing messages
   const auto PeekMsgs = [ ] ( ) -> bool
   {
      MSG msg;

      while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) != 0)
      {
         if (msg.message == WM_QUIT)
         {
            // application has requtested termination
            return false;
         }
         else
         {
            // translate and route the message
            TranslateMessage(&msg);
            DispatchMessage(&msg);
         }
      }

      return true;
   };

   const int32_t width = 500;
   const int32_t height = 500;

   const Matrixd camera = Matrixd::LookAt(0.0, 4.0, 8.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
   const Matrixd perspective = Matrixd::Perspective(45.0, static_cast< double >(width) / static_cast< double >(height), 1.0, 100.0);

   const char * const pVertShader =
   "#version 400 \n"
   "layout (location = 0) in vec3 Position; \n"
   "layout (location = 1) in vec4 VertColor; \n"
   "uniform mat4 ProjViewMat; \n"
   "out vec4 Color; \n"
   " \n"
   "void main( ) \n"
   "{ \n"
      "Color = VertColor; \n"
      "gl_Position = ProjViewMat * vec4(Position, 1.0); \n"
   "} \n";

   const char * const pFragShader =
   "#version 400 \n"
   "layout (location = 0) out vec4 FragColor; \n"
   "in vec4 Color; \n"
   "\n"
   "void main( ) \n"
   "{ \n"
      "FragColor = Color; \n"
   "} \n";

   const GLuint shaderProgram = glCreateProgram();
   const GLuint vertShader = shader::LoadShaderSrc(GL_VERTEX_SHADER, pVertShader);
   const GLuint fragShader = shader::LoadShaderSrc(GL_FRAGMENT_SHADER, pFragShader);
   shader::LinkShaders(shaderProgram, vertShader, 0, fragShader);

   GLuint vao = 0;
   glGenVertexArrays(1, &vao);
   glBindVertexArray(vao);

   const float vertices[] =
   {
      -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f, 1.0f,  1.0f,
       1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f,
       1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f,
      -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, 1.0f,  1.0f,
      -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f, -1.0f, 1.0f, -1.0f
   };

   GLuint verts = 0;
   glGenBuffers(1, &verts);
   glBindBuffer(GL_ARRAY_BUFFER, verts);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

   const float colors[] =
   {
      1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
      0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
      0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
      1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
      1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f
   };

   GLuint clrs = 0;
   glGenBuffers(1, &clrs);
   glBindBuffer(GL_ARRAY_BUFFER, clrs);
   glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

   glBindVertexArray(0);

   glEnable(GL_CULL_FACE);
   glEnable(GL_DEPTH_TEST);

   const uint64_t startTime = localTimer.GetCurrentTick();

   while (PeekMsgs())
   {
      // setup the amount of timer per frame
#if LET_SERVER_RUN_WILD
      const double ROTATE_MULTIPLIER = 1.0;
      const double MS_PER_FRAME = 0.0;
#else
      const double ROTATE_MULTIPLIER = 72.0;
      const double MS_PER_FRAME = 1000.0 / 60.0;
#endif

      // obtain the current tick count
      const uint64_t begTick = localTimer.GetCurrentTick();

      const Matrixf projViewModel =
         perspective * camera * Matrixd::Rotate(localTimer.DeltaSec(startTime) * ROTATE_MULTIPLIER, 0.0, 1.0, 0.0);

      glUseProgram(shaderProgram);

      const GLint pvm_loc = glGetUniformLocation(shaderProgram, "ProjViewMat");
      glUniformMatrix4fv(pvm_loc, 1, GL_FALSE, projViewModel);

      glBindVertexArray(vao);

      glDrawArrays(GL_QUADS, 0, sizeof(vertices) / sizeof(*vertices) / 3);

      glBindVertexArray(0);

      glUseProgram(0);

      // if there is time left, then do some waiting
      if (const double deltaMS = localTimer.DeltaMS(begTick) <= MS_PER_FRAME)
      {
         // wait for the remainder of the time
         localTimer.Wait(static_cast< unsigned long >(MS_PER_FRAME - deltaMS));
      }

      SwapBuffers(GetDC(wnd));
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   }

   glDeleteBuffers(1, &verts);
   glDeleteBuffers(1, &clrs);
   glDeleteVertexArrays(1, &vao);

   glDeleteShader(vertShader);
   glDeleteShader(fragShader);
   glDeleteProgram(shaderProgram);

   wglMakeContextCurrentARB(nullptr, nullptr, nullptr);

   wglDeleteContext(glContext);

   DestroyWindow(wnd);

   return 0;
}

uint32_t __stdcall WindowlessRenderWindow::ServerThreadEntry( void * pData )
{
   return reinterpret_cast< WindowlessRenderWindow * >(pData)->RunServerThread();
}
