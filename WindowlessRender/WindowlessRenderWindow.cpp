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

// std includes
#include <cstring>
#include <iostream>
#include <algorithm>

// slow the application down
#define LET_APP_RUN_WILD 0
#define LET_SERVER_RUN_WILD 0

// user defined messages to change the size of the texture
const uint32_t WM_INCREASE_FB_SIZE = WM_USER + 1;
const uint32_t WM_DECREASE_FB_SIZE = WM_USER + 2;

struct WindowlessRenderWindow::ImgData
{
   uint32_t       id;
   uint32_t       width;
   uint32_t       height;
   uint32_t       depth;
   size_t         size;
   const void *   pData;
};

WindowlessRenderWindow::WindowlessRenderWindow( ) :
mCurBuffers       ( 0 ),
mServerThread     ( 0 ),
mShowServerWnd    ( false )
{
   // clear out the buffers
   std::memset(mTextures, 0x00, sizeof(mTextures));
   std::memset(mPixelUnpackBuffers, 0x00, sizeof(mPixelUnpackBuffers));
   std::memset(mCurImgDataToRender, 0x00, sizeof(mCurImgDataToRender));

   // init the critical sections
   InitializeCriticalSection(&mImgDataPoolMtx);
   InitializeCriticalSection(&mImgDataToRenderMtx);

   // give some instructions
   std::cout << "S - Show or hide the hidden server window" << std::endl
             << "+ - Increase framebuffer size" << std::endl
             << "- - Decrease framebuffer size" << std::endl
             << std::ends;
}

WindowlessRenderWindow::~WindowlessRenderWindow( )
{
}

bool WindowlessRenderWindow::Create( unsigned int nWidth, unsigned int nHeight,
                                     const char * pWndTitle, const void ** /*pInitParams*/ )
{
   // initialize gl context params
   const OpenGLWindow::OpenGLInit glInit[] =
   {
      { 4, 1, true, true, false },
      { 4, 1, true, false, false },
      { 4, 0, true, true, false },
      { 4, 0, true, false, false },
      { 3, 3, true, true, false },
      { 3, 3, true, false, false },
      { 0 }
   };

   if (OpenGLWindow::Create(nWidth, nHeight, pWndTitle, glInit))
   {
      // make the gl context active
      MakeCurrent();

      // create a new thread
      mServerThread = _beginthreadex(nullptr, 0, &WindowlessRenderWindow::ServerThreadEntry, this, 0, nullptr);

      return true;
   }

   return false;
}

void WindowlessRenderWindow::OnDestroy( )
{
   // release the current image data if set
   std::for_each(mCurImgDataToRender,
                 mCurImgDataToRender + sizeof(mCurImgDataToRender) / sizeof(*mCurImgDataToRender),
   [ this ] ( const ImgData *& pImgData )
   {
      if (pImgData) ReleaseImgData(pImgData); pImgData = nullptr;
   });

   // post and wait for the other thread to quit
   PostThreadMessage(GetThreadId(reinterpret_cast< HANDLE >(mServerThread)), WM_QUIT, 0, 0);
   WaitForSingleObject(reinterpret_cast< HANDLE >(mServerThread), INFINITE);
   CloseHandle(reinterpret_cast< HANDLE >(mServerThread));

   // release the critical sections
   DeleteCriticalSection(&mImgDataPoolMtx);
   DeleteCriticalSection(&mImgDataToRenderMtx);

   // release gl buffers
   glDeleteTextures(2, mTextures);
   glDeleteBuffers(2, mPixelUnpackBuffers);

   // allow base class to cleanup
   OpenGLWindow::OnDestroy();
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
      bQuit = PeekAppMessages(appQuitVal);

      if (!bQuit)
      {
         // render the scene
         RenderScene();

         // if there is time left, then do some waiting
         if (const double deltaMS = localTimer.DeltaMS(begTick) <= MS_PER_FRAME)
         {
            // wait for the remainder of the time
            localTimer.Wait(static_cast< unsigned long >(MS_PER_FRAME - deltaMS));
         }
      }
   }

   return appQuitVal;
}

LRESULT WindowlessRenderWindow::MessageHandler( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
   switch (uMsg)
   {
   case WM_SIZE:
      // update the viewport
      glViewport(0, 0,
                 static_cast< GLsizei >(lParam & 0xFFFF),
                 static_cast< GLsizei >(lParam >> 16));

      break;

   case WM_KEYDOWN:
      // determine any keyboard actions
      switch (wParam)
      {
      case 'S': mShowServerWnd = !mShowServerWnd; break;

      case VK_OEM_PLUS:
      case VK_OEM_MINUS:
         // helper function to release main textures...
         // this could be done such that the textures could be
         // reused, but it is just easier to release them now
         // and have them be recreated...

         const HANDLE complete = CreateEvent(nullptr, TRUE, FALSE, nullptr);
         PostThreadMessage(GetThreadId(reinterpret_cast< HANDLE >(mServerThread)),
                           wParam == VK_OEM_PLUS ? WM_INCREASE_FB_SIZE : WM_DECREASE_FB_SIZE,
                           reinterpret_cast< WPARAM >(complete),
                           0);
         WaitForSingleObject(complete, INFINITE);
         CloseHandle(complete);

         std::for_each(mCurImgDataToRender,
                       mCurImgDataToRender + sizeof(mCurImgDataToRender) / sizeof(*mCurImgDataToRender),
         [ this ] ( const ImgData * const pImgData )
         { 
            if (pImgData) ReleaseImgData(pImgData);
         });
         std::memset(mCurImgDataToRender, 0x00, sizeof(mCurImgDataToRender));

         glDeleteTextures(sizeof(mTextures) / sizeof(*mTextures), mTextures);
         std::memset(mTextures, 0x00, sizeof(mTextures));

         break;
      }

      break;
   }

   return OpenGLWindow::MessageHandler(uMsg, wParam, lParam);
}

void WindowlessRenderWindow::RenderScene( )
{
   // create the projview matrix
   const Matrixf projview = Matrixf::Ortho(0.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f);

   // clear the buffers
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // copy the current contents over to the texture
   const uint32_t tex_id = mTextures[mCurBuffers];
   const uint32_t pbo_id = mPixelUnpackBuffers[mCurBuffers];

   if (tex_id && pbo_id)
   {
      // get the current image data for this texture...
      if (const ImgData * const pImgData = mCurImgDataToRender[mCurBuffers])
      {
         glBindTexture(GL_TEXTURE_2D, tex_id);
         glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo_id);

         glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, pImgData->width, pImgData->height, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
         glGenerateMipmap(GL_TEXTURE_2D);

         glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
         glBindTexture(GL_TEXTURE_2D, 0);

         // release the image data
         ReleaseImgData(pImgData);
         mCurImgDataToRender[mCurBuffers] = nullptr;
      }

      // note: render of this texture should be done
      // by a shader program bound with a vao, but since
      // it is so simple we can ignore it for right now...

      // enable textures and bind the fbo
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, tex_id);

      // update the matrix information...
      glLoadMatrixf(Matrixf::Ortho(0.0f, 1.0f, 0.0f, 1.0f, 1.0, -1.0f));

      // for the texture to replace any current color settings
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

      // start rendering the fullscreen quad
      glBegin(GL_QUADS);

      glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f);
      glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
      glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 0.0f);
      glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 1.0f, 0.0f);

      glEnd();

      // reset the matrix stack
      glLoadIdentity();

      // disable texturing
      glDisable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, 0);
   }

   // get the next image data to render
   const ImgData * const pImgDataToRender = GetNextImgDataToRender();

   if (pImgDataToRender)
   {
      // create a texture if one is not there...
      uint32_t & tex = mTextures[(mCurBuffers + 1) % 2];

      if (!tex)
      {
         glGenTextures(1, &tex);
         glBindTexture(GL_TEXTURE_2D, tex);
         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, pImgDataToRender->width, pImgDataToRender->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
         glBindTexture(GL_TEXTURE_2D, 0);
      }

      // obtain out pixel unpack buffer
      const uint32_t pbo = [ this, pImgDataToRender ] ( ) -> uint32_t
      {
         uint32_t & pbo_id = mPixelUnpackBuffers[(mCurBuffers + 1) % 2];

         if (!pbo_id)
         {
            glGenBuffers(1, &pbo_id);
         }

         return pbo_id;
      }();

      // bind buffer and setup the buffer size...
      glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
      glBufferData(GL_PIXEL_UNPACK_BUFFER, pImgDataToRender->size, nullptr, GL_STREAM_DRAW);

      // copy the data to the buffer
      void * const pData = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
      std::memcpy(pData, pImgDataToRender->pData, pImgDataToRender->size);
      glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

      // buffer data no longer active
      glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

      // save image data for later
      mCurImgDataToRender[(mCurBuffers + 1) % 2] = pImgDataToRender;

      // update the current buffer set
      mCurBuffers = (mCurBuffers + 1) % 2;
   }

   // swap the buffers to display
   SwapBuffers(GetHDC());
}

const WindowlessRenderWindow::ImgData * WindowlessRenderWindow::GetNextImgDataToRender( )
{
   // should be called only from the main thread

   EnterCriticalSection(&mImgDataToRenderMtx);

   if (!mImgDataToRender.empty())
   {
      // get the last rendered image
      const ImgData * const pData = mImgDataToRender.back();
      mImgDataToRender.pop_back();

      // release all other images back to other thread
      std::vector< ImgData * > imgDataToRelease;
      imgDataToRelease.swap(mImgDataToRender);

      LeaveCriticalSection(&mImgDataToRenderMtx);

      std::for_each(imgDataToRelease.cbegin(),
                    imgDataToRelease.cend(),
      [ this ] ( const ImgData * const pImgData ) -> void
      {
         ReleaseImgData(pImgData);
      });

      return pData;
   }

   LeaveCriticalSection(&mImgDataToRenderMtx);

   return nullptr;
}

void WindowlessRenderWindow::ReleaseImgData( const ImgData * const pImgData )
{
   // can be called by both threads

   EnterCriticalSection(&mImgDataPoolMtx);

   mImgDataPool.push_back(const_cast< ImgData * const >(pImgData));

   LeaveCriticalSection(&mImgDataPoolMtx);
}

WindowlessRenderWindow::ImgData * WindowlessRenderWindow::GetNextAvailableImgData( const uint32_t width,
                                                                                   const uint32_t height,
                                                                                   const uint32_t depth )
{
   // should be only be called by the server thread

   ImgData * pImgData = nullptr;

   EnterCriticalSection(&mImgDataPoolMtx);

   if (!mImgDataPool.empty())
   {
      // ReleaseImgData cannot unmap the buffer data, so it
      // needs to be done here... there should only be a few
      // items here, so just walk the list and reset them...
      // not efficient, but it gets the job done for now...
      std::for_each(mImgDataPool.cbegin(),
                    mImgDataPool.cend(),
      [ ] ( ImgData * const pImgData )
      {
         if (pImgData->pData)
         {
            glBindBuffer(GL_PIXEL_PACK_BUFFER, pImgData->id);
            glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
            glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
            pImgData->pData = nullptr;
         }
      });

      pImgData = mImgDataPool.back();
      mImgDataPool.pop_back();

      // might need to update the buffer size
      if (pImgData->width != width || pImgData->height != height || pImgData->depth != depth)
      {
         pImgData->width = width;
         pImgData->height = height;
         pImgData->depth = depth;
         pImgData->size = width * height * depth;

         glBindBuffer(GL_PIXEL_PACK_BUFFER, pImgData->id);
         glBufferData(GL_PIXEL_PACK_BUFFER, pImgData->size, nullptr, GL_STREAM_READ);
         glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
      }

      // if we have more than the ideal size, then reduce by a fraction
      const size_t max_ideal_size = 4;
      const size_t max_ideal_size_denom = 4;
      const size_t pool_size = mImgDataPool.size();

      if (pool_size > max_ideal_size)
      {
         const auto cbegin = mImgDataPool.cbegin() + (pool_size - pool_size / max_ideal_size_denom);

         std::for_each(cbegin, mImgDataPool.cend(),
         [ ] ( ImgData * const pImgData )
         {
            glDeleteBuffers(1, &pImgData->id);
            delete pImgData;
         });

         mImgDataPool.erase(cbegin, mImgDataPool.cend());
      }

      LeaveCriticalSection(&mImgDataPoolMtx);
   }
   else
   {
      LeaveCriticalSection(&mImgDataPoolMtx);

      // create a brand new image data set...
      pImgData = new ImgData;
      pImgData->width = width;
      pImgData->height = height;
      pImgData->depth = depth;
      pImgData->size = width * height * depth;
      pImgData->pData = nullptr;

      // generate a buffer for this set
      glGenBuffers(1, &pImgData->id);
      glBindBuffer(GL_PIXEL_PACK_BUFFER, pImgData->id);
      glBufferData(GL_PIXEL_PACK_BUFFER, pImgData->size, nullptr, GL_STREAM_READ);
      glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
   }

   return pImgData;
}

void WindowlessRenderWindow::AddImgDataForRendering( ImgData * const pImgData )
{
   // should be only be called by the server thread

   EnterCriticalSection(&mImgDataToRenderMtx);

   mImgDataToRender.push_back(pImgData);

   LeaveCriticalSection(&mImgDataToRenderMtx);
}

uint32_t WindowlessRenderWindow::RunServerThread( )
{
   // create a brand new invisible window...
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

      RegisterClassEx(&wnd_class_ex);

      return CreateWindowEx(0,
                            wnd_class_ex.lpszClassName,
                            wnd_class_ex.lpszClassName,
                            0,
                            1, 1, 500, 500,
                            nullptr, nullptr,
                            wnd_class_ex.hInstance,
                            nullptr);
   })();

   // setup opengl for this window...
   // notice, we have a few options turned off that would normally be on...
   const int32_t iAttribs[] =
   {
      // WGL_DRAW_TO_WINDOW_ARB should be false, but we need it to be
      // true if we want to be able to show the contents of the window...
      WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
      WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,
      WGL_SWAP_METHOD_ARB,    WGL_SWAP_UNDEFINED_ARB,
      WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
      WGL_DOUBLE_BUFFER_ARB,  GL_FALSE,
      WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
      WGL_COLOR_BITS_ARB,     32,
      WGL_DEPTH_BITS_ARB,     24,
      WGL_STENCIL_BITS_ARB,   0,
      0,                      0
   };

   // choose the right pixel format for the job
   int32_t format = 0;
   uint32_t numFormats = 0;
   wglChoosePixelFormatARB(GetDC(wnd), iAttribs, nullptr, 1, &format, &numFormats);

   // still need to call windows version fo SetPixelFormat...
   PIXELFORMATDESCRIPTOR pfd = { 0 };
   DescribePixelFormat(GetDC(wnd), format, sizeof(pfd), &pfd);
   pfd.dwFlags |= PFD_SUPPORT_COMPOSITION;
   SetPixelFormat(GetDC(wnd), format, &pfd);

   // describe the kind of profile to use and create it...
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
   // creation doesn't get default frame and depth buffers, but windows
   // does not allow for this due to opengl32's implementation... for now
   // just create it with the associated dc's and eat the cost...
   //wglMakeContextCurrentARB(nullptr, nullptr, glContext);
   wglMakeContextCurrentARB(GetDC(wnd), GetDC(wnd), glContext);

   // width and height of framebuffer
   GLsizei fboWidth = 1024;
   GLsizei fboHeight = 1024;

   // basic camera and projection information...
   const Matrixf camera = Matrixd::LookAt(0.0f, 4.0f, 8.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
   Matrixf perspective = Matrixd::Perspective(45.0f, static_cast< float >(fboWidth) / static_cast< float >(fboHeight), 1.0f, 100.0f);

   // vertex shader used to render into the framebuffer
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

   // fragment shader used to render into the framebuffer
   const char * const pFragShader =
   "#version 400 \n"
   "layout (location = 0) out vec4 FragColor; \n"
   "in vec4 Color; \n"
   "\n"
   "void main( ) \n"
   "{ \n"
      "FragColor = Color; \n"
   "} \n";

   // generate a program based on the shaders above...
   const GLuint shaderProgram = glCreateProgram();
   const GLuint vertShader = shader::LoadShaderSrc(GL_VERTEX_SHADER, pVertShader);
   const GLuint fragShader = shader::LoadShaderSrc(GL_FRAGMENT_SHADER, pFragShader);
   shader::LinkShaders(shaderProgram, vertShader, 0, fragShader);

   // generate the vertex array information
   const GLuint vao = [ ] ( ) -> GLuint
   {
      GLuint vao = 0;
      glGenVertexArrays(1, &vao);
      glBindVertexArray(vao);

      return vao;
   }();

   // generate vertices for the sides of the box...
   // this will be stored in the vao...
   const float vertices[] =
   {
      -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f, 1.0f,  1.0f,
       1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, 1.0f, -1.0f,
       1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f,
      -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, 1.0f,  1.0f,
      -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f, -1.0f, 1.0f, -1.0f
   };

   const GLuint verts = [ &vertices ] ( ) -> GLuint
   {
      GLuint verts = 0;
      glGenBuffers(1, &verts);
      glBindBuffer(GL_ARRAY_BUFFER, verts);
      glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

      return verts;
   }();

   // generate colors for the sides of the box...
   // this will be stored in the vao...
   const float colors[] =
   {
      1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
      0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
      0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
      1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
      1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f
   };

   const GLuint clrs = [ &colors ] ( ) -> GLuint
   {
      GLuint clrs = 0;
      glGenBuffers(1, &clrs);
      glBindBuffer(GL_ARRAY_BUFFER, clrs);
      glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

      return clrs;
   }();

   // vao is now complete... no longer needed...
   glBindVertexArray(0);

   // generate a texture to render into
   GLuint fboTex = 0;
   glGenTextures(1, &fboTex);
   glBindTexture(GL_TEXTURE_2D, fboTex);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, fboWidth, fboHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
   glBindTexture(GL_TEXTURE_2D, 0);

   // attach a render buffer to the framebuffer
   GLuint rbo = 0;
   glGenRenderbuffers(1, &rbo);
   glBindRenderbuffer(GL_RENDERBUFFER, rbo);
   glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fboWidth, fboHeight);
   glBindRenderbuffer(GL_RENDERBUFFER, 0);

   // attach the color texture and the depth buffer to the framebuffer
   GLuint fbo = 0;
   glGenFramebuffers(1, &fbo);
   glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
   glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTex, 0);
   glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

   // deativate the framebuffer
   glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

   // make sure the depth testing and cull detection are enabled
   glEnable(GL_CULL_FACE);
   glEnable(GL_DEPTH_TEST);

   // start our local timer
   Timer localTimer;
   const uint64_t startTime = localTimer.GetCurrentTick();

   // helper function for processing messages on this thread
   const auto PeekMsgs = [ & ] ( ) -> bool
   {
      MSG msg;

      while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) != 0)
      {
         if (msg.message == WM_QUIT)
         {
            // application has requtested termination
            return false;
         }
         if (msg.message == WM_INCREASE_FB_SIZE || msg.message == WM_DECREASE_FB_SIZE)
         {
            // constants not to be exceeded
            const GLsizei MIN_TEX_WIDTH = 1;
            const GLsizei MAX_TEX_WIDTH = 8192;
            const GLsizei MIN_TEX_HEIGHT = 1;
            const GLsizei MAX_TEX_HEIGHT = 8192;

            // make sure not too small or too large
            if (msg.message == WM_INCREASE_FB_SIZE &&
                fboWidth < MAX_TEX_WIDTH && fboHeight < MAX_TEX_HEIGHT)
            {
               fboWidth *= 2; fboHeight *= 2;
            }
            else if (msg.message == WM_DECREASE_FB_SIZE &&
                     fboWidth > MIN_TEX_WIDTH && fboHeight > MIN_TEX_HEIGHT)
            {
               fboWidth /= 2; fboHeight /= 2;
            }

            // indicate the change
            std::cout << "New framebuffer size of " << fboWidth << " x " << fboHeight << std::endl << std::ends;

            // increase / decrease the texture size
            glBindTexture(GL_TEXTURE_2D, fboTex);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, fboWidth, fboHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            glBindTexture(GL_TEXTURE_2D, 0);

            // increase / decrease the depth component size
            glBindRenderbuffer(GL_RENDERBUFFER, rbo);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fboWidth, fboHeight);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            // update the perspective matrix
            perspective.MakePerspective(45.0f, static_cast< float >(fboWidth) / static_cast< float >(fboHeight), 1.0f, 100.0f);

            // call to get the image data... all others will be released, as will this one...
            if (const ImgData * const pImgData = GetNextImgDataToRender())
            {
               ReleaseImgData(pImgData);
            }

            // release anything that might be waiting to finish copying
            auto * const pWndInst = this;
            std::for_each(mImgDataReadyForMapping.cbegin(),
                          mImgDataReadyForMapping.cend(),
            [ &pWndInst ] ( ImgData * const pImgData ) { pWndInst->ReleaseImgData(pImgData); });
            mImgDataReadyForMapping.clear();

            // signal the event complete
            SetEvent(reinterpret_cast< HANDLE >(msg.wParam));
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

   while (PeekMsgs())
   {
      // setup the amount of timer per frame
      const float ROTATE_MULTIPLIER = 72.0f;

#if LET_SERVER_RUN_WILD
      const double MS_PER_FRAME = 0.0;
#else
      const double MS_PER_FRAME = 1000.0 / 60.0;
#endif

      // obtain the current tick count to determine next frame render time
      const uint64_t begTick = localTimer.GetCurrentTick();

      // enable the frame buffer to render into
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

      // clear out the previous framebuffer contents
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      // make sure to setup the viewport for the framebuffer
      glViewport(0, 0, fboWidth, fboHeight);

      // begin using the shader program defined from above
      glUseProgram(shaderProgram);
      
      // update the model view / projection matrix for the shader program
      const float rotate_degs = static_cast< float >(localTimer.DeltaSec(startTime)) * ROTATE_MULTIPLIER;
      glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "ProjViewMat"),
                         1, GL_FALSE,
                         perspective * camera * Matrixf::Rotate(rotate_degs, 0.0f, 1.0f, 0.0f));

      // begin using the vao, which defines our vertices and color buffers
      glBindVertexArray(vao);

      // draw the 5 sides of the boxes
      glDrawArrays(GL_QUADS, 0, sizeof(vertices) / sizeof(*vertices) / 3);

      // no longer needing the vao
      glBindVertexArray(0);
      
      // no longer need shader program
      glUseProgram(0);
      
      // disable rendering into the framebuffer
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

      // get the next unused set of data for mapping
      ImgData * const pImgDataForMapping = GetNextAvailableImgData(fboWidth, fboHeight, 4);

      // copy the contents of the framebuffer into the pbo
      // this is an async operation and will not be completed until next frame...
      glBindTexture(GL_TEXTURE_2D, fboTex);
      glBindBuffer(GL_PIXEL_PACK_BUFFER, pImgDataForMapping->id);
      glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
      glBindTexture(GL_TEXTURE_2D, 0);

      // get the previous mapped image data so we can set this as ready to render for main window
      ImgData * const pImgDataToRender = [ this ] ( ) -> ImgData *
      {
         WGL_ASSERT(mImgDataReadyForMapping.size() <= 1);

         ImgData * pImgData = nullptr;

         if (!mImgDataReadyForMapping.empty())
         {
            pImgData = mImgDataReadyForMapping.back();
            mImgDataReadyForMapping.pop_back();
         }

         return pImgData;
      }();

      // add the image data for mapping to the mapping vector so that
      // next frame we can get the mapped data and send it to the visible
      // window for rendering...
      mImgDataReadyForMapping.push_back(pImgDataForMapping);

      // if there is data from the previous frame, add it
      if (pImgDataToRender)
      {
         // last frame should have copied the data into the pbo...
         // glMapBuffers should be a very quick data pointer fetch...
         glBindBuffer(GL_PIXEL_PACK_BUFFER, pImgDataToRender->id);
         pImgDataToRender->pData = glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);

         if (pImgDataToRender->pData)
         {
            // buffer mapping was successful, so add it for other thread to render
            AddImgDataForRendering(pImgDataToRender);
         }
         else
         {
            // buffer mapping was not successful, so just release the image data
            ReleaseImgData(pImgDataToRender);
         }
      }

      if (mShowServerWnd)
      {
         // server window is supposed to be shown
         if (!IsWindowVisible(wnd)) ShowWindow(wnd, SW_SHOW);

         // get the client width and height
         const RECT rect = [ &wnd ] ( ) -> RECT
         {
            RECT rect; GetClientRect(wnd, &rect); return rect;
         }();

         // note: render of this texture should be done
         // by a shader program bound with a vao, but since
         // it is so simple we can ignore it for right now...

         // clear the color buffer...
         glClear(GL_COLOR_BUFFER_BIT);

         // disable depth writes / reading
         glDepthMask(GL_FALSE);
         glDisable(GL_DEPTH_TEST);

         // setup the viewport
         glViewport(0, 0, rect.right, rect.bottom);

         // enable textures and bind the fbo
         glEnable(GL_TEXTURE_2D);
         glBindTexture(GL_TEXTURE_2D, fboTex);

         // update the matrix information...
         glLoadMatrixf(Matrixf::Ortho(0.0f, 1.0f, 0.0f, 1.0f, 1.0, -1.0f));

         // for the texture to replace any current color settings
         glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

         // start rendering the fullscreen quad
         glBegin(GL_QUADS);

         glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f);
         glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
         glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 0.0f);
         glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 1.0f, 0.0f);

         glEnd();

         // reset the matrix stack
         glLoadIdentity();

         // disable texturing
         glDisable(GL_TEXTURE_2D);
         glBindTexture(GL_TEXTURE_2D, 0);

         // enable depth writes / reading
         glDepthMask(GL_TRUE);
         glEnable(GL_DEPTH_TEST);
      }
      else
      {
         // server window is not supposed to be shown
         if (IsWindowVisible(wnd)) ShowWindow(wnd, SW_HIDE);
      }

      // flush the contents of the buffer...
      // this function returns at anytime, as it does not need to
      // wait on the results of the currently buffered commands...
      glFlush();

      // if there is time left, then do some waiting
      if (const double deltaMS = localTimer.DeltaMS(begTick) <= MS_PER_FRAME)
      {
         // wait for the remainder of the time
         localTimer.Wait(static_cast< unsigned long >(MS_PER_FRAME - deltaMS));
      }
   }

   // helper lambda function to delete all the image data sets
   const auto DeletePixelPackBuffers = [ ] ( const ImgData * const pImgData ) -> void
   {
      if (pImgData->pData)
      {
         glBindBuffer(GL_PIXEL_PACK_BUFFER, pImgData->id);
         glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
         glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
      }

      glDeleteBuffers(1, &pImgData->id);

      delete pImgData;
   };

   // delete all the image data sets
   std::for_each(mImgDataPool.cbegin(), mImgDataPool.cend(), DeletePixelPackBuffers);
   std::for_each(mImgDataToRender.cbegin(), mImgDataToRender.cend(), DeletePixelPackBuffers);
   std::for_each(mImgDataReadyForMapping.cbegin(), mImgDataReadyForMapping.cend(), DeletePixelPackBuffers);

   // release the framebuffer objects
   glDeleteFramebuffers(1, &fbo);
   glDeleteRenderbuffers(1, &rbo);
   glDeleteTextures(1, &fboTex);

   // release the vertex buffer / array objects
   glDeleteBuffers(1, &verts);
   glDeleteBuffers(1, &clrs);
   glDeleteVertexArrays(1, &vao);

   // release the shader objects
   glDeleteShader(vertShader);
   glDeleteShader(fragShader);
   glDeleteProgram(shaderProgram);

   // no longer need this context to be current
   wglMakeContextCurrentARB(nullptr, nullptr, nullptr);

   // delete the gl context for this window thread
   wglDeleteContext(glContext);

   // destroy this window thread
   DestroyWindow(wnd);

   return 0;
}

uint32_t __stdcall WindowlessRenderWindow::ServerThreadEntry( void * pData )
{
   return reinterpret_cast< WindowlessRenderWindow * >(pData)->RunServerThread();
}
