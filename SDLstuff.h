#ifndef _SDLSTUFF_H
#define _SDLSTUFF_H

#include "SDL/SDL.h"
#include "Engine.h"
#include "OpenGLstuff.h"
#include "RT_RayTracer.h"
#include <iostream>

using namespace std;

class SDLstuff{
	public:
		SDLstuff(RT_RayTracer* rayTracer_, OpenGLstuff* openglstuff_);
		~SDLstuff();

		void init();
		void checkEvents();

		void grabKeyAndMouse();
		void ungrabKeyAndMouse();

    inline SDL_Window* getMainWindow(){ return mainWindow; }

    RT_RayTracer* rayTracer;

	private:
		OpenGLstuff* openglstuff;
		SDL_Event event;
    SDL_Window* mainWindow;
    SDL_GLContext sdl_glContext;

		void handleKeyPressEvent(SDL_Keysym* keysym);
		void handleKeyReleaseEvent(SDL_Keysym* keysym);
    void handleMouseMotionEvent(SDL_MouseMotionEvent* mouseMotion);

		char title[16];
};

#endif
