// platform includes
#include <windows.h>

// local includes
#include "AllocConsole.h"
#include "AutoScaleWindow.h"

int WINAPI WinMain( HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine,
                    int nShowCmd )
{
   // indicates success or failure
   int error = -1;

   // allocate a console for the application
   AllocateDebugConsole();

   // create a new tesselation window
   AutoScaleWindow * const pASW = new AutoScaleWindow();
   if (pASW->Create(540, 540, "Auto Scale Window"))
   {
      // run the program
      error = pASW->Run();
   }

   return error;
}
