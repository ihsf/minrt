#include "Camera.h"
#include <float.h>

// class based on www.gametutorials.com - see license.txt

Camera::Camera(){
  init();
}

void Camera::init(){
  cameraDirectionIsTooSmall = 0;

  position = CVector3(0.0f, 10.0f, -20.0f);                 
  view     = CVector3(0.0f, 10.0f, -19.0f);                   
  upVector = CVector3(0.0f, 1.0f, 0.0f);      
}

Camera::~Camera(){
}

void Camera::applyMouseLook(){
  const int middleX = Engine::screenWidthGL  / 2;      
  const int middleY = Engine::screenHeightGL / 2;      
                    
  if((Engine::relativeMouseMotion.x == 0) && (Engine::relativeMouseMotion.y == 0)) 
    return;

  if(Engine::currentTime < 0.1f)
    return;

  float angleY = (float)((-Engine::relativeMouseMotion.x) / 1000.0f);
  float angleZ = (float)((-Engine::relativeMouseMotion.y) / 1000.0f);

  Engine::relativeMouseMotion = CVector2i(0,0); // reset motion for next frame

  CVector3 viewMinusPosition = view - position;

  float dotProduct = upVector.dotProduct(&viewMinusPosition);

  // avoid invalid camera values
  if(dotProduct > 0.95f){					
    viewMinusPosition.x += 0.05f;
    viewMinusPosition.y -= 0.1f;
    viewMinusPosition.normalize();
    dotProduct = upVector.dotProduct(&viewMinusPosition);
  }

  if(dotProduct < -0.95f){
    viewMinusPosition.x += 0.05f;
    viewMinusPosition.y += 0.1f;
    viewMinusPosition.normalize();
    dotProduct = upVector.dotProduct(&viewMinusPosition);
  }

  CVector3 axis;
  axis = axis.cross(viewMinusPosition, upVector);
  axis.normalize();

  // check to avoid invalid camera angles
#ifdef WIN32
  if(_isnan(axis.x )){
#else
  if(isnan(axis.x )){
#endif
    viewMinusPosition = CVector3(1, 0, 0);
    axis = axis.cross(viewMinusPosition, upVector);
    axis.normalize();			
  }

  rotateView(angleZ, axis.x, axis.y, axis.z);
  rotateView(angleY, 0, 1, 0);
}

void Camera::rotateView(float angle, float x, float y, float z){
  CVector3 direction = getDirectionNormalized(); 

  // increase mouse/rotation sensitivity
  angle *= 3.0f;
  
  const float cosTheta = cosf(angle);
  const float sinTheta = sinf(angle);

  CVector3 newView;

  newView.x  = (cosTheta + (1 - cosTheta) * x * x)     * direction.x;
  newView.x += ((1 - cosTheta) * x * y - z * sinTheta) * direction.y;
  newView.x += ((1 - cosTheta) * x * z + y * sinTheta) * direction.z;

  newView.y  = ((1 - cosTheta) * x * y + z * sinTheta) * direction.x;
  newView.y += (cosTheta + (1 - cosTheta) * y * y)     * direction.y;
  newView.y += ((1 - cosTheta) * y * z - x * sinTheta) * direction.z;

  newView.z  = ((1 - cosTheta) * x * z - y * sinTheta) * direction.x;
  newView.z += ((1 - cosTheta) * y * z + x * sinTheta) * direction.y;
  newView.z += (cosTheta + (1 - cosTheta) * z * z)     * direction.z;
  
#ifdef WIN32    
  if(_isnan(newView.x )){
#else
  if(isnan(newView.x )){
#endif
    return;
  }

  view = position + newView;
}

void Camera::strafeCamera(float speed){   
  position.x += strafe.x * -speed;
  position.z += strafe.z * -speed;

  view.x += strafe.x * -speed;
  view.z += strafe.z * -speed;
}

void Camera::moveCamera(float speed){
  CVector3 direction = getDirectionNormalized(); 

  position += direction * speed;
  view += direction * speed;       
}

void Camera::applyKeyboardMovements(){
  const float speedUp = 30.0f; 
  float speed = speedUp * Engine::frameInterval;

  if(speed < 0.0f)
    speed = -speed;

  if(Engine::crouchKey){
    position.y -= speed;
    view.y -= speed;
  }

  if(Engine::jumpKey){
    position.y += speed;
    view.y += speed;
  }

  if(Engine::upKey) {             
    moveCamera(speed);              
  }

  if(Engine::downKey) {           
    moveCamera(-speed);             
  }

  if(Engine::leftKey) {           
    strafeCamera(-speed);
  }

  if(Engine::rightKey) {          
    strafeCamera(speed);
  }   
}

void Camera::update() {
  if (view == NULLVECTOR3f && position == NULLVECTOR3f)
    return;

  strafe = strafe.cross(view - position, upVector);
  strafe.normalize();

  applyMouseLook();
  applyKeyboardMovements();
}

CVector3 Camera::getDirectionNormalized(){
  CVector3 dir = view - position;
  if(dir.magnitude() <0.01f) {
    cameraDirectionIsTooSmall++;
    if(cameraDirectionIsTooSmall){
      // if this happened to often, we quit the application. most likely the client has already disconnected
      if(cameraDirectionIsTooSmall > 1500 && Engine::server){
        Engine::done = true;
      }
    }
  }
  dir.normalize();

  return dir;
}
