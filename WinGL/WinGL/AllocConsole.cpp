// local includes
#include "AllocConsole.h"

// platform includes
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <windows.h>

// stl includes
#include <fstream>
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

      // obtain the standard handles...
      const HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
      const HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);
      const HANDLE hStdErr = GetStdHandle(STD_ERROR_HANDLE);

      // open a crt file descriptor and assign it to the os...
      const int hConOut = _open_osfhandle(reinterpret_cast< intptr_t >(hStdOut), _O_TEXT);
      const int hConIn = _open_osfhandle(reinterpret_cast< intptr_t >(hStdIn), _O_TEXT);
      const int hConErr = _open_osfhandle(reinterpret_cast< intptr_t >(hStdErr), _O_TEXT);

      // open the crt file descriptors for reading and writing...
      const FILE * const pStdOut = _fdopen(hConOut, "w");
      const FILE * const pStdIn = _fdopen(hConIn, "r");
      const FILE * const pStdErr = _fdopen(hConErr, "w");

      // update the standard file streams
      *stdout = *pStdOut;
      *stdin = *pStdIn;
      *stderr = *pStdErr;

      // modify the stream buffer and buffering sizes
      setvbuf(stdout, NULL, _IONBF, 0);
      setvbuf(stdin, NULL, _IONBF, 0);
      setvbuf(stderr, NULL, _IONBF, 0);

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