#ifndef _NETWORKSTUFF_H
#define _NETWORKSTUFF_H

#include "Engine.h"
#include "SDLstuff.h"
#include "Camera.h"
#include "RT_RayTracer.h"

using namespace std;

struct clientMessage{
  int magic;
  int clientMessageLength;
  int protocolVersion;

  CVector3 cameraPosition;
  CVector3 cameraView;
  CVector3 cameraUp;

  int screenWidthRT;
  int screenHeightRT;

  int rectLeft;
  int rectTop;
  int rectRight;
  int rectBottom;

  float currentTime;
  int frameNr;

  bool doExit;
};

class NetworkStuff {
  public:
    NetworkStuff(Camera* camera_, RT_RayTracer* rayTracer);
    ~NetworkStuff();

    void shutdown();

    void sendMessageToGameClient();
    void receiveMessageFromGameClient();

  private:
    Camera* camera;
    RT_RayTracer* rayTracer;

    unsigned char* outputBuffer;
    unsigned char* frameBufferCopy;
    char* lz4Buf;

    int numBytesToSend;

    void init();
    void determineNumBytesToSend();
    void receiveInitPacket();

    void sendMessageToGameClientETC1noThread();

    void copyRect(unsigned char* copiedBuffer, unsigned char* frameBuffer);
};

#endif
