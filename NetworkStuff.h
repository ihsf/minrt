#ifndef _NETWORKSTUFF_H
#define _NETWORKSTUFF_H

#include "Engine.h"
#include "SDLstuff.h"
#include "Camera.h"
#include "OpenGLstuff.h"
#include "RT_RayTracer.h"
#include "Etc1Fast.h"

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
		NetworkStuff(Camera* camera_, OpenGLstuff* openglstuff, RT_RayTracer* rayTracer);
		~NetworkStuff();

		void shutdown();

		void sendMessageToGameClient();
		void receiveMessageFromGameClient();

	private:
		Camera* camera;
		OpenGLstuff* openglstuff;
		RT_RayTracer* rayTracer;
		
		unsigned char* outputBuffer;
		unsigned char* frameBufferCopy;

		int numBytesToSend;

		void init();
		//void initPThread();
		void determineNumBytesToSend();		
		void receiveInitPacket();

    void sendMessageToGameClientETC1noThread();
    //void sendMessageToGameClientFFMPEG();

    void copyRect(unsigned char* copiedBuffer, unsigned char* frameBuffer);
};

#endif
