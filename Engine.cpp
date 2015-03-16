#include "Engine.h"

// frame rate calculation based on www.gametutorials.com - see license.txt

#if 0
  int Engine::screenWidthRT = 2560;  // needs to be divisable by RENDERTILE_SIZE
  int Engine::screenHeightRT = 1440;  // needs to be divisable by RENDERTILE_SIZE
  int Engine::screenWidthGL = 2560;
  int Engine::screenHeightGL = 1440;
#else
  #if 0
    int Engine::screenWidthRT = 1920;  
    int Engine::screenHeightRT = 1080;  
    int Engine::screenWidthGL = 1920;
    int Engine::screenHeightGL = 1080;
  #else
    #if 1
      int Engine::screenWidthRT = 1280;  
      int Engine::screenHeightRT = 720;  
      int Engine::screenWidthGL = 1280;
      int Engine::screenHeightGL = 720;
    #else
      #if 0
        int Engine::screenWidthRT = 240;  
        int Engine::screenHeightRT = 240; 
        int Engine::screenWidthGL = 240;
        int Engine::screenHeightGL = 240;
      #else
        int Engine::screenWidthRT = 512;  
        int Engine::screenHeightRT = 256;  
        int Engine::screenWidthGL = 512;
        int Engine::screenHeightGL = 256;
      #endif
    #endif
  #endif
#endif

int Engine::RENDERLINE_SIZE = 4;

// keys & mouse
bool Engine::upKey = false;
bool Engine::downKey = false;
bool Engine::leftKey = false;
bool Engine::rightKey = false;
bool Engine::leftButton = false;
bool Engine::rightButton = false;
bool Engine::jumpKey = false;
bool Engine::crouchKey = false;
bool Engine::done = false;

float Engine::currentTime = 0.0f;

int Engine::numAccelerometerHits = 0;
int Engine::previousNumAccelerometerHits = 0;
bool Engine::accelerometerChangedThisFrame = false;

#if 1
bool Engine::server = true;
bool Engine::dedicated = true;
#else
bool Engine::server = false;
bool Engine::dedicated = false;
#endif

TCPsocket Engine::sd = NULL;
TCPsocket Engine::csd = NULL;
IPaddress Engine::ip;
IPaddress* Engine::remoteIP = NULL;

int Engine::serverPort = 2000;

MultiThreadMethods Engine::methodToMultiThread = MultiThreadMethods::TASKDISPATCH; 

bool Engine::debugLatency = false;
int Engine::debugNumCameraChanges = 0;

float Engine::framesPerSecond = 1.0f;

int Engine::numFramesRendered = 0;

int Engine::embreeIntersect = 8;

float Engine::cg_fov = 65.0f;

float Engine::frameInterval = 0.3f;
float Engine::frameTime = 0.0f;
float Engine::lastTime = 0.0f;

int Engine::rectLeft = -1;
int Engine::rectTop = -1;
int Engine::rectRight = -1;
int Engine::rectBottom = -1;
int Engine::rectSizeX = -1;
int Engine::rectSizeY = -1;
bool Engine::rectMode = false;

CVector2i Engine::relativeMouseMotion;

void Engine::init(int argc, char *argv[]){
  cout << "Engine initialized." << endl;
  if (screenHeightRT % RENDERLINE_SIZE != 0){
    cout << "Invalid screenWidthRT or screenHeightRT for RENDERTILE_SIZE" << endl;
    exit(1);
  }

  parseArgv(argc, argv);
}

void Engine::calculateFrameRate(){
  float backupCurrentTime = currentTime;
  float totalCurrentTime = SDL_GetTicks() * 0.001f;
  float localCurrentTime = totalCurrentTime;

  currentTime = localCurrentTime;

  float backupFrameInterval = frameInterval;
  frameInterval = currentTime - frameTime;
  frameTime = currentTime;

  framesPerSecond++;

  if(currentTime - lastTime > 1.0f){
    lastTime = currentTime;
    framesPerSecond = 0;
  }

  if(server){
    frameInterval = backupFrameInterval;
    currentTime = backupCurrentTime;
  }
}

void Engine::parseArgv(int argc, char *argv[]){
  for(int i = 1; i < argc; i++){
    // 
    // port
    // 
    if(!strcmp(argv[i], "-port")){
      serverPort = atoi(argv[i + 1]);
      cout << "Overwriting port to: " << serverPort << endl;
    }
  }
}

bool Engine::isPointInsideRect(int x, int y){
  if (x < rectLeft)
    return false;
  if (x > rectRight)
    return false;
  if (y < rectBottom)
    return false;
  if (y > rectTop)
    return false;

  return true;
}
