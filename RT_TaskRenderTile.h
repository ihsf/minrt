#ifndef _RT_TASKRENDERTILE_H
#define _RT_TASKRENDERTILE_H

#include <math.h>			
#include <stdio.h>	
#include "CVector2i.h"
#include "CVector3.h"
#include "Engine.h"
#include "RT_Task.h"
#include "RT_FrameBuffer.h"
#include "RT_Camera.h"
#include "RT_RayQuery.h"
#include "EmbreeStuff.h"

using namespace std;

class RT_TaskRenderTile : public RT_Task {
  public:
    RT_TaskRenderTile(CVector2i* startPixel_, RT_Camera* camera_, RT_FrameBuffer* frameBuffer_, int tileNumber_);
    ~RT_TaskRenderTile(){};

    void run();

  private:
    CVector2i startPixel;
    RT_Camera* camera;
    RT_FrameBuffer* frameBuffer;

    int tileNumber;

    unsigned char* fBuffer;

    int sizeX;
    int sizeXtimes4;

    finline void putColorInFrameBuffer(int x, int y, CVector3* color){
      Assert(color, "putColorInFrameBuffer");
      const int offset = x * 4 + y * sizeXtimes4;

      fBuffer[offset] = color->x * 255.0f;
      fBuffer[offset + 1] = color->y * 255.0f;
      fBuffer[offset + 2] = color->z * 255.0f;
    }
};


#endif

