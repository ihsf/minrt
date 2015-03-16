#ifndef _RT_FRAMEBUFFER_H
#define _RT_FRAMEBUFFER_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "Engine.h"

using namespace std;

// only 32 bit RGBA supported
class RT_FrameBuffer {
  public:
    RT_FrameBuffer(int sizeX, int sizeY);
    ~RT_FrameBuffer();

    finline int getSizeX(){ return sizeX; }
    finline int getSizeY(){ return sizeY; }
    finline int getBytesPerPixel(){ return bytesPerPixel; }
    finline unsigned char* getFrameBuffer() { return frameBuffer; }

    void clear();

  private:
    unsigned char* frameBuffer;
    int sizeX;
    int sizeY;
    int bytesPerPixel;

    void deleteFrameBuffer();
};

#endif
