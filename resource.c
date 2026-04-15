#include <stddef.h>
#include <stdbool.h>
#include "data.h"
#include "logging.h"
#include "rtos_api.h"
#include "sys.h"


void InitPVS(TSemaphore s) {
  if (s == NULL) {
    Log("InitPVS: NULL semaphore pointer\n");
    return;
  }
  // Ресурс свободен.
  s->locked = false;
  Log("InitPVS: semaphore initialized. State: unlocked\n");
}

void P(TSemaphore s) {
  if (s == NULL) {
    Log("P: NULL semaphore pointer\n");
    return;
  }
  
  Log("P: task=%s tries to acquire semaphore (state: %s)\n", 
      CurrentTask ? CurrentTask->name : "unknown",
      s->locked ? "locked" : "unlocked");

  // Активное ожидание с Yield().
  // Не трогаем внутренние поля планировщика.
  while (s->locked) {
    Log("P: task=%s spinning on semaphore\n", 
        CurrentTask ? CurrentTask->name : "unknown");
    Yield();
  }
  
  // Захват семафора.
  s->locked = true;
  
  Log("P: task=%s acquired semaphore\n", 
      CurrentTask ? CurrentTask->name : "unknown");
}

void V(TSemaphore s) {
  if (s == NULL) {
    Log("V: NULL semaphore pointer\n");
    return;
  }
  
  Log("V: task=%s releases semaphore\n", 
      CurrentTask ? CurrentTask->name : "unknown");
  
  // Освобождение семафора.
  s->locked = false;
}