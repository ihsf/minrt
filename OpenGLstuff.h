#ifndef _OPENGLSTUFF_H
#define _OPENGLSTUFF_H

#ifdef _WIN32
  #include <windows.h>
#endif

#include "Engine.h"
#include "WindowsHelper.h"
#include "RT_RayTracer.h"

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif
#include <SDL.h>					// for buffer swapping

class OpenGLstuff{
	public:
    OpenGLstuff(RT_RayTracer* rayTracer_);
		~OpenGLstuff();

    void init(SDL_Window* mainWindow_);

		void render();
		void swapBuffers();

	private:		
    SDL_Window* mainWindow;
    RT_RayTracer* rayTracer;

		void generateFramebufferTexture();

		void setVSync(int interval);
		unsigned int framebufferTexID;  
};


#endif
