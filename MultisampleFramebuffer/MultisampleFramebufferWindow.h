#ifndef _MULTISAMPLE_FRAMEBUFFER_WINDOW_H_
#define _MULTISAMPLE_FRAMEBUFFER_WINDOW_H_

// local includes
#include "OpenGLWindow.h"

// wingl includes

// gl includes
#include <gl/glew.h>
#include <gl/GL.h>

// std includes
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <thread>

#define ENABLE_MULTISAMPLE_FRAMEBUFFER 1

class MultisampleFramebufferWindow :
   public OpenGLWindow
{
public:
   // constructor
   // destructor is not public to make sure
   // that the class must be destroyed internally
   // through message WM_NCDESTROY...
   MultisampleFramebufferWindow( );

   // creates the application
   virtual bool Create(
      unsigned int nWidth,
      unsigned int nHeight,
      const char * pWndTitle,
      const void * pInitParams = nullptr );

   // basic run implementation
   // will process messages until app quit
   virtual int Run( );
   
protected:
   // destructor
   virtual ~MultisampleFramebufferWindow( );

   // called when the window is about to be destroyed
   virtual void OnDestroy( ) override;

   // handles messages passed by the system
   virtual LRESULT MessageHandler(
      UINT uMsg,
      WPARAM wParam,
      LPARAM lParam ) override;

private:
   // intializes all the gl attributes
   void InitGLData( );
   void InitTextures( );

   void RenderScene( );
   void RenderColorBuffer( );

   void RenderThread(
      std::condition_variable & init_complete );

   size_t current_frame;
   GLuint frame_buffer[2];
   GLuint color_buffer[2];
   GLuint depth_buffer;

#if ENABLE_MULTISAMPLE_FRAMEBUFFER
   GLuint frame_buffer_resolve;
   GLuint color_buffer_resolve;
#endif

   std::thread render_thread;
   std::atomic_bool quit_render_thread;

   enum class FrameMode : uint32_t
   {
      RENDER, PRESENT
   };

   FrameMode frame_mode;
   std::mutex frame_mutex;
   std::condition_variable frame;

};

#endif // _MULTISAMPLE_FRAMEBUFFER_WINDOW_H_
