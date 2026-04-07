#include <stddef.h>
#include "data.h"
#include "defs.h"
#include "rtos_api.h"
#include "sys.h"

// Добавить задачу в конец очереди ожидающих семафора
static void SemPush(TSemaphore s, TTask task) {
  task->next = NULL;
  if (s->wait_tail == NULL) {
    s->wait_head = task;
  } else {
    s->wait_tail->next = task;
  }
  s->wait_tail = task;
}

// Извлечь задачу из начала очереди ожидающих семафора
static TTask SemPop(TSemaphore s) {
  TTask task = s->wait_head;
  if (task == NULL) return NULL;
  s->wait_head = task->next;
  if (s->wait_head == NULL) {
    s->wait_tail = NULL;
  }
  task->next = NULL;
  return task;
}

void InitPVS(TSemaphore s, int count) {
  if (s == NULL) return;
  s->count = count;
  s->wait_head = NULL;
  s->wait_tail = NULL;
  Log("InitPVS: semaphore initialized, count=%d\n", count);
}

void P(TSemaphore s) {
  if (s == NULL) return;
  Log("P: task=%s tries to acquire semaphore (count=%d)\n",
      CurrentTask->name, s->count);

  while (s->count == 0) {
    Log("P: task=%s blocked on semaphore\n", CurrentTask->name);
    SemPush(s, CurrentTask);
    Yield();
  }

  s->count--;
  Log("P: task=%s acquired semaphore (count now=%d)\n",
      CurrentTask->name, s->count);
}

void V(TSemaphore s) {
  if (s == NULL) return;

  TTask waiter = SemPop(s);
  if (waiter != NULL) {
    Log("V: task=%s releases semaphore, waking task=%s\n",
        CurrentTask->name, waiter->name);
    ActivateTask(waiter);
  } else {
    s->count++;
    Log("V: task=%s releases semaphore (count now=%d)\n",
        CurrentTask->name, s->count);
  }
}