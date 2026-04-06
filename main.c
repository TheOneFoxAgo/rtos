#include <stdarg.h>
#include <stdio.h>
#include "rtos_api.h"
#include "sys.h"

DeclareTask(Counter);
DeclareTask(Printer);
DeclareTask(Killer);

int main(void) {
  EnableLogging = 0;
  StartOS(Counter);
  return 0;
}

TASK(Counter, 1) {
  int counter = 0;
  ActivateTask(Printer);
  while (counter < 5) {
    counter += 1;
    Yield();
  }
  ActivateTask(Killer);
  TerminateTask();  // Не забывайте в конце каждой таски.
}

TASK(Printer, 1) {
  while (1) {
    printf("Printing!\n");
    Yield();
  }  // Я думаю понятно, почему тут нету TerminateTask
}

TASK(Killer, 3) {
  ShutdownOS();  // Ну это исключение. После Shutdown можно таску и не
                 // завершать.
}
