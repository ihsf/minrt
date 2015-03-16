#include "RT_Task.h"

RT_Task::RT_Task(){
  description[0] = 0;
}

void RT_Task::setDescription(const char* const taskDescription){
  Assert(taskDescription, "RT_Task::setDescription");
  strncpy(description, taskDescription, DEBUG_STRING_SIZE - 1);
}
