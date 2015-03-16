#include "CVector3.h"

// class based on www.gametutorials.com - see license.txt

ostream& operator<<(ostream& s, const CVector3& vec){
  s << "(" << vec.x << " " << vec.y << " " << vec.z << ")";
  return s;
}

CVector3 operator+(const CVector3& lhs, const CVector3& rhs){
  return CVector3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

CVector3 operator-(const CVector3& lhs, const CVector3& rhs){
  return CVector3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

