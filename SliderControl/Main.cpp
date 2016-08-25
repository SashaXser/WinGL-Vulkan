// platform includes
#include <windows.h>

// local includes
#include "AllocConsole.h"
#include "SliderControlWindow.h"

int WINAPI WinMain( HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine,
                    int nShowCmd )
{
   // indicates success or failure
   int error = -1;

   // allocate a console for the application
   AllocateDebugConsole();

   // create a new slider control window
   SliderControlWindow * const pSCW = new SliderControlWindow();
   if (pSCW->Create(540, 540, "Slider Control Window"))
   {
      // run the program
      error = pSCW->Run();
   }

   return error;
}
