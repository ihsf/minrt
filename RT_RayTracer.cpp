#include <stdint.h>

#include "TaskDispatch.hpp"
#include "RT_RayTracer.h"
#ifdef __INTEL_COMPILER
  #include <cilk/cilk.h>
#endif
#include "ProcessRGB.hpp"

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
  blockFB = new RT_FrameBuffer(Engine::screenWidthRT, Engine::screenHeightRT);
  etcdata = new unsigned char[Engine::screenWidthRT * Engine::screenHeightRT / 2];

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

void RT_RayTracer::renderFrameETC()
{
  auto fb1 = (uint32_t*)frameBuffer->getFrameBuffer();
  auto fb2 = (uint32_t*)blockFB->getFrameBuffer();
  const auto w = frameBuffer->getSizeX();

  look();
  taskManager.deleteAllTasks();
  createRenderingTasks();
  for( int i=0; i<(int)taskManager.tasks.size(); i++ )
  {
    TaskDispatch::Queue( [this, i, fb1, fb2, w]{
      taskManager.tasks[i]->run();
      auto src = fb1 + w * Engine::RENDERLINE_SIZE * i;
      auto dst = fb2 + w * Engine::RENDERLINE_SIZE * i;
      for( int by=0; by<Engine::RENDERLINE_SIZE/4; by++ )
      {
        for( int bx=0; bx<w/4; bx++ )
        {
          for( int x=0; x<4; x++ )
          {
            for( int y=0; y<4; y++ )
            {
              *dst++ = *src;
              src += w;
            }
            src -= w * 4 - 1;
          }
        }
        src += w * 3;
      }
      auto etc = ((uint64_t*)etcdata) + i * w / 4;
      auto etcsrc = ((uint8_t*)fb2) + w * Engine::RENDERLINE_SIZE * i * 4;
      for( int i=0; i < w*Engine::RENDERLINE_SIZE/16; i++ )
      {
        *etc++ = ProcessRGB( etcsrc );
        etcsrc += 4*4*4;
      }
    } );
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

  const int sizeX = frameBuffer->getSizeX();
  const int sizeY = frameBuffer->getSizeY();
  const int numTiles = sizeY / Engine::RENDERLINE_SIZE;

  char debugTaskName[] = "rendTile";
  char fullDebugTaskName[DEBUG_STRING_SIZE];

	// taking tiles from left corner to right corner, then top to bottom
  for(int i = 0; i < numTiles; i++){
    startPixel.x = 0;
    startPixel.y = i * Engine::RENDERLINE_SIZE;

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
