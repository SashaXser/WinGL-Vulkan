// platform includes
#include <windows.h>

// local includes
#include "PlanetsWindow.h"

int WINAPI WinMain( HINSTANCE /*hInstance*/,
                    HINSTANCE /*hPrevInstance*/,
                    LPSTR /*lpCmdLine*/,
                    int /*nShowCmd*/ )
{
   // indicates success or failure
   int error = -1;

   // create the window
   PlanetsWindow * const pPW = new PlanetsWindow();
   if (pPW->Create(400, 400, "Planets Window"))
   {
      // begin displaying the window
      error = pPW->Run();
   }

   return error;
}
