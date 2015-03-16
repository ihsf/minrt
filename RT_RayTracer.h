#ifndef _RT_RAYTRACER_H
#define _RT_RAYTRACER_H

#include "Camera.h"
#include "CVector2i.h"
#include "Engine.h"
#include "RT_Camera.h"
#include "RT_FrameBuffer.h"
#include "RT_RayQuery.h"
#include "RT_TaskManager.h"

#include <math.h>
#include <stdio.h>
#include <vector>

using namespace std;

class RT_RayTracer {
  public:
    RT_RayTracer(Camera* gameCamera_);
    ~RT_RayTracer();

    void init();

    void look();
    void renderFrame();
    void renderFrameETC();

    unsigned char* getFrameBuffer();
    const unsigned char* getDataETC() const { return etcdata; }
    Camera* getGameCamera(){ return gameCamera; }

    void clearFrameBuffers();

  private:
    Camera* gameCamera;

    RT_TaskManager taskManager;

    void initCamera();

    void renderScene();

    void createRenderingTasks();
    void waitForAllThreads();
    void runTasksOpenMP();
    void runTasksCilk();
    void runTasksOpenMPT();
    void runTasksTaskDispatcher();

    RT_Camera* internalCamera;
    RT_FrameBuffer* frameBuffer;
    RT_FrameBuffer* blockFB;
    unsigned char* etcdata;
};

#endif
