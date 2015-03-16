#ifndef _RT_TASKMANAGER_H
#define _RT_TASKMANAGER_H

#include <math.h>			
#include <stdio.h>	
#include "Engine.h"
#include "RT_Task.h"
#include "RT_TaskRenderTile.h"

using namespace std;

class RT_TaskManager {
  public:
    RT_TaskManager();
    ~RT_TaskManager();

    void addTaskRenderTile(RT_TaskRenderTile* taskRenderTile);
    void deleteAllTasks();
    vector<RT_Task*> tasks;
};

#endif
