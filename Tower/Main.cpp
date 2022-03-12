// platform includes
#include <windows.h>

// local includes
#include "TowerWindow.h"

int WINAPI WinMain( HINSTANCE /*hInstance*/,
                    HINSTANCE /*hPrevInstance*/,
                    LPSTR /*lpCmdLine*/,
                    int /*nShowCmd*/ )
{
   // indicates success or failure
   int error = -1;

   // create the window
   TowerWindow * const pTW = new TowerWindow();
   if (pTW->Create(400, 400, "Tower Window"))
   {
      // begin displaying the window
      error = pTW->Run();
   }

   return error;
}
