#ifndef _CVECTOR2_H
#define _CVECTOR2_H

// class based on www.gametutorials.com - see license.txt

#include <iostream>
#include <ostream>
#include <stdlib.h>
#include <math.h>
#include "ForceInline.h"

using namespace std;

class CVector2 {
public:
    finline CVector2() { x = y = 0.0f; }

    finline CVector2(float X_, float Y_){
      x = X_; y = Y_;
    }

    finline CVector2 operator+(CVector2 v){
      return CVector2(v.x + x, v.y + y);
    }

    finline CVector2 operator-(CVector2 v){
      return CVector2(x - v.x, y - v.y);
    }

    finline CVector2 operator*(float value){
      return CVector2(x * value, y * value);
    }

    finline CVector2 operator/(float value){
      return CVector2(x / value, y / value);
    }

    finline bool operator== (CVector2 v){
      if (x == v.x && y == v.y)
        return true;
      else
        return false;
    }

    finline bool operator!= (CVector2 v){
      if (x == v.x && y == v.y)
        return false;
      else
        return true;
    }

    finline CVector2 operator-() const {
      return CVector2(-x, -y);
    }

    finline float magnitude(){
      return sqrtf((x * x) + (y * y));
    }

    finline void normalize(){
      const float magnitudeValue = magnitude();

      x = x / magnitudeValue;
      y = y / magnitudeValue;
    }

    finline void normalizeAndSaveMagnitude(float* pMagnitude){
      *pMagnitude = magnitude();

      x = x / *pMagnitude;
      y = y / *pMagnitude;
    }

    finline void clamp(){
      if (x < 0.0f)
        x = 0.0f;
      if (x > 1.0f)
        x = 1.0f;

      if (y < 0.0f)
        y = 0.0f;
      if (y > 1.0f)
        y = 1.0f;
    }

    finline CVector2 operator*(CVector2 v){
      return CVector2(x * v.x, y * v.y);
    }

    int maxComponentAbs();

    friend ostream& operator<<(ostream& s, const CVector2& vec);

    float x, y;

    typedef enum{
      X_COMPONENT = 0,
      Y_COMPONENT,
    } components;
};

const CVector2 operator*(float num, const CVector2& rhs);

#endif
