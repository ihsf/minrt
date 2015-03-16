#include "RT_TaskManager.h"

RT_TaskManager::RT_TaskManager(){
}

RT_TaskManager::~RT_TaskManager(){
}

void RT_TaskManager::deleteAllTasks(){
  for (int i = 0; i < (int)tasks.size(); i++){
    delete tasks[i];
    tasks[i] = NULL;
  }

  tasks.clear();
}

void RT_TaskManager::addTaskRenderTile(RT_TaskRenderTile* taskRenderTile){
  Assert(taskRenderTile, "RT_TaskManager::addTaskRenderTile");
  tasks.push_back(taskRenderTile);
}


