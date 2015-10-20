// platform includes
#include <windows.h>

// local includes
#include "TransformFeedbackWindow.h"

int WINAPI WinMain( HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine,
                    int nShowCmd )
{
   // indicates success or failure
   int error = -1;

   // create the window
   TransformFeedbackWindow * const pTFW = new TransformFeedbackWindow();
   if (pTFW->Create(100, 100, "Transform Feedback Window"))
   {
      // begin displaying the window
      error = pTFW->Run();
   }

   return error;
}
