#include "NetworkStuff.h"

NetworkStuff::NetworkStuff(Camera* camera_, OpenGLstuff* openglstuff_, RT_RayTracer* rayTracer_){
	this->camera = camera_;
	this->openglstuff = openglstuff_;
	this->rayTracer = rayTracer_;

	if(!Engine::server)
		return; 

	init();
	determineNumBytesToSend();

  // FIXME. Try without the HACK. HACK *2
	outputBuffer = new unsigned char[numBytesToSend * 2];
	frameBufferCopy = new unsigned char[Engine::screenWidthRT * Engine::screenHeightRT * 4];
}

NetworkStuff::~NetworkStuff(){
}

void NetworkStuff::init() {
	if (SDLNet_Init() < 0){
		cout << "SDLNet_Init server: Failed\n" << endl;
		exit(EXIT_FAILURE);
	}
 
	if (SDLNet_ResolveHost(&Engine::ip, NULL, Engine::serverPort) < 0){
		cout << "SDLNet_ResolveHost: Failed\n" << endl;
		exit(EXIT_FAILURE);
	}
 
	if (!(Engine::sd = SDLNet_TCP_Open(&Engine::ip))){
		cout << "SDLNet_TCP_Open: Failed\n" << endl;
		exit(EXIT_FAILURE);	
	}

  cout << "Network protocol version: " << NETWORK_PROTOCOL_VERSION << endl;
	cout << "SDLNet_TCP_Accept: Waiting for client\n" << endl;
	while (!(Engine::csd = SDLNet_TCP_Accept(Engine::sd))) {
		;		 
	}

	if (!(Engine::remoteIP = SDLNet_TCP_GetPeerAddress(Engine::csd))) {
		cout << "SDLNet_TCP_GetPeerAddress: Failed\n" << endl;
		exit(EXIT_FAILURE);
	} else {
		printf("Host connected: %x %d\n", SDLNet_Read32(&Engine::remoteIP->host), SDLNet_Read16(&Engine::remoteIP->port));
	}	

	receiveInitPacket();
}

void NetworkStuff::receiveInitPacket(){
	clientMessage msgBuffer;
	memset(&msgBuffer, 0, sizeof(clientMessage));

	int length = sizeof(msgBuffer);
	int receivedLength = SDLNet_TCP_Recv(Engine::csd, (unsigned char *)&msgBuffer, sizeof(msgBuffer));

	if(length != receivedLength){
		cout << "NetworkStuff::receiveInitPacket: Did not receive full init packet!" << endl;
		exit(1);
	}

  if(msgBuffer.magic != NETWORK_PROTCOL_MAGICK){
    cout << "Network magick header should be " << NETWORK_PROTCOL_MAGICK << ", but is " << msgBuffer.magic << endl;
    exit(1);
  }

  if(msgBuffer.protocolVersion != NETWORK_PROTOCOL_VERSION){
    cout << "Network protcol should be " << NETWORK_PROTOCOL_VERSION << ", but is " << msgBuffer.protocolVersion << endl;
    exit(1);
  }

  if( (msgBuffer.screenWidthRT != Engine::screenWidthRT)  || (msgBuffer.screenHeightRT != Engine::screenHeightRT)){
    cout << "ERROR: screen width/height is different between client and server. Does not work in yet. Please change manually." << endl;
    exit(1);
  }

	Engine::screenWidthGL = Engine::screenWidthRT = msgBuffer.screenWidthRT;
	Engine::screenHeightGL = Engine::screenHeightRT = msgBuffer.screenHeightRT;

  int rectSizeX = msgBuffer.rectRight - msgBuffer.rectLeft + 1;  // +1 to count in pixel 0
  int rectSizeY = msgBuffer.rectTop -  msgBuffer.rectBottom + 1;
  if( rectSizeX > Engine::RENDERTILE_SIZE && rectSizeY > Engine::RENDERTILE_SIZE){
    int remainingPixelsX = rectSizeX % Engine::RENDERTILE_SIZE;
    int remainingPixelsY = rectSizeY % Engine::RENDERTILE_SIZE;

    if(remainingPixelsX == 0 && remainingPixelsY == 0){
      Engine::rectMode = true;
      Engine::rectLeft = msgBuffer.rectLeft;
      Engine::rectTop = msgBuffer.rectTop;
      Engine::rectRight = msgBuffer.rectRight;
      Engine::rectBottom = msgBuffer.rectBottom;
      Engine::rectSizeX = rectSizeX;
      Engine::rectSizeY = rectSizeY;
      cout << "==============================" << endl;
      cout << "Rendering Rect enabled: X (" << Engine::rectLeft << " - " << Engine::rectRight << ")   Y (" << Engine::rectBottom << " - " << Engine::rectTop << ")" << endl;
      cout << "==============================" << endl;
    }
  }
}

void NetworkStuff::shutdown(){
	if (Engine::server) {
		SDLNet_TCP_Close(Engine::csd);
		SDLNet_TCP_Close(Engine::sd);
		SDLNet_Quit();
	}
}

void NetworkStuff::determineNumBytesToSend(){
  if(!Engine::rectMode){
    numBytesToSend = (Engine::screenWidthRT *  Engine::screenHeightRT) / 2;
  } else {
    numBytesToSend = (Engine::rectSizeX *  Engine::rectSizeY) / 2;
  }

  cout << "NetworkStuff::numBytesToSend: " << numBytesToSend << endl;
}

void NetworkStuff::receiveMessageFromGameClient(){
	// Check for messages from Client
	clientMessage msgBuffer;
	memset(&msgBuffer, 0, sizeof(clientMessage));

	int length = sizeof(msgBuffer);
	if (SDLNet_TCP_Recv(Engine::csd, (unsigned char *)&msgBuffer, sizeof(msgBuffer)) < sizeof(msgBuffer)) {
		Engine::done = true;
		return;
	}

	camera->setPosition(msgBuffer.cameraPosition);
	camera->setView(msgBuffer.cameraView);
  camera->setUpVector(msgBuffer.cameraUp);
	Engine::currentTime = msgBuffer.currentTime;

	Engine::numFramesRendered = msgBuffer.frameNr;
  Engine::done = msgBuffer.doExit;

  srand(Engine::currentTime);
}

void NetworkStuff::copyRect(unsigned char* copiedBuffer, unsigned char* frameBuffer){
  unsigned char* copiedBufferPointer = copiedBuffer;
  for(int y = Engine::rectBottom; y < Engine::rectTop; y++){
    memcpy(copiedBufferPointer, &(frameBuffer[Engine::rectLeft * 4 + Engine::screenWidthRT * 4 * y]), Engine::rectSizeX * 4);
    copiedBufferPointer += Engine::rectSizeX * 4;
  }
}


void NetworkStuff::sendMessageToGameClient(){
  //WindowsHelper::getMsElapsed();

  rayTracer->renderFrame();

  //float timeForRendering = WindowsHelper::getMsElapsed();  

  //WindowsHelper::getMsElapsed();

  if(!Engine::rectMode){
    memcpy(frameBufferCopy, rayTracer->getFrameBuffer(), Engine::screenWidthRT * Engine::screenHeightRT * 4);
    Etc1Fast::convertRGBAtoETC1(outputBuffer, frameBufferCopy, Engine::screenWidthRT, Engine::screenHeightRT);
  } else {
    copyRect(frameBufferCopy, rayTracer->getFrameBuffer());
    Etc1Fast::convertRGBAtoETC1(outputBuffer, frameBufferCopy, Engine::rectSizeX, Engine::rectSizeY);
  }  

  //float etc1EncodeTime = WindowsHelper::getMsElapsed();

  int fbBytesSent = SDLNet_TCP_Send(Engine::csd, outputBuffer, numBytesToSend);
}






