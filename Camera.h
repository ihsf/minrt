#ifndef _CAMERA_H
#define _CAMERA_H

// class based on www.gametutorials.com - see license.txt

#include "CVector3.h"
#include "Engine.h"
#include "WindowsHelper.h"

#include <iostream>

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif


using namespace std;

// handles the camera movement (per keyboard and mouse)
class Camera {
  public:
    Camera();
    ~Camera();

    finline CVector3 getPosition() { return position; }
    finline CVector3 getView()     { return view; }
    finline CVector3 getUpVector() { return upVector; }

    CVector3 getDirectionNormalized();
    CVector3 getMinusDirectionNormalized();

    finline CVector3 getStrafe()   { return strafe; }

    finline CVector3* getPositionPointer() { return &position; }
    finline CVector3* getViewPointer()     { return &view; }
    finline CVector3* getStrafePointer()   { return &strafe; }
    finline CVector3* getUpVectorPointer() { return &upVector; }

    void setPosition(CVector3 pos_){ position = pos_; }
    void setView(CVector3 view_){ view = view_; }
    void setUpVector(CVector3 up_){ upVector = up_; }

    void rotateView(float angle, float X, float Y, float Z);

    void applyMouseLook();
    void applyKeyboardMovements();

    void strafeCamera(float speed);
    void moveCamera(float speed);

    void update();

  private:
    void init();

    CVector3 position;
    CVector3 view;
    CVector3 upVector;
    CVector3 strafe;

    int cameraDirectionIsTooSmall; // for error handling
};

#endif
