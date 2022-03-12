// platform includes
#include <windows.h>

// local includes
#include "AllocConsole.h"
#include "NormalMappingWindow.h"

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
   NormalMappingWindow * const pNMW = new NormalMappingWindow();
   if (pNMW->Create(540, 540, "Normal Mapping Window"))
   {
      // run the program
      error = pNMW->Run();
   }

   return error;
}
