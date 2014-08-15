// platform includes
#include <windows.h>

// local includes
#include "ShadowMapWindow.h"

int WINAPI WinMain( HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine,
                    int nShowCmd )
{
   // indicates success or failure
   int error = -1;

   // create the window
   ShadowMapWindow * const pSMW = new ShadowMapWindow();
   if (pSMW->Create(200, 200, "Shadow Map Window"))
   {
      // begin displaying the window
      error = pSMW->Run();
   }

   return error;
}