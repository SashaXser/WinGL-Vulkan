// local includes
#include "MultisampleFramebufferWindow.h"

// wgl includes
#include "WglAssert.h"

// gl includes

// platform includes
#include <windows.h>

// std incluces
#include <chrono>
#include <functional>
#include <iterator>
#include <utility>

constexpr GLsizei NUM_SAMPLES { 16 };

static std::pair< HWND, HGLRC > hidden_gl_window { };

std::pair< HWND, HGLRC > CreateHiddenWindow( )
{
   std::pair< HWND, HGLRC >
      window { };

   const WNDCLASSEX window_class {
      sizeof(window_class),
      CS_OWNDC,
      &DefWindowProc,
      0, 0,
      GetModuleHandle(nullptr),
      nullptr,
      nullptr,
      nullptr,
      nullptr,
      "hidden_gl_window",
      nullptr
   };

   RegisterClassEx(
      &window_class);

   window.first =
      CreateWindowEx(
         WS_EX_NOACTIVATE,
         window_class.lpszClassName,
         "hidden gl window",
         0,
         0, 0, 16, 16,
         nullptr,
         nullptr,
         window_class.hInstance,
         nullptr);

   if (window.first)
   {
      const PIXELFORMATDESCRIPTOR pfd {
         sizeof(pfd),
         1,
         PFD_SUPPORT_OPENGL | PFD_DEPTH_DONTCARE |
         PFD_DOUBLEBUFFER_DONTCARE | PFD_STEREO_DONTCARE,
         PFD_TYPE_RGBA,
         32,
         8, 16,
         8, 8,
         8, 0,
         8, 24,
         0, 0, 0, 0, 0,
         0, 0,
         0,
         0,
         0,
         0,
         0,
         0
      };

      const int32_t pixel_format =
         ChoosePixelFormat(
            GetDC(window.first),
            &pfd);

      SetPixelFormat(
         GetDC(window.first),
         pixel_format,
         &pfd);
      
      window.second =
         wglCreateContext(
            GetDC(window.first));
   }

   return window;
}

MultisampleFramebufferWindow::MultisampleFramebufferWindow( ) :
current_frame { 0 },
frame_buffer { 0 },
color_buffer { 0 },
depth_buffer { 0 },
#if ENABLE_MULTISAMPLE_FRAMEBUFFER
frame_buffer_resolve { 0 },
color_buffer_resolve { 0 },
#endif
quit_render_thread { false },
frame_mode { FrameMode::RENDER }
{
   if (!hidden_gl_window.first ||
       !hidden_gl_window.second)
   {
      hidden_gl_window =
         CreateHiddenWindow();
   }
}

MultisampleFramebufferWindow::~MultisampleFramebufferWindow( )
{
   auto deleted =
      wglDeleteContext(
         hidden_gl_window.second);

   deleted =
      DeleteDC(
         GetDC(hidden_gl_window.first));

   deleted =
      DestroyWindow(
         hidden_gl_window.first);
}

bool MultisampleFramebufferWindow::Create(
   unsigned int nWidth,
   unsigned int nHeight,
   const char * pWndTitle,
   const void * /*pInitParams*/ )
{
   // initialize with a context else nothing
   const OpenGLWindow::OpenGLInit glInit[] =
   {
      { 4, 6, true, true, false },
      { 4, 0, true, true, false },
      { 3, 2, true, true, false },
      { }
   };

   // call base class to init
   if (OpenGLWindow::Create(
       nWidth, nHeight, pWndTitle, glInit))
   {
      std::mutex init_complete_mutex;
      std::condition_variable init_complete;

      render_thread =
         std::thread(
            &MultisampleFramebufferWindow::RenderThread,
            this,
            std::ref(init_complete));

      std::unique_lock lock {
         init_complete_mutex
      };

      init_complete.wait(lock);

      // make the context current
      MakeCurrent();

      return true;
   }

   return false;
}

void MultisampleFramebufferWindow::OnDestroy( )
{
   quit_render_thread.store(
      true);

   frame.notify_all();

   render_thread.join();

   // call the base class destroy
   OpenGLWindow::OnDestroy();
}
            
int MultisampleFramebufferWindow::Run( )
{
   // vars for the loop
   int appQuitVal = 0;
   bool bQuit = false;

   // basic message pump and render frame
   while (!bQuit)
   {
      {
         std::unique_lock lock {
            frame_mutex
         };

         frame.wait(
            lock,
            [ this ]
            {
               return frame_mode == FrameMode::PRESENT;
            });
      }

      // process all the messages
      bQuit = PeekAppMessages(appQuitVal);

      if (!bQuit)
      {
         RenderColorBuffer();

         // swap the front and back buffers
         SwapBuffers(GetHDC());
      }

      // do not call member variables after class is released
      if (!bQuit)
      {
         frame_mode =
            FrameMode::RENDER;

         frame.notify_all();
      }
   }

   return appQuitVal;
}

LRESULT MultisampleFramebufferWindow::MessageHandler(
   UINT uMsg,
   WPARAM wParam,
   LPARAM lParam )
{
   LRESULT result = 0;

   switch (uMsg)
   {
   case WM_SIZE:
      result =
         OpenGLWindow::MessageHandler(
            uMsg,
            wParam,
            lParam);
      
      if (color_buffer && depth_buffer
#if ENABLE_MULTISAMPLE_FRAMEBUFFER
          && color_buffer_resolve
#endif
         )
      {
         InitTextures();
      }

      break;

   default:
      // allow default processing to happen
      result =
         OpenGLWindow::MessageHandler(
            uMsg,
            wParam,
            lParam);
   }
   
   return result;
}

void MultisampleFramebufferWindow::InitGLData( )
{
   const GLenum texture_target {
#if ENABLE_MULTISAMPLE_FRAMEBUFFER
      GL_TEXTURE_2D_MULTISAMPLE
#else
      GL_TEXTURE_2D
#endif // ENABLE_MULTISAMPLE_FRAMEBUFFER
   };

   for (auto & cb : color_buffer)
   {
      glGenTextures(
         1,
         &cb);
   }

   glGenTextures(
      1,
      &depth_buffer);

#if ENABLE_MULTISAMPLE_FRAMEBUFFER
   glGenTextures(
      1,
      &color_buffer_resolve);
#endif

   InitTextures();

   for (auto & fb : frame_buffer)
   {
      glGenFramebuffers(
         1,
         &fb);
      glBindFramebuffer(
         GL_FRAMEBUFFER,
         fb);
      glFramebufferTexture2D(
         GL_FRAMEBUFFER,
         GL_COLOR_ATTACHMENT0,
         texture_target,
         color_buffer[&fb - frame_buffer],
         0);
      glFramebufferTexture2D(
         GL_FRAMEBUFFER,
         GL_DEPTH_STENCIL_ATTACHMENT,
         texture_target,
         depth_buffer,
         0);

      const auto status =
         glCheckFramebufferStatus(
            GL_FRAMEBUFFER);

      WGL_ASSERT(
         status == GL_FRAMEBUFFER_COMPLETE);
   }

#if ENABLE_MULTISAMPLE_FRAMEBUFFER

   glGenFramebuffers(
      1,
      &frame_buffer_resolve);
   glBindFramebuffer(
      GL_FRAMEBUFFER,
      frame_buffer_resolve);
   glFramebufferTexture2D(
      GL_FRAMEBUFFER,
      GL_COLOR_ATTACHMENT0,
      GL_TEXTURE_2D,
      color_buffer_resolve,
      0);

   const auto status_resolve =
      glCheckFramebufferStatus(
         GL_FRAMEBUFFER);

   WGL_ASSERT(
      status_resolve == GL_FRAMEBUFFER_COMPLETE);

#endif

   glBindFramebuffer(
      GL_FRAMEBUFFER,
      0);
}

void MultisampleFramebufferWindow::InitTextures( )
{
   const GLenum texture_target {
#if ENABLE_MULTISAMPLE_FRAMEBUFFER
      GL_TEXTURE_2D_MULTISAMPLE
#else
      GL_TEXTURE_2D
#endif // ENABLE_MULTISAMPLE_FRAMEBUFFER
   };

   for (const auto cb : color_buffer)
   {
      glBindTexture(
         texture_target,
         cb);
#if ENABLE_MULTISAMPLE_FRAMEBUFFER
      glTexImage2DMultisample(
         GL_TEXTURE_2D_MULTISAMPLE,
         NUM_SAMPLES,
         GL_RGBA8,
         GetSize().width,
         GetSize().height,
         GL_FALSE);
#else // ENABLE_MULTISAMPLE_FRAMEBUFFER
      glTexImage2D(
         GL_TEXTURE_2D,
         0,
         GL_RGBA8,
         GetSize().width,
         GetSize().height,
         0,
         GL_BGRA,
         GL_UNSIGNED_BYTE,
         nullptr);
      glTexParameteri(
         GL_TEXTURE_2D,
         GL_TEXTURE_MIN_FILTER,
         GL_NEAREST);
      glTexParameteri(
         GL_TEXTURE_2D,
         GL_TEXTURE_MAG_FILTER,
         GL_NEAREST);
#endif // ENABLE_MULTISAMPLE_FRAMEBUFFER
   }

   glBindTexture(
      texture_target,
      depth_buffer);
#if ENABLE_MULTISAMPLE_FRAMEBUFFER
   glTexImage2DMultisample(
      GL_TEXTURE_2D_MULTISAMPLE,
      NUM_SAMPLES,
      GL_DEPTH32F_STENCIL8,
      GetSize().width,
      GetSize().height,
      GL_FALSE);
#else // ENABLE_MULTISAMPLE_FRAMEBUFFER
   glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_DEPTH32F_STENCIL8,
      GetSize().width,
      GetSize().height,
      0,
      GL_DEPTH_STENCIL,
      GL_FLOAT_32_UNSIGNED_INT_24_8_REV,
      nullptr);
   glTexParameteri(
      GL_TEXTURE_2D,
      GL_TEXTURE_MIN_FILTER,
      GL_NEAREST);
   glTexParameteri(
      GL_TEXTURE_2D,
      GL_TEXTURE_MAG_FILTER,
      GL_NEAREST);
#endif // ENABLE_MULTISAMPLE_FRAMEBUFFER

#if ENABLE_MULTISAMPLE_FRAMEBUFFER
   glBindTexture(
      GL_TEXTURE_2D,
      color_buffer_resolve);

   glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_RGBA8,
      GetSize().width,
      GetSize().height,
      0,
      GL_RGBA,
      GL_UNSIGNED_BYTE,
      nullptr);
   glTexParameteri(
      GL_TEXTURE_2D,
      GL_TEXTURE_MIN_FILTER,
      GL_NEAREST);
   glTexParameteri(
      GL_TEXTURE_2D,
      GL_TEXTURE_MAG_FILTER,
      GL_NEAREST);

   glBindTexture(
      GL_TEXTURE_2D,
      0);
#endif // ENABLE_MULTISAMPLE_FRAMEBUFFER

   glBindTexture(
      texture_target,
      0);
}

void MultisampleFramebufferWindow::RenderScene( )
{
   glBindFramebuffer(
      GL_FRAMEBUFFER,
      frame_buffer[current_frame]);

   glDrawBuffer(
      GL_COLOR_ATTACHMENT0);

   const float clear_color[] {
      0.0f, 0.0f, 0.0f, 1.0f
   };

   glClearBufferfv(
      GL_COLOR,
      0,
      clear_color);
   glClearBufferfi(
      GL_DEPTH_STENCIL,
      0,
      1.0f,
      0x00);

   glEnable(GL_DEPTH_TEST);

#if ENABLE_MULTISAMPLE_FRAMEBUFFER
   glEnable(GL_MULTISAMPLE);
#endif // ENABLE_MULTISAMPLE_FRAMEBUFFER

   const auto size =
      GetSize();

   glViewport(
      0, 0,
      size.width,
      size.height);

   const double half_width =
      size.width / 2.0;
   const double half_height =
      size.height / 2.0;

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(
      -half_width, half_width,
      -half_height, half_height,
      -1.0, 1.0);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   const double vertices[] {
      -half_height, -half_height, 0.0,
       half_height,  half_height, 0.0,
      -half_height,  half_height, 0.0,
      -half_height, -half_height, 0.0,
       half_height, -half_height, 0.0,
       half_height,  half_height, 0.0
   };

   glEnableClientState(GL_VERTEX_ARRAY);

   const float colors_scales_distances[][6] {
      { 1.0f, 0.0f, 0.0f, 1.0f, 0.9f, -0.9f },
      { 0.0f, 1.0f, 0.0f, 1.0f, 0.8f, -0.6f },
      { 0.0f, 0.0f, 1.0f, 1.0f, 0.7f, -0.3f },
      { 1.0f, 1.0f, 0.0f, 1.0f, 0.6f,  0.3f },
      { 1.0f, 0.0f, 1.0f, 1.0f, 0.5f,  0.6f },
      { 0.0f, 1.0f, 1.0f, 1.0f, 0.4f,  0.9f }
   };

   static thread_local float degrees_rates[][2] {
      { 0.0f, 0.8f },
      { 0.0f, 0.7f },
      { 0.0f, 0.6f },
      { 0.0f, 0.5f },
      { 0.0f, 0.4f },
      { 0.0f, 0.3f }
   };

   for (const auto color_scale_dist :
        colors_scales_distances)
   {
      glPushMatrix();

      const auto index =
         std::distance(
            colors_scales_distances,
            reinterpret_cast< const float (*)[6] >(
               color_scale_dist));

      glTranslatef(
         0.0f, 0.0f, color_scale_dist[5]);
      glRotatef(
         degrees_rates[index][0] +=
            degrees_rates[index][1],
         0.0f, 0.0f, 1.0f);
      glScalef(
         color_scale_dist[4],
         color_scale_dist[4],
         1.0f);

      glColor4fv(
         color_scale_dist);

      glVertexPointer(
         3,
         GL_DOUBLE,
         3 * sizeof(double),
         vertices);

      glDrawArrays(
         GL_LINE_STRIP,
         0,
         6);

      glPopMatrix();
   }

   glDisableClientState(GL_VERTEX_ARRAY);

   glDisable(GL_DEPTH_TEST);

#if ENABLE_MULTISAMPLE_FRAMEBUFFER
   glDisable(GL_MULTISAMPLE);

   glBindFramebuffer(
      GL_READ_FRAMEBUFFER,
      frame_buffer[current_frame]);
   glBindFramebuffer(
      GL_DRAW_FRAMEBUFFER,
      frame_buffer_resolve);

   glBlitFramebuffer(
      0, 0, size.width, size.height,
      0, 0, size.width, size.height,
      GL_COLOR_BUFFER_BIT,
      GL_NEAREST);
#endif // ENABLE_MULTISAMPLE_FRAMEBUFFER

   glBindFramebuffer(
      GL_FRAMEBUFFER,
      0);
}

void MultisampleFramebufferWindow::RenderColorBuffer( )
{
   glDrawBuffer(
      GL_BACK);

   glViewport(
      0, 0,
      GetSize().width,
      GetSize().height);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   const float vertices[] {
      -1.0f, -1.0f, 0.0f,
       1.0f,  1.0f, 0.0f,
      -1.0f,  1.0f, 0.0f,

      -1.0f, -1.0f, 0.0f,
       1.0f, -1.0f, 0.0f,
       1.0f,  1.0f, 0.0f,
   };

   const float tcoords[] {
      0.0f, 0.0f,
      1.0f, 1.0f,
      0.0f, 1.0f,

      0.0f, 0.0f,
      1.0f, 0.0f,
      1.0f, 1.0f
   };

   glBindTexture(
      GL_TEXTURE_2D,
#if ENABLE_MULTISAMPLE_FRAMEBUFFER
      color_buffer_resolve);
#else
      color_buffer[current_frame]);
#endif

   GLint cur_tex_env_mode { };
   glGetTexEnviv(
      GL_TEXTURE_ENV,
      GL_TEXTURE_ENV_MODE,
      &cur_tex_env_mode);

   glTexEnvi(
      GL_TEXTURE_ENV,
      GL_TEXTURE_ENV_MODE,
      GL_REPLACE);

   glEnable(GL_TEXTURE_2D);
   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);

   glVertexPointer(
      3,
      GL_FLOAT,
      3 * sizeof(float),
      vertices);
   glTexCoordPointer(
      2,
      GL_FLOAT,
      2 * sizeof(float),
      tcoords);

   glDrawArrays(
      GL_TRIANGLES,
      0,
      6);

   glDisable(GL_TEXTURE_2D);
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);

   glTexEnvi(
      GL_TEXTURE_ENV,
      GL_TEXTURE_ENV_MODE,
      cur_tex_env_mode);

   glBindTexture(
      GL_TEXTURE_2D,
      0);
}

void MultisampleFramebufferWindow::RenderThread(
   std::condition_variable & init_complete )
{
   auto shared =
      wglShareLists(
         hidden_gl_window.second,
         GetGLContext());

   const auto local_hidden_window =
      CreateHiddenWindow();

   shared =
      wglShareLists(
         hidden_gl_window.second,
         local_hidden_window.second);

   wglMakeCurrent(
      GetDC(local_hidden_window.first),
      local_hidden_window.second);

   InitGLData();

   init_complete.notify_all();

   while (!quit_render_thread)
   {
      {
         std::unique_lock lock {
            frame_mutex
         };

         frame.wait(
            lock,
            [ this ]
            {
               return
                  frame_mode == FrameMode::RENDER ||
                  quit_render_thread;
            });
      }

      current_frame =
         current_frame++ %
         std::size(frame_buffer);

      wglMakeCurrent(
         GetDC(local_hidden_window.first),
         local_hidden_window.second);

      const auto time_begin =
         std::chrono::high_resolution_clock::now();

      RenderScene();

      glFinish();

      wglMakeCurrent(
         nullptr,
         nullptr);

      const auto time_end =
         std::chrono::high_resolution_clock::now();
      
      const auto time_remaining =
         std::chrono::milliseconds(33) -
         (time_end - time_begin);

      if (time_remaining.count() > 0)
         std::this_thread::sleep_for(
            time_remaining);

      frame_mode =
         FrameMode::PRESENT;

      frame.notify_all();
   }

   for (const auto cb : color_buffer)
   {
      glDeleteTextures(
         1,
         &cb);
   }
   
   glDeleteTextures(
      1,
      &depth_buffer);

   for (const auto fb : color_buffer)
   {
      // need to delete the frame buffer here...
      // frame buffers cannot be shared between contexts...
      glDeleteFramebuffers(
         1,
         &fb);
   }

#if ENABLE_MULTISAMPLE_FRAMEBUFFER
   glDeleteTextures(
      1,
      &color_buffer_resolve);

   glDeleteFramebuffers(
      1,
      &frame_buffer_resolve);
#endif

   wglMakeCurrent(
      nullptr,
      nullptr);

   auto deleted =
      wglDeleteContext(
         local_hidden_window.second);

   deleted =
      DeleteDC(
         GetDC(local_hidden_window.first));

   deleted =
      DestroyWindow(
         local_hidden_window.first);
}
