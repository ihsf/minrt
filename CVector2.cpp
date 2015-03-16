#include "CVector2.h"

// class based on www.gametutorials.com - see license.txt

const CVector2 operator*(float num, const CVector2& rhs){
  return CVector2(rhs.x * num, rhs.y * num);
}

ostream& operator<<(ostream& s, const CVector2& vec){
  s << "(" << vec.x << " " << vec.y << ")";
  return s;
}

int CVector2::maxComponentAbs(){
  if (fabsf(x) > fabsf(y)){
    return X_COMPONENT;
  }
  else {
    return Y_COMPONENT;
  }
}
