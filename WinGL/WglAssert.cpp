// local includes
#include "WglAssert.h"

// platform includes
#include <windows.h>
#include <process.h>

// message data for the threaded assert
struct MsgBoxParams
{
   const char * pCaption;
   const char * pMessage;
};

#ifdef _DEBUG

// threaded assert message box implementation
unsigned __stdcall WglThreadedAssertMsgBox( void * pMsg )
{
   // display the message box
   int btnPress = MessageBox(NULL,
                             reinterpret_cast< const MsgBoxParams * >(pMsg)->pMessage,
                             reinterpret_cast< const MsgBoxParams * >(pMsg)->pCaption,
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

// assert function that brings up a message box and
// displays the assert...
int __WglAssertMsgBox( const char * const pCaption,
                       const char * const pMessage )
{
   MsgBoxParams msg =
   {
      pCaption, pMessage
   };

   const uintptr_t handle =
      _beginthreadex(nullptr, 0, &WglThreadedAssertMsgBox, reinterpret_cast< void * >(&msg), 0, nullptr);

   WaitForSingleObject(reinterpret_cast< HANDLE >(handle), INFINITE);

   unsigned long btn_pressed = WGL_CANCEL;
   GetExitCodeThread(reinterpret_cast< HANDLE >(handle), &btn_pressed);

   CloseHandle(reinterpret_cast< HANDLE >(handle));

   return btn_pressed;
}

#endif // _DEBUG

// threaded message box implementation
unsigned __stdcall WglThreadedMsgBox( void * pMsg )
{
   // display the message box
   int btnPress = MessageBox(NULL,
                             reinterpret_cast< const MsgBoxParams * >(pMsg)->pMessage,
                             reinterpret_cast< const MsgBoxParams * >(pMsg)->pCaption,
                             MB_OK | MB_ICONINFORMATION);

   return btnPress;
}

// function to bring up a message box
int WglMsgBox( const char * const pCaption, const char * const pMessage )
{
   MsgBoxParams msg =
   {
      pCaption, pMessage
   };

   const uintptr_t handle =
      _beginthreadex(nullptr, 0, &WglThreadedMsgBox, reinterpret_cast< void * >(&msg), 0, nullptr);

   WaitForSingleObject(reinterpret_cast< HANDLE >(handle), INFINITE);

   unsigned long btn_pressed = IDOK;
   GetExitCodeThread(reinterpret_cast< HANDLE >(handle), &btn_pressed);

   CloseHandle(reinterpret_cast< HANDLE >(handle));

   return btn_pressed;
}
