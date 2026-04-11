#define DEBUG

#include <stdarg.h>
#include <stdio.h>
#include "rtos_api.h"
#include "sys.h"

DeclareTask(Setupper);
DeclareTask(Counter);
DeclareTask(Printer);
DeclareTask(Killer);

DeclareEvent(EV_CounterDone);

TSemaphore sem_print_mutex_var;
TSemaphore sem_print_signal_var;
TSemaphore SEM_PrintMutex = (TSemaphore)&sem_print_mutex_var;
TSemaphore SEM_PrintSignal = (TSemaphore)&sem_print_signal_var;

int main(void) {
    StartOS(Setupper);
    return 0;
}

TASK(Setupper, 2) {
    printf("=== System Starting ===\n");
    
    InitPVS(SEM_PrintMutex, 1);
    InitPVS(SEM_PrintSignal, 0);
    
    ActivateTask(Counter);
    ActivateTask(Printer);
    ActivateTask(Killer);
    
    // Вместо TerminateTask - бесконечный цикл с самым низким приоритетом
    while(1) {
        Yield();
    }
}

TASK(Counter, 1) {
    printf("Counter: started\n");
    
    int counter = 0;
    while (counter < 5) {
        counter++;
        printf("Counter: iteration %d/5\n", counter);
        V(SEM_PrintSignal);
        
        for (volatile int i = 0; i < 100000; i++);
    }
    
    printf("Counter: finished all iterations\n");
    SetEvent(Killer, EV_CounterDone);
    
    // Понижаем приоритет до самого низкого и крутимся
    while(1) {
        Yield();
    }
}

TASK(Printer, 1) {
    printf("Printer: started, waiting for signals...\n");
    
    for (int i = 0; i < 5; i++) {
        P(SEM_PrintSignal);
        P(SEM_PrintMutex);
        printf("Printer: Printing message %d/5!\n", i + 1);
        V(SEM_PrintMutex);
    }
    
    printf("Printer: finished all prints\n");
    
    while(1) {
        Yield();
    }
}

TASK(Killer, 3) {
    printf("Killer: waiting for counter completion event...\n");
    
    WaitEvent(EV_CounterDone);
    
    printf("Killer: shutdown signal received, shutting down OS...\n");
    ShutdownOS();
    
    while(1);
}
