#ifndef _RT_SHADINGINFO_H
#define _RT_SHADINGINFO_H

#include <math.h>			
#include <stdio.h>	
#include "CVector3.h"
#include "Engine.h"

using namespace std;

class RT_ShadingInfo {
  public:
    finline RT_ShadingInfo(){
      reset();
    }

    float hitBeta;  // barycentric coords
    float hitGamma;

    float t;

    int objectThatGotHit;
    int primIdThatGotHit;

    finline void reset(){
      objectThatGotHit = -1;
      primIdThatGotHit = -1;
    }

  private:

};

#endif
