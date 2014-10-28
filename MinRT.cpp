#include "TaskDispatch.hpp"
#include "MinRT.h"
#include "Dither.hpp"

TaskDispatch dispatch( 8 );

int main(int argc, char *argv[]){
  InitDither();
  Engine::init(argc, argv);
	Camera camera;

  EmbreeStuff::init();  

  RT_RayTracer rayTracer(&camera);

	OpenGLstuff openglstuff(&rayTracer);	

	SDLstuff sdlstuff(&rayTracer, &openglstuff);
  sdlstuff.init();

  openglstuff.init(sdlstuff.getMainWindow());

	NetworkStuff networkStuff(&camera, &openglstuff, &rayTracer);

	rayTracer.init();
  
  camera.setMainWindow(sdlstuff.getMainWindow());

  sdlstuff.grabKeyAndMouse();

  EmbreeStuff::addSampleMesh1();
  EmbreeStuff::addSampleMesh2();
  EmbreeStuff::addSampleMesh3();
  EmbreeStuff::compile();

  // main game loop
  while(!Engine::done)  {
		doGameLoop(&sdlstuff, &camera, &rayTracer, &openglstuff, &networkStuff);
	}

	// Close the client socket	
	networkStuff.shutdown();

	// preparing to exit... 
  sdlstuff.ungrabKeyAndMouse();

	cout << "MinRT: Leaving now..." << endl;  

  return 0;
}


void doGameLoop(SDLstuff* sdlstuff, Camera* camera, RT_RayTracer* rayTracer, OpenGLstuff* openglstuff, NetworkStuff* networkStuff){
  if(Engine::server) {
		networkStuff->receiveMessageFromGameClient();
	} else {
		sdlstuff->checkEvents();
	}

  Engine::calculateFrameRate();
	camera->update();	

	if(Engine::server) {
		networkStuff->sendMessageToGameClient();
	} else {
    rayTracer->renderFrame(); 
	}

  printAverageFPS();

  openglstuff->render();
	openglstuff->swapBuffers(); 
}

void printAverageFPS(){
  static int frameID = 0;
  static float elapsedTime = 0, totalTime = 0;
  static float renderTimes[8];

  float dt = WindowsHelper::getMsElapsed()/1000.0;
  elapsedTime += dt;

  if (frameID == 0) 
    cout.width(7);

  int frame8 = frameID++ % 8;
  renderTimes[frame8] = dt;

  if (frame8 == 8-1 && frameID > 8) {
	  float avg = 0.0f; 
    for (int i=0; i<8; i++){ 
      avg += renderTimes[i];
    }

	  totalTime += avg;
	  avg /= 8.0f;
	  elapsedTime = 0.0f;

		if(rand() % 32 == 1)
			cout << "Rendering " << frameID << ": " << 1.0f/avg << " fps"  << endl;
  }
}
