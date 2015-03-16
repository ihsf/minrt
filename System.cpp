// https://bitbucket.org/wolfpld/etcpak
// Bartosz Taudul <wolf.pld@gmail.com>

#ifdef _WIN32
#  include <windows.h>
#else
#  include <pthread.h>
#  include <unistd.h>
#endif

#include "System.hpp"

int System::CPUCores()
{
  static int cores = 0;
  if (cores == 0)
  {
#ifdef _WIN32
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    cores = (int)info.dwNumberOfProcessors;
#else
#  ifndef _SC_NPROCESSORS_ONLN
#    ifdef _SC_NPROC_ONLN
#      define _SC_NPROCESSORS_ONLN _SC_NPROC_ONLN
#    elif defined _SC_CRAY_NCPU
#      define _SC_NPROCESSORS_ONLN _SC_CRAY_NCPU
#    endif
#  endif
    cores = (int)(long)sysconf( _SC_NPROCESSORS_ONLN );
#endif
    if (cores <= 0)
    {
      cores = 1;
    }
  }
  return cores;
}
