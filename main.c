#define DEBUG

#include <stdarg.h>
#include <stdio.h>
#include "rtos_api.h"
#include "sys.h"

DeclareTask(Setupper);
DeclareTask(Counter);
DeclareTask(Printer);
DeclareTask(Killer);

// Объявляем события
DeclareEvent(EV_CounterDone);

// Объявление семафоров
TSemaphore sem_print_mutex_var;
TSemaphore sem_print_signal_var;
TSemaphore SEM_PrintMutex = (TSemaphore)&sem_print_mutex_var;
TSemaphore SEM_PrintSignal = (TSemaphore)&sem_print_signal_var;

int main(void) {
    StartOS(Counter);
    return 0;
}

TASK(Setupper, 2) {
    printf("=== System Starting ===\n");
    
    // Инициализация семафоров
    InitPVS(SEM_PrintMutex, 1);   // Мьютекс (свободен)
    InitPVS(SEM_PrintSignal, 0);   // Сигнал Printer'у (заблокирован)
    
    ActivateTask(Counter);
    ActivateTask(Printer);
    ActivateTask(Killer);
    
    // Setupper завершает свою работу
    TerminateTask();
}

TASK(Counter, 1) {
    printf("Counter: started\n");
    
    int counter = 0;
    while (counter < 5) {
        counter++;
        printf("Counter: iteration %d/5\n", counter);
        
        // Даем сигнал Printer'у через семафор (разблокирует Printer)
        V(SEM_PrintSignal);
        
        // Имитация работы
        for (volatile int i = 0; i < 100000; i++);
    }
    
    printf("Counter: finished all iterations\n");
    
    // Устанавливаем событие завершения для Killer
    SetEvent(Killer, EV_CounterDone);
    
    // Завершаем задачу Counter
    TerminateTask();
}

TASK(Printer, 1) {
    printf("Printer: started, waiting for signals...\n");
    
    for (int i = 0; i < 5; i++) {
        // БЛОКИРУЮЩИЙ вызов - ждет сигнала от Counter
        // Не использует Yield! Планировщик сам переключит контекст
        P(SEM_PrintSignal);
        
        // Критическая секция (печать)
        P(SEM_PrintMutex);
        printf("Printer: Printing message %d/5!\n", i + 1);
        V(SEM_PrintMutex);
    }
    
    printf("Printer: finished all prints\n");
    
  while (1) {
    printf("Printing!\n");
    Yield();
  }
}

TASK(Killer, 3) {
    printf("Killer: waiting for counter completion event...\n");
    
    // БЛОКИРУЮЩИЙ вызов - ждет события от Counter
    // Не использует Yield! Планировщик сам переключит контекст
    WaitEvent(EV_CounterDone);
    
    printf("Killer: shutdown signal received, shutting down OS...\n");
    ShutdownOS();
    
    while(1); // Никогда не выполнится
}
