#include <stddef.h>
#include "data.h"
#include "defs.h"
#include "logging.h"
#include "rtos_api.h"
#include "sys.h"

void SetEvent(TTask task, TEventMask mask) {
  if (task == NULL || mask == EMPTY_MASK)
    return;

  task->pending_events |= mask;
  Log("Set events to task=%s: mask=0x%04X\n", task->name, mask);
}

void GetEvent(TTask task, TEventMask* event) {
  if (task == NULL || event == NULL)
    return;

  *event = task->pending_events;
  Log("Got events from task=%s: mask=0x%04X\n", task->name, *event);
}

void WaitEvent(TEventMask mask) {
  if (mask == EMPTY_MASK)
    return;

  TTask current = CurrentTask;
  Log("Waiting events on task=%s: mask=0x%04X\n", current->name, mask);

  TEventMask occurred = current->pending_events & mask;
  while ((occurred & mask) == 0) {
    Yield();
    occurred = current->pending_events & mask;
  }

  current->pending_events &= ~occurred;
  Log("Events occurred on task=%s: mask=0x%04X\n", current->name, occurred);
}

void ClearEvent(TEventMask mask) {
  if (mask == EMPTY_MASK)
    return;

  TTask current = CurrentTask;
  current->pending_events &= ~mask;
  Log("Cleared events on task=%s: mask 0x%04X\n", current->name, mask);
}
