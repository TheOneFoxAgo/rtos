#include <stdarg.h>

#include <stdio.h>
#include "rtos_api.h"
#include "sys.h"

DeclareTask(Setupper);
DeclareTask(Counter);
DeclareTask(Printer);
DeclareTask(Killer);

DeclareEvent(EV_ShuttingDown);

DeclareTask(Task1);
DeclareTask(Task2);
DeclareTask(Task3);
DeclareTask(Task4);
DeclareTask(Task5);
DeclareTask(Task6);
DeclareTask(Task7);
DeclareTask(Task8);
// DeclareTask(Task9);
DeclareTask(Task10);
DeclareTask(Watchdog);

DeclareEvent(EV_Event1);
DeclareEvent(EV_Event2);
DeclareEvent(EV_Event3);
DeclareEvent(EV_Event4);
DeclareEvent(EV_Event5);
DeclareEvent(EV_Event6);
DeclareEvent(EV_Event7);
DeclareEvent(EV_Event8);
DeclareEvent(EV_Event9);
DeclareEvent(EV_Event10);

TSemaphore sem_test_var;
TSemaphore SEM_Test = (TSemaphore)&sem_test_var;
void RunScenario(int i);
int main(void) {
  StartOS(Counter);
  for (int i = 0; i <= 5; ++i) {
    RunScenario(i);
    printf("\n--- Scenario %d completed ---\n", i);
  }
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

TASK(Task1, 1) {
  printf("Starting Task1...\n");

  ActivateTask(Task2);
  ActivateTask(Task3);

  P(SEM_Test);
  WaitEvent(EV_Event1 | EV_Event2);

  printf("Executing Task1 after context restore...\n");
  //    task1_restored = 1;

  V(SEM_Test);
  TerminateTask();
}

TASK(Task2, 2) {
  printf("Starting Task2...\n");

  SetEvent(Task1, EV_Event1 | EV_Event2);
  printf("Executing Task2...\n");

  TerminateTask();
}

TASK(Task3, 3) {
  printf("Starting Task3...\n");

  V(SEM_Test);
  printf("Executing Task3 after semaphore wakeup...\n");

  TerminateTask();
}

/*********************************/
/* Сценарий 1: ожидание события и пробуждение */
/*********************************/
TASK(Task4, 1) {
  printf("Starting Task4...\n");

  ActivateTask(Task5);
  WaitEvent(EV_Event3);
  printf("Executing Task4 after Event3...\n");

  TerminateTask();
}

TASK(Task5, 2) {
  printf("Starting Task5...\n");

  SetEvent(Task4, EV_Event3);
  printf("Executing Task5...\n");

  TerminateTask();
}

/*********************************/
/* Сценарий 2: событие установлено до WaitEvent */
/*********************************/
TASK(Task6, 1) {
  printf("Starting Task6...\n");

  SetEvent(Task6, EV_Event4);
  WaitEvent(EV_Event4);
  printf("Executing Task6 without blocking because Event4 was preset...\n");

  TerminateTask();
}

/*********************************/
/* Сценарий 3: повторная активация задачи */
/*********************************/
// TASK(Task9, 1) {
//   printf("Starting Task9...\n");
//   printf("Task9 intentionally performs repeated activation\n");

//   ActivateTask(Task9);

//   TerminateTask();
// }

/*********************************/
/* Сценарий 4: освобождение чужого семафора */
/*********************************/
TASK(Task7, 1) {
  printf("Starting Task7...\n");

  P(SEM_Test);
  ActivateTask(Task8);

  V(SEM_Test);
  TerminateTask();
}

TASK(Task8, 2) {
  printf("Starting Task8...\n");
  printf("Task8 intentionally tries to release a foreign semaphore\n");

  V(SEM_Test);

  TerminateTask();
}

/*********************************/
/* Сценарий 5: deadlock при ожидании события */
/*********************************/
TASK(Task10, 1) {
  printf("Starting Task10...\n");
  printf("Task10 intentionally enters deadlock waiting for Event10\n");

  WaitEvent(EV_Event10);

  TerminateTask();
}

TASK(Watchdog, 0) {  // Самый высокий приоритет
  // Подождать немного
  for (volatile int i = 0; i < 10000000; i++)
    ;

  printf("Watchdog: deadlock detected, forcing shutdown...\n");
  ShutdownOS();

  while (1)
    Yield();
}

void RunScenario(int scenario) {
  switch (scenario) {
    case 0:
      printf("==========================================\n");
      printf(
          "SCENARIO 0: activation, scheduling, event wait and semaphore "
          "wakeup\n");
      printf("==========================================\n");
      //            StartOS(Task1);
      break;
    case 1:
      printf("==========================================\n");
      printf("SCENARIO 1: waiting for event and waking the task\n");
      printf("==========================================\n");
      StartOS(Task4);
      break;
    case 2:
      printf("==========================================\n");
      printf("SCENARIO 2: event is set before WaitEvent\n");
      printf("==========================================\n");
      StartOS(Task6);
      break;
    case 3:
      // printf("==========================================\n");
      // printf("SCENARIO 3: repeated activation error\n");
      // printf("==========================================\n");
      // StartOS(Task9);
      break;
    case 4:
      printf("==========================================\n");
      printf("SCENARIO 4: wrong semaphore owner release\n");
      printf("==========================================\n");
      //            InitPVS(SEM_Test, 1);
      //            StartOS(Task7);
      break;
    case 5:
      printf("==========================================\n");
      printf("SCENARIO 5: deadlock while waiting for event\n");
      printf("==========================================\n");
      ActivateTask(Watchdog);
      StartOS(Task10);
      break;
    default:
      printf("Unknown scenario %d\n", scenario);
      break;
  }
}
