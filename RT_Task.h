#ifndef _RT_TASK_H
#define _RT_TASK_H

#include <math.h>			
#include <stdio.h>	
#include "CVector3.h"
#include "Engine.h"

using namespace std;

class RT_Task {
  public:
    RT_Task();
    virtual ~RT_Task(){};

    virtual void run() = 0;

    void setDescription(const char* const taskDescription);

  private:
    char description[DEBUG_STRING_SIZE];
};

#endif
