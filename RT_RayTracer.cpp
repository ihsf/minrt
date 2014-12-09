#include <stdint.h>

#include "TaskDispatch.hpp"
#include "RT_RayTracer.h"
#ifdef __cilk
  #include <cilk/cilk.h>
#endif
#include "ProcessRGB.hpp"
#include "Dither.hpp"

RT_RayTracer::RT_RayTracer( Camera* gameCamera_){
  Assert(gameCamera_, "RT_RayTracer: gameCamera");
  this->gameCamera = gameCamera_;

  frameBuffer = NULL;
  blockFB = NULL;
	internalCamera = NULL;
  etcdata = NULL;
}

RT_RayTracer::~RT_RayTracer(){
  delete frameBuffer;
  delete blockFB;
	delete internalCamera;
  delete[] etcdata;
}

void RT_RayTracer::initCamera(){
	internalCamera = new RT_Camera();

  internalCamera->setPosition(gameCamera->getPositionPointer());
  internalCamera->setLookAt(gameCamera->getViewPointer());
	internalCamera->setUpVector(0,1,0);

  const float tangensValueDEG2R = tanf(DEG2RAD(Engine::cg_fov / 2.0f));
  const float viewDistance = ((float)Engine::screenHeightRT / 2.0f) / tangensValueDEG2R; 
	
	internalCamera->setViewDistance(viewDistance);
  internalCamera->computeCameraCoordinateSystem();
}

void RT_RayTracer::init(){
  frameBuffer = new RT_FrameBuffer(Engine::screenWidthRT, Engine::screenHeightRT);

  if(Engine::rectMode){
    cout << "RT_RayTracer::init: rectmode is on. Creating smaller blockFB and etcdata." << endl;
    blockFB = new RT_FrameBuffer(Engine::rectSizeX, Engine::rectSizeY);
    etcdata = new unsigned char[Engine::rectSizeX * Engine::rectSizeY / 2];
  } else {
    blockFB = new RT_FrameBuffer(Engine::screenWidthRT, Engine::screenHeightRT);
    etcdata = new unsigned char[Engine::screenWidthRT * Engine::screenHeightRT / 2];
  }

  clearFrameBuffers();
	initCamera();
}

void RT_RayTracer::clearFrameBuffers(){
  if(frameBuffer)
    frameBuffer->clear();
}

unsigned char* RT_RayTracer::getFrameBuffer(){ 
	Assert(frameBuffer, "getFrameBuffer");
	unsigned char* returnMe = frameBuffer->getFrameBuffer();

	Assert(returnMe, "getFrameBuffer");
	return returnMe;
}

void RT_RayTracer::renderFrame(){
  look();
  renderScene();

  // in server mode the server will be told which frame number it is working at through the network packet
  if(!Engine::server)  {
	  Engine::numFramesRendered++;
  }
}

void RT_RayTracer::renderFrameETC(){
  uint32_t* fb1 = (uint32_t*)frameBuffer->getFrameBuffer();
  uint32_t* fb2 = (uint32_t*)blockFB->getFrameBuffer();

  const int widthFB1 = frameBuffer->getSizeX(); // width of full frame buffer, e.g. 1280 for 1280x720
  int widthFB2 = widthFB1; 
  if(Engine::rectMode){
    widthFB2 = Engine::rectSizeX;   // width of rect, e.g. 640 if only one half of the full frame buffer should be rendered
  }

  look();
  taskManager.deleteAllTasks();
  createRenderingTasks();

  if(Engine::server){
    for(int i=0; i<(int)taskManager.tasks.size(); i++){
      TaskDispatch::Queue([this, i, fb1, fb2, widthFB1, widthFB2]{
        // render the tile to get RGBA data into the framebuffer
        taskManager.tasks[i]->run();

        auto src = fb1 + widthFB1 * Engine::RENDERLINE_SIZE * i;
        if(Engine::rectMode){
          src += Engine::rectBottom * widthFB1 + Engine::rectLeft;  // offset the RGBA access until the bottom (lower part) of the rect starts and on the left side
        }
        auto dst = fb2 + widthFB2 * Engine::RENDERLINE_SIZE * i;

        // copy RGBA data from fb1 into the blockwise-oriented fb2
        for(int blockY = 0; blockY < Engine::RENDERLINE_SIZE/4; blockY++){  // if RENDERLINE_SIZE=4 then this loop will only be executed once
          for(int blockX = 0; blockX < widthFB2 / 4; blockX++){
            for(int x = 0; x < 4; x++){
              for(int y = 0; y < 4; y++){
                *dst++ = *src;
                src += widthFB1;
              }
              src -= widthFB1 * 4 - 1;
            }
          }
          src += widthFB1 * 3; // if RENDERLINE_SIZE=4 then this is irrelevant as src will not be used later
        }

        auto etc = ((uint64_t*)etcdata) + i * widthFB2 / 4;
        auto etcsrc = ((uint8_t*)fb2) + widthFB2 * Engine::RENDERLINE_SIZE * i * 4;

        // loop through the blockwise-oriented fb2 and compress RGBA into ETC1 and store this in etc/etcdata.
        for (int i = 0; i < widthFB2*Engine::RENDERLINE_SIZE / 16; i++)
        {
  #if 0
          Dither( etcsrc );
  #endif
          *etc++ = ProcessRGB( etcsrc );
          etcsrc += 4*4*4;
        }
      } );
    }
  } else {
    for (int i = 0; i<(int)taskManager.tasks.size(); i++){
      TaskDispatch::Queue([this, i]{
        taskManager.tasks[i]->run();
      });
    }
  }
  TaskDispatch::Sync();
}

void RT_RayTracer::look(){
	CVector3 position = gameCamera->getPosition();
  internalCamera->setPosition(&position);
	
  CVector3 lookAt = position + gameCamera->getDirectionNormalized();

	internalCamera->setLookAt(&lookAt);
  internalCamera->setUpVector(gameCamera->getUpVectorPointer());
  internalCamera->computeCameraCoordinateSystem();
}

void RT_RayTracer::createRenderingTasks(){
  CVector2i startPixel;

  int sizeY = frameBuffer->getSizeY();
  if (Engine::rectMode){
    sizeY = Engine::rectSizeY;
  }

  const int numTiles = sizeY / Engine::RENDERLINE_SIZE;

	// taking tiles from top to bottom
  for(int i = 0; i < numTiles; i++){
    if (!Engine::rectMode){
      startPixel.x = 0;
      startPixel.y = i * Engine::RENDERLINE_SIZE;
    } else {
      startPixel.x = Engine::rectLeft;
      startPixel.y = Engine::rectBottom + (i * Engine::RENDERLINE_SIZE);
    }

    RT_TaskRenderTile* taskRenderTile = new RT_TaskRenderTile(&startPixel, internalCamera, frameBuffer, i);
    taskManager.addTaskRenderTile(taskRenderTile);
  }  
}

#if 0
char fullDebugTaskName[DEBUG_STRING_SIZE];
char debugTaskName[] = "rendTile";
// set debug name for task
sprintf(fullDebugTaskName, "%s_%i", debugTaskName, i);
taskRenderTile->setDescription(fullDebugTaskName);
#endif

void RT_RayTracer::renderScene(){	
  taskManager.deleteAllTasks();

  createRenderingTasks();

	switch(Engine::methodToMultiThread){
		case Engine::OPENMP:
			runTasksOpenMP();
			break;
		
		case Engine::OPENMPT:
			runTasksOpenMPT();
			break;

		case Engine::CILK:
			runTasksCilk();
			break;

    case Engine::TASKDISPATCH:
      runTasksTaskDispatcher();
      break;

		default:
			cout << "RT_RayTracer::renderScene: Invalid method to multi-thread" << endl;
			break;
	}
}

void RT_RayTracer::runTasksOpenMP(){
#pragma omp parallel for
  for(int i = 0; i < (int)taskManager.tasks.size(); i++){
    _mm_setcsr(_mm_getcsr() | /*FTZ:*/ (1<<15) | /*DAZ:*/ (1<<6));
    taskManager.tasks[i]->run();    
  }
}

void RT_RayTracer::runTasksOpenMPT(){
#pragma omp parallel
#pragma omp single
  for(int i = 0; i < (int)taskManager.tasks.size(); i++){
    _mm_setcsr(_mm_getcsr() | /*FTZ:*/ (1<<15) | /*DAZ:*/ (1<<6));
#ifdef __INTEL_COMPILER
#pragma omp task    
#endif
	taskManager.tasks[i]->run();    
  }
}

void RT_RayTracer::runTasksCilk(){
  for(int i = 0; i < (int)taskManager.tasks.size(); i++){
    _mm_setcsr(_mm_getcsr() | /*FTZ:*/ (1<<15) | /*DAZ:*/ (1<<6));
#ifdef __cilk
    cilk_spawn(taskManager.tasks[i]->run());    
#endif
  }
}

void RT_RayTracer::runTasksTaskDispatcher()
{
  for( int i=0; i<(int)taskManager.tasks.size(); i++ )
  {
    TaskDispatch::Queue( [this, i]{ taskManager.tasks[i]->run(); } );
  }
  TaskDispatch::Sync();
}
