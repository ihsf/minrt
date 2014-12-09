#ifndef _WINDOWSHELPER_H
#define _WINDOWSHELPER_H

#ifdef _MSC_VER
	#pragma warning( disable : 4530 ) // warning C4530: C++ exception handler used, but unwind semantics are not enabled. Specify /EHsc
#endif

#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <vector>

#ifdef _WIN32
  #include <windows.h>
  #include <winnt.h>
#else
  #include <sys/time.h>
#endif

using namespace std;

class WindowsHelper {
  public:
    WindowsHelper();
    ~WindowsHelper();

    static float getMsElapsed();
    static float getTime();
};
#endif
