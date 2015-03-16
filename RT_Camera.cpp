#include "RT_Camera.h"

RT_Camera::RT_Camera(){
  position = NULLVECTOR3f;
  lookAt = VECTOR1_0_0;
  up = VECTOR0_1_0;

  u = VECTOR1_0_0;
  v = VECTOR0_1_0;
  w = VECTOR0_0_1;

  viewDistance = 0.0f;
}

RT_Camera::~RT_Camera(){
}

void RT_Camera::setupRayQuery(int x, int y, float sizeXminus1Half, float sizeYminus1Half, RT_RayQuery* rayQuery){
  Assert(rayQuery, "setupRayQuery");
  CVector2 origin2D = CVector2(x - sizeXminus1Half, y - sizeYminus1Half);

  CVector3 dir = (CVector3)(origin2D.x * u) + (CVector3)(origin2D.y * v) - (CVector3)(viewDistance * w);
  float normalizationFactor;
  dir.normalizeAndSaveMagnitude(&normalizationFactor);

  rayQuery->setDirection(&dir);
  rayQuery->setOrigin(&position);
}

void RT_Camera::computeCameraCoordinateSystem(){
  w = position - lookAt;
  w.normalize();
  u = up.cross(up, w);
  u.normalize();
  v = w.cross(w, u);
}


