#include "Engine.h"

int Engine::screenWidthRT = 1280;  // needs to be divisable by RENDERTILE_SIZE
int Engine::screenHeightRT =  720;  // needs to be divisable by RENDERTILE_SIZE
int Engine::screenWidthGL = 1280;
int Engine::screenHeightGL = 720;

int Engine::RENDERTILE_SIZE = 40;

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

bool Engine::server = true; // false;

TCPsocket Engine::sd = NULL;
TCPsocket Engine::csd = NULL;
IPaddress Engine::ip;
IPaddress* Engine::remoteIP = NULL;

int Engine::serverPort = 2000;

int Engine::methodToMultiThread = Engine::OPENMP;  // might be overwritten in Engine::init

bool Engine::dedicated = true; // false;

float Engine::framesPerSecond = 1.0f;

int Engine::numFramesRendered = 0;

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
  if ((screenWidthRT % RENDERTILE_SIZE != 0) || screenHeightRT % RENDERTILE_SIZE != 0){
    cout << "Invalid screenWidthRT or screenHeightRT for RENDERTILE_SIZE" << endl;
    exit(1);
  }

  parseArgv(argc, argv);

#ifdef __INTEL_COMPILER
  methodToMultiThread = Engine::CILK;
#endif
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
