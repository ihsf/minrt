#ifndef _EMBREESTUFF_H
#define _EMBREESTUFF_H

#include <iostream>
#include <vector>
#include <embree2/rtcore.h>
#include <embree2/rtcore_ray.h>
#include "RT_RayQuery.h"
#include "Engine.h"
#include "StanfordBunny.h"

#ifdef _MSC_VER
#pragma warning( disable : 4996 ) // '*' was declared deprecated
#endif

using namespace std;

class EmbreeStuff {
  public:
    EmbreeStuff();
    ~EmbreeStuff();

    static void init();

    static finline void shootRay4(RT_RayQuery rayQuery[4]){
      __align(16)RTCRay4 ray;

      CVector3* originPointer;
      CVector3* directionPointer;

      for (int i = 0; i < 4; i++){
        originPointer = rayQuery[i].getOriginPointer();
        ray.orgx[i] = originPointer->x;
        ray.orgy[i] = originPointer->y;
        ray.orgz[i] = originPointer->z;

        directionPointer = rayQuery[i].getDirectionPointer();
        ray.dirx[i] = directionPointer->x;
        ray.diry[i] = directionPointer->y;
        ray.dirz[i] = directionPointer->z;

        ray.tnear[i] = 0.0f;
        ray.tfar[i] = DEFAULT_RAY_MAX_DISTANCE;
        ray.geomID[i] = RTC_INVALID_GEOMETRY_ID;
        ray.primID[i] = RTC_INVALID_GEOMETRY_ID;
        ray.instID[i] = RTC_INVALID_GEOMETRY_ID;
        ray.mask[i] = 0xFFFFFFFF;
        ray.time[i] = 0.0f;
      }

      __align(16) int valid[4] = { -1, -1, -1, -1 };
      rtcIntersect4(valid, scene, ray);

      for (int i = 0; i < 4; i++){
        RT_ShadingInfo* shadingInfoPointer = rayQuery[i].getShadingInfoPointer();

        if (ray.geomID[i] == RTC_INVALID_GEOMETRY_ID){
          continue;
        }

        shadingInfoPointer->primIdThatGotHit = ray.primID[i];
        shadingInfoPointer->objectThatGotHit = ray.geomID[i];
        shadingInfoPointer->hitBeta = ray.u[i];
        shadingInfoPointer->hitGamma = ray.v[i];
        shadingInfoPointer->t = ray.tfar[i];
      }
    }

    static finline void shootRay8(RT_RayQuery rayQuery[8]){
      __align(32)RTCRay8 ray;

      CVector3* originPointer;
      CVector3* directionPointer;

      for (int i = 0; i < 8; i++){
        originPointer = rayQuery[i].getOriginPointer();
        ray.orgx[i] = originPointer->x;
        ray.orgy[i] = originPointer->y;
        ray.orgz[i] = originPointer->z;

        directionPointer = rayQuery[i].getDirectionPointer();
        ray.dirx[i] = directionPointer->x;
        ray.diry[i] = directionPointer->y;
        ray.dirz[i] = directionPointer->z;

        ray.tnear[i] = 0.0f;
        ray.tfar[i] = DEFAULT_RAY_MAX_DISTANCE; ray.geomID[i] = RTC_INVALID_GEOMETRY_ID;
        ray.primID[i] = RTC_INVALID_GEOMETRY_ID;
        ray.instID[i] = RTC_INVALID_GEOMETRY_ID;
        ray.mask[i] = 0xFFFFFFFF;
        ray.time[i] = 0.0f;
      }

      __align(32) int valid[8] = { -1, -1, -1, -1, -1, -1, -1, -1 };
      rtcIntersect8(valid, scene, ray);

      for (int i = 0; i < 8; i++){
        RT_ShadingInfo* shadingInfoPointer = rayQuery[i].getShadingInfoPointer();

        if (ray.geomID[i] == RTC_INVALID_GEOMETRY_ID){
          continue;
        }

        shadingInfoPointer->primIdThatGotHit = ray.primID[i];
        shadingInfoPointer->objectThatGotHit = ray.geomID[i];
        shadingInfoPointer->hitBeta = ray.u[i];
        shadingInfoPointer->hitGamma = ray.v[i];
        shadingInfoPointer->t = ray.tfar[i];
      }
    }

    static void compile();

    static void addSampleMesh1();
    static void addSampleMesh2();
    static void addSampleMesh3();

    static __align(32)RTCScene scene;

  private:
    struct Vertex   { float x, y, z, a; };
    struct Triangle { int v0, v1, v2; };

    static bool compiledOnce;
};

#endif