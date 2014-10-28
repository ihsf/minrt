#include "SDLstuff.h"
#include "SDL/SDL_syswm.h"

SDLstuff::SDLstuff(RT_RayTracer* rayTracer_, OpenGLstuff* openglstuff_){
	this->rayTracer = rayTracer_;
	this->openglstuff = openglstuff_;
	
	mainWindow = NULL;
  sdl_glContext = NULL;
}

SDLstuff::~SDLstuff(){
  if(sdl_glContext)
    SDL_GL_DeleteContext(sdl_glContext);  
}

void SDLstuff::init(){
  if(!Engine::dedicated){
	  if( SDL_Init( SDL_INIT_VIDEO  | SDL_INIT_NOPARACHUTE  ) < 0 ) {
		  cerr << "Failed initializing SDL Video: " << SDL_GetError() << endl;
		  exit(1);
    }
	}

  int flagsForWindow = 0;
  if(Engine::dedicated){
    flagsForWindow = SDL_SWSURFACE;
  } else {
    flagsForWindow = SDL_WINDOW_OPENGL;
  }

	strcpy(title, "MinRT");
  sprintf(title, "MinRT %s", MINRT_VERSION);

  // move the window starting position a bit away from the upper left corner
  const int totalOffsetX = 6;
  const int totalOffsetY = 28;

  if(!Engine::dedicated){
    cout << "Creating SDL window with " << Engine::screenWidthGL << "x" << Engine::screenHeightGL << endl;
    mainWindow = SDL_CreateWindow(title, totalOffsetX, totalOffsetY, Engine::screenWidthGL, Engine::screenHeightGL, flagsForWindow);

    if (!mainWindow){
      cout << "SDLstuff::createMyWindow. No mainWindow: " << SDL_GetError() << endl;
      exit(1);
    }

    sdl_glContext = SDL_GL_CreateContext(mainWindow);
    if(!sdl_glContext){
      cout << "SDLstuff::createMyWindow. No mainWindow: " << SDL_GetError() << endl;
      exit(1);
    }
  }
}

void SDLstuff::checkEvents(){
  while(SDL_PollEvent(&event)){
    switch(event.type){     
      case SDL_KEYDOWN:                             
        handleKeyPressEvent(&event.key.keysym);        
        break;

      case SDL_KEYUP:
        handleKeyReleaseEvent(&event.key.keysym);         
        break;

      case SDL_MOUSEMOTION:
        handleMouseMotionEvent(&event.motion);
        break;

      default:
        break;
    }
	}
}

void SDLstuff::handleMouseMotionEvent(SDL_MouseMotionEvent* mouseMotion){
  Engine::relativeMouseMotion = CVector2i(mouseMotion->xrel, mouseMotion->yrel);
}

void SDLstuff::handleKeyPressEvent(SDL_Keysym* keysym){  
	SDL_Keycode key = keysym->sym;
    
	switch(key){
    case SDLK_ESCAPE:                                  // if it is ESCAPE
				Engine::done = true;		
			break;

    case SDLK_UP:                                     // If we hit the UP arrow Key
    case SDLK_w:
      Engine::upKey = true;      
      break;
        
    case SDLK_DOWN:                                    // If we hit the Down arrow Key
    case SDLK_s:
		  Engine::downKey = true; 
      break;

    case SDLK_RIGHT:                                   // If we hit the right arrow Key
    case SDLK_d:
      Engine::leftKey = true;
      break;

    case SDLK_LEFT:                                    // If we hit the left arrow key
    case SDLK_a:
      Engine::rightKey = true; 
      break;

	  case SDLK_SPACE:
		  Engine::jumpKey = true;
			break;

	  case SDLK_c:
		  Engine::crouchKey = true;
			break;

    default:
      break;
  }
}

void SDLstuff::handleKeyReleaseEvent(SDL_Keysym* keysym){
	SDL_Keycode key = keysym->sym;

	switch(key) {
		case SDLK_UP:                       
		case SDLK_w:
			Engine::upKey = false;       
			break;
            
		case SDLK_DOWN:                    
		case SDLK_s:
			Engine::downKey = false;     
			break;

		case SDLK_RIGHT:                    
		case SDLK_d:
			Engine::leftKey = false;    
			break;

		case SDLK_LEFT:                    
		case SDLK_a:
			Engine::rightKey = false;   
			break;

		case SDLK_SPACE:
			Engine::jumpKey = false;
			break;

		case SDLK_c:
			Engine::crouchKey = false;
			break;

		default:                     
			break;                    
  }
}

void SDLstuff::grabKeyAndMouse(){
  SDL_SetRelativeMouseMode(SDL_TRUE);
}

void SDLstuff::ungrabKeyAndMouse(){
  SDL_SetRelativeMouseMode(SDL_FALSE);
}

