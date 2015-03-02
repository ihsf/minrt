#ifndef _RT_RAYTRACER_H
#define _RT_RAYTRACER_H

#include "Camera.h"
#include "CVector2i.h"
#include "Engine.h"
#include "Etc1Fast.h"
#include "RT_Camera.h"
#include "RT_FrameBuffer.h"
#include "RT_RayQuery.h"
#include "RT_TaskManager.h"

#include "CTexture.h" // ToDo: remove before release

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
    Camera* getGameCamera(){ return gameCamera;}  

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

    // remove later
    struct ETC1Header {
      char tag[6];                  // "PKM 10"
      unsigned short format;        // Format == number of mips (== zero)
      unsigned short texWidth;       // Texture dimensions, multiple of 4 (big-endian)
      unsigned short texHeight;
      unsigned short origWidth;      // Original dimensions (big-endian)
      unsigned short origHeight;
    };
};

#endif
