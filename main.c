#define DEBUG  // Штоб печатало

#include <stdarg.h>
#include <stdio.h>
#include "rtos_api.h"
#include "sys.h"

DeclareTask(Setupper);
DeclareTask(Counter);
DeclareTask(Printer);
DeclareTask(Killer);

DeclareEvent(EV_ShuttingDown);

int main(void) {
  StartOS(Counter);
  return 0;
}

TASK(Setupper, 2) {
  ActivateTask(Counter);
  ActivateTask(Printer);
  ActivateTask(Killer);
  TerminateTask();
}

TASK(Counter, 1) {
  printf("Starting counter!\n");
  int counter = 0;
  while (counter < 5) {
    counter += 1;
    Yield();
  }
  ActivateTask(Killer);
  // SetEvent(Killer, EV_ShuttingDown);
  TerminateTask();  // Не забывайте в конце каждой таски.
}

TASK(Printer, 1) {
  while (1) {
    printf("Printing!\n");
    Yield();
  }  // Я думаю понятно, почему тут нету TerminateTask
}

TASK(Killer, 3) {
  // WaitEvent(EV_ShuttingDown);
  ShutdownOS();  // Ну это исключение. После Shutdown можно таску и не
                 // завершать.
}
