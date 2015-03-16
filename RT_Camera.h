#ifndef _RT_CAMERA_H
#define _RT_CAMERA_H

#include <math.h>			
#include <stdio.h>	
#include "Engine.h"
#include "RT_RayQuery.h"

using namespace std;

class RT_Camera {
  public:
    RT_Camera();
    ~RT_Camera();

    void computeCameraCoordinateSystem();
    void setupRayQuery(int x, int y, float sizeXminus1Half, float sizeYminus1Half, RT_RayQuery* rayQuery);

    void setPosition(const CVector3* p){ Assert(p, "setPosition p"); position.x = p->x; position.y = p->y; position.z = p->z; }
    void setPosition(float x, float y, float z){ position.x = x; position.y = y; position.z = z; }
    void setLookAt(const CVector3* p){ Assert(p, "setLookAt p"); lookAt.x = p->x; lookAt.y = p->y; lookAt.z = p->z; }
    void setLookAt(float x, float y, float z){ lookAt.x = x; lookAt.y = y; lookAt.z = z; }
    void setUpVector(const CVector3* u_){ Assert(u_, "setUpVector u"); up.x = u_->x; up.y = u_->y; up.z = u_->z; }
    void setUpVector(float x, float y, float z){ up.x = x; up.y = y; up.z = z; }

    void setViewDistance(float viewDistance_){ viewDistance = viewDistance_; }

  protected:
    CVector3 position;
    CVector3 lookAt;
    CVector3 up;
    CVector3 u;
    CVector3 v;
    CVector3 w;

    float viewDistance;
};

#endif
