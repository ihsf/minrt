#ifndef _RT_RAYQUERY_H
#define _RT_RAYQUERY_H

#include <math.h>			
#include <stdio.h>		
#include "Engine.h"
#include "RT_ShadingInfo.h"
#include "CVector2.h"

using namespace std;

class RT_RayQuery {
public:
    finline RT_RayQuery(){};
    finline RT_RayQuery(CVector3* origin_, CVector3* direction_, float maxDistance_){
      Assert(origin_ && direction_ && maxDistance_ > 0.0f, "RT_RayQuery");
      origin = *origin_;
      direction = *direction_;
    }

    finline void setOrigin(CVector3* origin_){
      Assert(origin_, "setOrigin");
      origin = *origin_;
    }

    finline void setDirection(CVector3* direction_){
      Assert(direction_, "setDirection");
      direction = *direction_;
    }

    finline CVector3 getOrigin(){ return origin; }
    finline CVector3 getDirection(){ return direction; }

    finline CVector3* getOriginPointer(){ return &origin; }
    finline CVector3* getDirectionPointer(){ return &direction; }

    finline RT_ShadingInfo* getShadingInfoPointer(){ return &shadingInfo; }

  private:
    CVector3 origin;
    CVector3 direction;
    RT_ShadingInfo shadingInfo;
};

#endif
