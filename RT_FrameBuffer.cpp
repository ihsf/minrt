#include "RT_FrameBuffer.h"

RT_FrameBuffer::RT_FrameBuffer(int sizeX_, int sizeY_){
  frameBuffer = NULL;
  this->sizeX = sizeX_;
  this->sizeY = sizeY_;
  bytesPerPixel = 4;

  Assert(sizeX > 0 && sizeY > 0, "RT_FrameBuffer");

  const size_t alignedBy = 64;
  frameBuffer = (unsigned char*)_mm_malloc((size_t)(sizeX * sizeY * bytesPerPixel), alignedBy);

  memset(frameBuffer, 255, (size_t)(sizeX * sizeY * bytesPerPixel));
}

RT_FrameBuffer::~RT_FrameBuffer(){
  deleteFrameBuffer();
}

void RT_FrameBuffer::deleteFrameBuffer(){
  _mm_free(frameBuffer);
  frameBuffer = NULL;
}

void RT_FrameBuffer::clear(){
  if (!frameBuffer)
    return;

  for (int i = 0; i < sizeX * sizeY * 4; i++){
    frameBuffer[i] = 0;
  }
}
