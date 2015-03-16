#ifndef _CVECTOR2I_H
#define _CVECTOR2I_H

#include <iostream>
#include <ostream>
#include <stdlib.h>
#include <math.h>
#include "ForceInline.h"

using namespace std;

class CVector2i {
  public:
    finline CVector2i() { x = y = 0; }

    finline CVector2i(int X_, int Y_){
      x = X_; y = Y_;
    }

    finline CVector2i operator+(CVector2i v){
      return CVector2i(v.x + x, v.y + y);
    }

    finline CVector2i operator-(CVector2i v){
      return CVector2i(x - v.x, y - v.y);
    }

    finline CVector2i operator*(int value){
      return CVector2i(x * value, y * value);
    }

    finline CVector2i operator/(int value){
      return CVector2i(x / value, y / value);
    }

    finline bool operator== (CVector2i v){
      if (x == v.x && y == v.y)
        return true;
      else
        return false;
    }

    finline bool operator!= (CVector2i v){
      if (x == v.x && y == v.y)
        return false;
      else
        return true;
    }

    finline CVector2i operator-() const {
      return CVector2i(-x, -y);
    }

    finline CVector2i operator*(CVector2i v){
      return CVector2i(x * v.x, y * v.y);
    }

    int x, y;
};


#endif
