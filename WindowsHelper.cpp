#include "WindowsHelper.h"

WindowsHelper::WindowsHelper(){
}

WindowsHelper::~WindowsHelper(){
}

#ifndef WIN32
  struct timeval last_tv;
#endif

float WindowsHelper::getMsElapsed() {
#ifdef _WIN32
  static ULONGLONG prevCount = 0;
  static float freq = 0;
  ULONGLONG count = 0;

  float delta = 0.0f;

  if (freq == 0.0f){
    LARGE_INTEGER iFreq;
    QueryPerformanceFrequency(&iFreq);

    freq = float(iFreq.QuadPart) / 1000.0f;
  }

  QueryPerformanceCounter((LARGE_INTEGER*)&count);

  if (prevCount){
    delta = float(count - prevCount) / freq;
  }

  prevCount = count;

  return delta;
#else
  struct timeval tv;
  gettimeofday(&tv,NULL);
  double secs = (tv.tv_sec - last_tv.tv_sec) + 1e-6 * (tv.tv_usec - last_tv.tv_usec);
  // convert to ms
  secs *= 1000.0f;

  last_tv = tv;	

  if(secs > 0.00001f)
    return secs;
  else 
    return 0.001f;
#endif
}
