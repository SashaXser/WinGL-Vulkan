// platform includes
#include <windows.h>

// local includes
#include "AllocConsole.h"
#include "MultisampleFramebufferWindow.h"

#include <condition_variable>
#include <mutex>
#include <thread>

int WINAPI WinMain( HINSTANCE /*hInstance*/,
                    HINSTANCE /*hPrevInstance*/,
                    LPSTR /*lpCmdLine*/,
                    int /*nShowCmd*/ )
{
   // indicates success or failure
   int error = -1;

   // allocate a console for the application
   AllocateDebugConsole();

   // create a new tesselation window
   auto * const pMFW = new MultisampleFramebufferWindow();
   if (pMFW->Create(640, 480, "Multisample Framebuffer Window"))
   {
      bool init_complete { false };
      std::mutex init_complete_mutex;
      std::condition_variable init_complete_cv;

      std::thread thread {
         [ & ] ( )
         {
            auto * const pMFW = new MultisampleFramebufferWindow();
            if (pMFW->Create(640, 480, "MFW"))
            {
               {
                  std::unique_lock lock { init_complete_mutex };

                  init_complete = true;
               }

               init_complete_cv.notify_all();

               pMFW->Run();
            }
         }
      };

      {
         std::unique_lock lock { init_complete_mutex };

         init_complete_cv.wait(
            lock,
            [ & ] ( ) { return init_complete; });
      }

      // run the program
      error = pMFW->Run();

      thread.join();
   }

   return error;
}
