#include "RT_TaskRenderTile.h"

RT_TaskRenderTile::RT_TaskRenderTile(CVector2i* startPixel_, RT_Camera* camera_, RT_FrameBuffer* frameBuffer_, int tileNumber_){
  Assert(startPixel_, "RT_TaskRenderTile");
  this->startPixel = *startPixel_;
  this->camera = camera_;
  this->frameBuffer = frameBuffer_;
  this->tileNumber = tileNumber_;

  Assert(camera && frameBuffer, "RT_TaskRenderTile");
  Assert(tileNumber >= 0, "RT_TaskRenderTile");

  fBuffer = frameBuffer->getFrameBuffer();
}

void RT_TaskRenderTile::run(){
  sizeX = frameBuffer->getSizeX();  // saved in the class
  sizeXtimes4 = sizeX * 4;				  // saved in the class
  const int sizeY = frameBuffer->getSizeY();
  float sizeXminus1Half = ((float)sizeX - 1.0f) * 0.5f;
  float sizeYminus1Half = ((float)sizeY - 1.0f) * 0.5f;

  CVector3 color;
  CVector2i endPixel;
  if (!Engine::rectMode){
    endPixel = CVector2i(startPixel.x + sizeX, startPixel.y + Engine::RENDERLINE_SIZE);
  }
  else {
    endPixel = CVector2i(startPixel.x + Engine::rectSizeX, startPixel.y + Engine::RENDERLINE_SIZE);
  }

  if (Engine::embreeIntersect == 8){
    RT_RayQuery rayQuery[8];
    int xcoord[8];
    int ycoord[8];
    // precalculate primary ray hits with embree here
    for (int y = startPixel.y; y < endPixel.y; y += 2){
      for (int x = startPixel.x; x < endPixel.x; x += 4){
        xcoord[0] = x;
        ycoord[0] = y;
        xcoord[1] = x + 1;
        ycoord[1] = y;
        xcoord[2] = x;
        ycoord[2] = y + 1;
        xcoord[3] = x + 1;
        ycoord[3] = y + 1;
        xcoord[4] = x + 2;
        ycoord[4] = y + 1;
        xcoord[5] = x + 2;
        ycoord[5] = y;
        xcoord[6] = x + 3;
        ycoord[6] = y;
        xcoord[7] = x + 3;
        ycoord[7] = y + 1;

        int accessToArray[8];
        for (int i = 0; i < 8; i++){
          accessToArray[i] = xcoord[i] + Engine::screenWidthRT * ycoord[i];
        }

        for (int i = 0; i < 8; i++){
          rayQuery[i].getShadingInfoPointer()->reset();
          camera->setupRayQuery(xcoord[i], ycoord[i], sizeXminus1Half, sizeYminus1Half, &rayQuery[i]);
        }

        EmbreeStuff::shootRay8(rayQuery);

        for (int i = 0; i < 8; i++){
          RT_ShadingInfo* shadingInfoPointer = rayQuery[i].getShadingInfoPointer();

          if (shadingInfoPointer->objectThatGotHit >= 0){
            if (shadingInfoPointer->objectThatGotHit == 1){  // some colored shading for the floor              
              color = CVector3(0.0f + (rayQuery[i].getDirectionPointer()->x / 3.0f), 0.4f + (rayQuery[i].getDirectionPointer()->z / 3.0f), 0.8f);
            }
            else if (shadingInfoPointer->objectThatGotHit == 2){   // bunny              
              CVector3 normal = CVector3(meshNormals[shadingInfoPointer->primIdThatGotHit * 4], meshNormals[shadingInfoPointer->primIdThatGotHit * 4 + 1],
                meshNormals[shadingInfoPointer->primIdThatGotHit * 4 + 2]);

              bool backFacingGeometryNormal = (rayQuery[i].getDirectionPointer()->dotProduct(&normal) < 0);
              if (backFacingGeometryNormal){
                normal = -normal;
              }

              const float gray = rayQuery[i].getDirectionPointer()->dotProduct(&normal);
              color = CVector3(gray, gray, gray);
            }
            else {
              // some colored shading for the walls
              CVector3 hitPoint = rayQuery[i].getOrigin() + shadingInfoPointer->t * rayQuery[i].getDirection();
              color = CVector3(rayQuery[i].getDirectionPointer()->z, rayQuery[i].getDirectionPointer()->x, hitPoint.y / 32.0f);
            }

            color.clamp();
          }
          else {
            color = CVector3(0.1f, 0.1f, 0.2f); // environment background color
          }

          if (Engine::debugLatency){
            if (Engine::debugNumCameraChanges % 2 == 1){
              color += CVector3(0.0f, 0.6f, 0.0f);
            }
            else {
              color += CVector3(0.6f, 0.0f, 0.0f);
            }

            if (Engine::accelerometerChangedThisFrame){
              color += CVector3(0.0f, 0.0f, 0.6f);
            }

            color.clamp();
          }

          putColorInFrameBuffer(xcoord[i], ycoord[i], &color);
        }
      }
    }
  }
  else if (Engine::embreeIntersect == 4){
    RT_RayQuery rayQuery[4];
    int xcoord[4];
    int ycoord[4];
    // precalculate primary ray hits with embree here
    for (int y = startPixel.y; y < endPixel.y; y += 2){
      for (int x = startPixel.x; x < endPixel.x; x += 2){
        xcoord[0] = x;
        ycoord[0] = y;
        xcoord[1] = x + 1;
        ycoord[1] = y;
        xcoord[2] = x;
        ycoord[2] = y + 1;
        xcoord[3] = x + 1;
        ycoord[3] = y + 1;

        int accessToArray[4];
        for (int i = 0; i < 4; i++){
          accessToArray[i] = xcoord[i] + Engine::screenWidthRT * ycoord[i];
        }

        for (int i = 0; i < 4; i++){
          rayQuery[i].getShadingInfoPointer()->reset();
          camera->setupRayQuery(xcoord[i], ycoord[i], sizeXminus1Half, sizeYminus1Half, &rayQuery[i]);
        }

        EmbreeStuff::shootRay4(rayQuery);

        for (int i = 0; i < 4; i++){
          RT_ShadingInfo* shadingInfoPointer = rayQuery[i].getShadingInfoPointer();

          if (shadingInfoPointer->objectThatGotHit >= 0){
            if (shadingInfoPointer->objectThatGotHit == 1){  // some colored shading for the floor
              color = CVector3(0.0f + (rayQuery[i].getDirectionPointer()->x / 3.0f), 0.4f + (rayQuery[i].getDirectionPointer()->z / 3.0f), 0.8f);
            }
            else if (shadingInfoPointer->objectThatGotHit == 2){ // bunny
              CVector3 normal = CVector3(meshNormals[shadingInfoPointer->primIdThatGotHit * 4], meshNormals[shadingInfoPointer->primIdThatGotHit * 4 + 1],
                meshNormals[shadingInfoPointer->primIdThatGotHit * 4 + 2]);

              bool backFacingGeometryNormal = (rayQuery[i].getDirectionPointer()->dotProduct(&normal) < 0);
              if (backFacingGeometryNormal){
                normal = -normal;
              }

              const float gray = rayQuery[i].getDirectionPointer()->dotProduct(&normal);
              color = CVector3(gray, gray, gray);
            }
            else {
              // some colored shading for the walls
              CVector3 hitPoint = rayQuery[i].getOrigin() + shadingInfoPointer->t * rayQuery[i].getDirection();
              color = CVector3(rayQuery[i].getDirectionPointer()->z, rayQuery[i].getDirectionPointer()->x, hitPoint.y / 32.0f);
            }

            color.clamp();
          }
          else {
            color = CVector3(0.1f, 0.1f, 0.2f); // environment background color
          }

          if (Engine::debugLatency){
            if (Engine::debugNumCameraChanges % 2 == 1){
              color += CVector3(0.0f, 0.6f, 0.0f);
            }
            else {
              color += CVector3(0.6f, 0.0f, 0.0f);
            }

            color.clamp();
          }

          putColorInFrameBuffer(xcoord[i], ycoord[i], &color);
        }
      }
    }
  }
}







