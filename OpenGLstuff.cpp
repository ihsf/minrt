#include "OpenGLstuff.h"

OpenGLstuff::OpenGLstuff(RT_RayTracer* rayTracer_){
  this->rayTracer = rayTracer_;
  mainWindow = NULL;
}

OpenGLstuff::~OpenGLstuff(){
}

void OpenGLstuff::init(SDL_Window* mainWindow_){
  this->mainWindow = mainWindow_;

  if(Engine::dedicated)
    return;

  // Init OpenGL Camera
  glViewport(0, 0, Engine::screenWidthGL, Engine::screenHeightGL);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(0,1,0,1,-1,1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glEnable(GL_TEXTURE_2D);

  generateFramebufferTexture();

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  setVSync(0);

  cout << "OpenGL initialized." << endl;
}

void OpenGLstuff::generateFramebufferTexture(){
  if(Engine::dedicated)
    return;

  // Generate texture
  glGenTextures(1, &framebufferTexID);

  for (int i = 0; i < 2; i++) {
    glBindTexture(GL_TEXTURE_2D, framebufferTexID);
     
    // Here we bind the texture and set up the filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);			

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Engine::screenWidthRT, Engine::screenHeightRT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    
    glBindTexture(GL_TEXTURE_2D, 0);
  }
}

void OpenGLstuff::render(){
  if(Engine::dedicated)
    return;

  glEnable(GL_TEXTURE_2D);

  glLoadIdentity();

  // draw ray traced rectangle 
  glBindTexture(GL_TEXTURE_2D, framebufferTexID); 
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Engine::screenWidthRT, Engine::screenHeightRT, 0, GL_RGBA, GL_UNSIGNED_BYTE, rayTracer->getFrameBuffer());

  glColor3f(1.0f, 1.0f, 1.0f);

  glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(0.0f, 1.0f);

    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(1.0f, 1.0f);

    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(1.0f, 0.0f);

    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(0.0f, 0.0f);
  glEnd();
}


void OpenGLstuff::swapBuffers(){
  if(Engine::dedicated)
    return;

  SDL_GL_SwapWindow(mainWindow);
}

void OpenGLstuff::setVSync(int interval){
  if(Engine::dedicated)
    return;

#ifdef _WIN32
  typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALFARPROC)(int); // Windows specfic to enable vsync turn off 
  PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;

  const char *extensions = (char *)(glGetString(GL_EXTENSIONS));

  if(!extensions)
    return;

  if(strstr(extensions, "WGL_EXT_swap_control") == 0 ){
    return; 
  } else {
    wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress("wglSwapIntervalEXT");
  }

  if(wglSwapIntervalEXT)
    wglSwapIntervalEXT(interval);
#endif
}
