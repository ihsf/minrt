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

  // Count number of ms per frame.
  if (freq == 0.0f){
    LARGE_INTEGER iFreq;
    QueryPerformanceFrequency(&iFreq);

    // iFreq cycles / sec * 1 sec / 1000 ms = cycles / ms
    freq = float(iFreq.QuadPart) / 1000.0f;
  }

  // Get elapsed time
  QueryPerformanceCounter((LARGE_INTEGER*)&count);

  // Update color values.
  if (prevCount){
    // Delta is a floating pointer number of 16ms "frames" elapsed since
    // the last draw.  The DeltaColor values are based on 16ms updates.
    delta = float(count - prevCount) / freq;
  }

  // Update previous time
  prevCount = count;

  return delta;
#else
  struct timeval tv;
  gettimeofday(&tv,NULL);
  double secs = (tv.tv_sec - last_tv.tv_sec) + 1e-6 * (tv.tv_usec - last_tv.tv_usec);
	// make it ms
	secs *= 1000.0f;

  last_tv = tv;	

	if(secs > 0.00001f)
		return secs;
	else 
		return 0.001f;
#endif
}
