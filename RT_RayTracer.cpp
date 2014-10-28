#include "TaskDispatch.hpp"
#include "RT_RayTracer.h"
#ifdef __INTEL_COMPILER
  #include <cilk/cilk.h>
#endif

RT_RayTracer::RT_RayTracer( Camera* gameCamera_){
  Assert(gameCamera_, "RT_RayTracer: gameCamera");
  this->gameCamera = gameCamera_;

  frameBuffer = NULL;
	internalCamera = NULL;
}

RT_RayTracer::~RT_RayTracer(){
  delete frameBuffer;
	delete internalCamera;
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
  const int numTiles = (sizeX * sizeY) / (Engine::RENDERTILE_SIZE * Engine::RENDERTILE_SIZE);
  const int numTilesPerRow = frameBuffer->getSizeX() / Engine::RENDERTILE_SIZE;

  char debugTaskName[] = "rendTile";
  char fullDebugTaskName[DEBUG_STRING_SIZE];

	// taking tiles from left corner to right corner, then top to bottom
  for(int i = 0; i < numTiles; i++){
    int tileX = i % numTilesPerRow;
    int tileY = i / numTilesPerRow;
    
    startPixel.x = tileX * Engine::RENDERTILE_SIZE;
    startPixel.y = tileY * Engine::RENDERTILE_SIZE;

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
