#ifndef _CVECTOR3_H
#define _CVECTOR3_H

// class based on www.gametutorials.com - see license.txt

#include <math.h>
#include <iostream>
#include <ostream>
#include <assert.h>
#include "ForceInline.h"

#define Assert(a,b) assert( a && b )

#ifndef _MSC_VER
  #pragma warning( disable : 1786 279 )
#endif

using namespace std;

class CVector3{
  public:
    finline CVector3(){
      x = y = z = 0.0f;
    }

    finline CVector3(float X_, float Y_, float Z_){
      this->x = X_;
      this->y = Y_;
      this->z = Z_;
    }

    finline CVector3 cross(CVector3 vector1, CVector3 vector2){
      CVector3 crossProduct;

      crossProduct.x = ((vector1.y * vector2.z) - (vector1.z * vector2.y));
      crossProduct.y = ((vector1.z * vector2.x) - (vector1.x * vector2.z));
      crossProduct.z = ((vector1.x * vector2.y) - (vector1.y * vector2.x));

      return crossProduct;
    }

    finline float magnitude(){
      return sqrtf((x * x) + (y* y) + (z * z));
    }

    finline void normalizeAndSaveMagnitude(float* normalizationFactor){
      const float magnitudeValue = magnitude();

      Assert(magnitudeValue != 0, "CVector3::normalizeAndSaveMagnitude(float* normalizationFactor): vector should not be 0 for normalization!");
      *normalizationFactor = 1.0f / magnitudeValue;

      x = x / magnitudeValue;
      y = y / magnitudeValue;
      z = z / magnitudeValue;
    }

    finline void normalize(){
      const float magnitudeValue = magnitude();
      Assert(magnitudeValue != 0, "CVector3::Normalize: vector should not be 0 for normalization!");

      x = x / magnitudeValue;
      y = y / magnitudeValue;
      z = z / magnitudeValue;
    }

    finline float dotProduct(CVector3* v){
      return (x * v->x + y * v->y + z * v->z);
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

      if (z < 0.0f)
        z = 0.0f;
      if (z > 1.0f)
        z = 1.0f;
    }

    finline CVector3 operator+(CVector3 v){
      return CVector3(v.x + x, v.y + y, v.z + z);
    }

    finline CVector3 operator-(CVector3 v){
      return CVector3(x - v.x, y - v.y, z - v.z);
    }

    finline CVector3 operator/(float value){
      return CVector3(x / value, y / value, z / value);
    }

    finline CVector3& operator=(const CVector3 &other){
      x = other.x;
      y = other.y;
      z = other.z;

      return *this;
    }

    CVector3(const CVector3 &other){
      x = other.x;
      y = other.y;
      z = other.z;
    }

    finline bool operator== (CVector3 v){
      if (x == v.x && y == v.y && z == v.z)
        return true;
      else
        return false;
    }

    finline bool operator!= (CVector3 v){
      if (x == v.x && y == v.y && z == v.z)
        return false;
      else
        return true;
    }

    finline CVector3 operator*(float value){
      return CVector3(x * value, y * value, z * value);
    }

    finline CVector3 operator*(CVector3 v){
      return CVector3(x * v.x, y * v.y, z * v.z);
    }

    finline CVector3& operator*= (CVector3 v){
      x *= v.x;
      y *= v.y;
      z *= v.z;
      return *this;
    }

    finline CVector3& operator/= (CVector3 v){
      x /= v.x;
      y /= v.y;
      z /= v.z;
      return *this;
    }

    finline CVector3& operator+= (CVector3 v){
      x += v.x;
      y += v.y;
      z += v.z;
      return *this;
    }

    finline CVector3& operator-= (CVector3 v){
      x -= v.x;
      y -= v.y;
      z -= v.z;
      return *this;
    }

    finline CVector3& operator*= (float value){
      x *= value;
      y *= value;
      z *= value;
      return *this;
    }

    finline CVector3& operator/= (float value){
      x /= value;
      y /= value;
      z /= value;
      return *this;
    }

    finline CVector3 operator-() const {
      return CVector3(-x, -y, -z);
    }

    friend ostream& operator<<(ostream& s, const CVector3& vec);

    float x, y, z;
};

finline const CVector3 operator*(float value, const CVector3& rhs){
  return CVector3(rhs.x * value, rhs.y * value, rhs.z * value);
}

CVector3 operator+(const CVector3& lhs, const CVector3& rhs);
CVector3 operator-(const CVector3& lhs, const CVector3& rhs);

#endif
