#ifndef _MAIN_H
#define _MAIN_H

#if defined(_WIN32)
  #include <windows.h>
#endif

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "SDL/SDL.h"
#include <iostream>
#include "Engine.h"
#include "SDLstuff.h"
#include "Camera.h"
#include "OpenGLstuff.h"
#include "CVector3.h"
#include "RT_RayTracer.h"
#include "WindowsHelper.h"
#include "NetworkStuff.h"
#include "EmbreeStuff.h"

using namespace std;

int main(int argc, char *argv[]);

void doGameLoop(SDLstuff* sdlstuff, Camera* camera, RT_RayTracer* rayTracer, OpenGLstuff* openglstuff, NetworkStuff* networkStuff);

void printAverageFPS();
//void init_Network(int argc, char *argv[]);

#endif 

