#ifndef _ENGINE_H
#define _ENGINE_H

// frame rate calculation based on www.gametutorials.com - see license.txt

#ifdef _WIN32
  #include <windows.h>
#endif

#include "CVector2.h"
#include "CVector2i.h"
#include "CVector3.h"
#include "ForceInline.h"

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <SDL.h>
#include <SDL_net.h>

using namespace std;

#ifdef _MSC_VER
  #pragma warning(disable: 4244) // conversion from 'const double' to 'float', possible loss of data
  #pragma warning(disable: 4996) // '*' was declared deprecated
#endif

#ifdef __INTEL_COMPILER
  #pragma warning(disable: 1786) // function strcpy was declared deprecated ...
  #pragma warning(disable: 279)
  #pragma warning(disable: 1478) //  function "alutLoadWAVFile" was declared "deprecated"
  #pragma warning(disable: 589)  //  transfer of control bypasses initialization of
#endif

#define Assert(a,b) assert( a && b )

#ifdef _WIN32
  #define _ALIGN(sz) __declspec(align(sz))
  #define __align(...)           __declspec(align(__VA_ARGS__))
#else
  #define _ALIGN(...) __attribute__((aligned(__VA_ARGS__)))
  #define __align(...) __attribute__((aligned(__VA_ARGS__)))
#endif

#ifndef PI_DEFINED
  #define PI_DEFINED
  const double PI = 3.1415926535897932384626433832795;
#endif

#ifndef M_180_PI
  #define M_180_PI	57.295779513082320876798154814105f
#endif

#ifndef M_PI_180
  #define M_PI_180	0.017453292519943295769236907684886f
#endif

#ifndef M_1_PI
  #define M_1_PI   0.31830988618379067154f
#endif

#define RAD2DEG(a) (a*M_180_PI)
#define DEG2RAD(a) (a*M_PI_180)

static CVector2 NULLVECTOR2f(0.0f, 0.0f);
static CVector3 NULLVECTOR3f(0.0f, 0.0f, 0.0f);
static CVector3 VECTOR1_0_0(1.0f, 0.0f, 0.0f);
static CVector3 VECTOR0_1_0(0.0f, 1.0f, 0.0f);
static CVector3 VECTOR0_0_1(0.0f, 0.0f, 1.0f);
static CVector3 VECTOR1_1_1(1.0f, 1.0f, 1.0f);
static const int DEFAULT_RAY_MAX_DISTANCE = 100000.0f;
static const int DEBUG_STRING_SIZE = 16;
static const int NETWORK_PROTOCOL_VERSION = 10;
static const int NETWORK_PROTCOL_MAGICK = 123456;
const char * const MINRT_VERSION = "1.00";

enum class MultiThreadMethods {
  TASKDISPATCH = 1,
  OPENMP       = 2,
  OPENMPT      = 3,
  CILK         = 4,
};

class Engine {
  public:
    Engine(){};
    ~Engine(){};
    
    static void init(int argc, char *argv[]);

    // resolution ray tracer and openGL for diplaying
    static int screenWidthRT; 
    static int screenHeightRT;
    static int screenWidthGL; 
    static int screenHeightGL; 

    static int RENDERLINE_SIZE;

    // keys & mouse
    static bool upKey;
    static bool downKey; 
    static bool leftKey; 
    static bool rightKey;
    static bool leftButton;
    static bool rightButton;
    static bool jumpKey;
    static bool crouchKey;
    static bool done;

    static bool server;

    static float currentTime;

    static MultiThreadMethods methodToMultiThread;

    static CVector2i relativeMouseMotion;

    static bool dedicated;

    static bool debugLatency;
    static int debugNumCameraChanges;

    static int numFramesRendered;

    static float cg_fov;

    static float frameInterval;

    static float framesPerSecond;
    static float frameTime;

    static int embreeIntersect;

    // Networking stuff
    static TCPsocket sd, csd; // Socket descriptor, Client socket descriptor 
    static IPaddress ip, *remoteIP;

    static int serverPort;

    // rectangular image part instead of full image
    static int rectLeft;
    static int rectTop;
    static int rectRight;
    static int rectBottom;
    static bool rectMode;
    static int rectSizeX;
    static int rectSizeY;

    static int previousNumAccelerometerHits;
    static int numAccelerometerHits;
    static bool accelerometerChangedThisFrame;

    static void calculateFrameRate();

    static bool isPointInsideRect(int x, int y);

  private:		
    static float lastTime;				// This will hold the time from the last frame	
    static void parseArgv(int argc, char *argv[]);
};

#endif
