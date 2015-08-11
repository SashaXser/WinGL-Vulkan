// local includes
#include "AllocConsole.h"

// platform includes
#include <windows.h>

// stl includes
#include <cstdio>
#include <iostream>

// global static data
static bool gDebugConsoleAllocated = false;

// allocates a debug console
bool AllocateDebugConsole( )
{
   // defines the number of lines for the console
   static const unsigned short MAX_CONSOLE_LINES = 9999;

   if (!gDebugConsoleAllocated)
   {
      // allocate the console...
      AllocConsole();

      // set the console title...
      SetConsoleTitle("WinGL Debug Output Console");

      // obtain the current console info...
      CONSOLE_SCREEN_BUFFER_INFO conInfo;
      GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &conInfo);

      // update the maximum number of lines...
      conInfo.dwSize.Y = MAX_CONSOLE_LINES;
      SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), conInfo.dwSize);

      // redirect the std files to the console
      freopen("CONOUT$", "w", stdout);
      freopen("CONIN$", "r", stdin);
      freopen("CONOUT$", "w", stderr);

      // make sure that iostream and crt library operations occur
      // in the order that they appear in source code
      std::ios::sync_with_stdio();

      // clear any errors on the standard streams
      std::cout.clear();
      std::cin.clear();
      std::cerr.clear();

      // console has been allocated...
      gDebugConsoleAllocated = true;
   }

   return gDebugConsoleAllocated;
}

// determines if a debug console is attached
bool DebugConsoleIsAttached( )
{
   return gDebugConsoleAllocated;
}