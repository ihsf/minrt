#include <stdint.h>

#include "TaskDispatch.hpp"
#include "RT_RayTracer.h"
#ifdef __INTEL_COMPILER
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

  if (Engine::rectMode){
    cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
    cout << "RT_RayTracer::init: rectmode is on." << endl;
    cout << "RT_RayTracer::init: ToDo: change blockFB to consider rectSizeX/Y." << endl;
    cout << "RT_RayTracer::init: ToDo: change etcdata to consider rectSizeX/Y." << endl;
    cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
    blockFB = new RT_FrameBuffer(Engine::screenWidthRT, Engine::screenHeightRT);
    etcdata = new unsigned char[Engine::screenWidthRT * Engine::screenHeightRT / 2];
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
  const int width = frameBuffer->getSizeX();

  look();
  taskManager.deleteAllTasks();
  createRenderingTasks();

  if(Engine::server){
    for(int i=0; i<(int)taskManager.tasks.size(); i++){
      TaskDispatch::Queue([this, i, fb1, fb2, width]{
        // render the tile to get RGBA data into the framebuffer
        taskManager.tasks[i]->run();

        auto src = fb1 + width * Engine::RENDERLINE_SIZE * i;
        auto dst = fb2 + width * Engine::RENDERLINE_SIZE * i;

        // copy from RGBA data from fb1 into the blockwise-oriented fb2
        for(int blockY = 0; blockY < Engine::RENDERLINE_SIZE/4; blockY++){
          for(int blockX = 0; blockX < width / 4; blockX++){
            for(int x = 0; x < 4; x++){
              for(int y = 0; y < 4; y++){
                *dst++ = *src;
                src += width;
              }
              src -= width * 4 - 1;
            }
          }
          src += width * 3;
        }

        auto etc = ((uint64_t*)etcdata) + i * width / 4;
        auto etcsrc = ((uint8_t*)fb2) + width * Engine::RENDERLINE_SIZE * i * 4;

        // loop through the blockwise-oriented fb2 and compress RGBA into ETC1 and store this in etc/etcdata.
        for (int i = 0; i < width*Engine::RENDERLINE_SIZE / 16; i++)
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

// ToDo:
// this has to be changed to support rect mode
// in rect mode we render e.g.
//    * rect0    X (0 - 959)   Y (0 - 1079)
//    * rect1    X (960 - 1919)   Y (0 - 1079)
//
//
// right now because of the RENDERLINE_SIZE this will not work
//
// also might need to change allocation of "etcdata" to be rect size
// and to make sure that only the LZ4 compressed rect will be sent, not the whole frame buffer
void RT_RayTracer::createRenderingTasks(){
  CVector2i startPixel;

  const int sizeX = frameBuffer->getSizeX();
  const int sizeY = frameBuffer->getSizeY();
  const int numTiles = sizeY / Engine::RENDERLINE_SIZE;

  char debugTaskName[] = "rendTile";
  char fullDebugTaskName[DEBUG_STRING_SIZE];

	// taking tiles from left corner to right corner, then top to bottom
  for(int i = 0; i < numTiles; i++){
    startPixel.x = 0;
    startPixel.y = i * Engine::RENDERLINE_SIZE;

    if(Engine::rectMode){
      if(!Engine::isPointInsideRect(startPixel.x, startPixel.y)){
        continue;
      }
    }

    RT_TaskRenderTile* taskRenderTile = new RT_TaskRenderTile(&startPixel, internalCamera, frameBuffer, i);
    taskManager.addTaskRenderTile(taskRenderTile);

    // set debug name for task
    sprintf(fullDebugTaskName, "%s_%i", debugTaskName, i);
    taskRenderTile->setDescription(fullDebugTaskName);
  }  
}

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
#ifdef __INTEL_COMPILER
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
