// local includes
#include "WglAssert.h"

// platform includes
#include <windows.h>

#ifdef _DEBUG

// assert function that brings up a message box and
// displays the assert...
int __WglAssertMsgBox( const char * const pCaption,
                       const char * const pMessage )
{
   // display the message box
   int btnPress = MessageBox(NULL,
                             pMessage, pCaption,
                             MB_YESNOCANCEL | MB_ICONSTOP);

   // return the type of button pressed
   switch (btnPress)
   {
   case IDYES: btnPress = WGL_YES; break;
   case IDNO: btnPress = WGL_NO; break;
   case IDCANCEL: btnPress = WGL_CANCEL; break;
   }

   return btnPress;
}

#endif // _DEBUG

// function to bring up a message box
int WglMsgBox( const char * const pCaption, const char * const pMessage )
{
   return MessageBox(NULL, pMessage, pCaption, MB_OK | MB_ICONINFORMATION);
}
