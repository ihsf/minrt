#include "TaskDispatch.hpp"
#include "MinRT.h"
#include "Dither.hpp"
#include "System.hpp"

TaskDispatch dispatch(System::CPUCores());

int main(int argc, char *argv[]){
  InitDither();
  Engine::init(argc, argv);
  Camera camera;

  EmbreeStuff::init();

  RT_RayTracer rayTracer(&camera);

  OpenGLstuff openglstuff(&rayTracer);
  SDLstuff sdlstuff(&rayTracer);
  NetworkStuff networkStuff(&camera, &rayTracer);

  if (!Engine::server) {
    sdlstuff.init();
    openglstuff.init(sdlstuff.getMainWindow());
  }
  rayTracer.init();

  sdlstuff.grabKeyAndMouse();

  EmbreeStuff::addSampleMesh1();
  EmbreeStuff::addSampleMesh2();
  EmbreeStuff::addSampleMesh3();
  EmbreeStuff::compile();

  // main game loop
  while (!Engine::done) {
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
  if (Engine::server) {
    networkStuff->receiveMessageFromGameClient();
  } else {
    sdlstuff->checkEvents();
  }

  Engine::calculateFrameRate();
  camera->update();

  if (Engine::server) {
    networkStuff->sendMessageToGameClient();
  } else {
    rayTracer->renderFrame();
    openglstuff->render();
    openglstuff->swapBuffers();
  }

  printAverageFPS();
}

// another fps counter that averages over 8 frames
// changed how often it should print with "rand() % 32 == 1"
void printAverageFPS(){
  static int frameID = 0;
  static float elapsedTime = 0, totalTime = 0;
  static float renderTimes[8];

  float dt = WindowsHelper::getMsElapsed() / 1000.0;
  elapsedTime += dt;

  if (frameID == 0)
    cout.width(7);

  int frame8 = frameID++ % 8;
  renderTimes[frame8] = dt;

  if (frame8 == 8 - 1 && frameID > 8) {
    float avg = 0.0f;
    for (int i = 0; i < 8; i++){
      avg += renderTimes[i];
    }

    totalTime += avg;
    avg /= 8.0f;
    elapsedTime = 0.0f;

    if (rand() % 32 == 1)
      cout << "Rendering " << frameID << ": " << 1.0f / avg << " fps" << endl;
  }
}
