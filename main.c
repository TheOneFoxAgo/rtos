#define DEBUG
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
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

// Функции безопасной печати
void safe_print(const char* str) {
    while (*str) {
        write(STDOUT_FILENO, str, 1);
        str++;
    }
}

void safe_print_int(int num) {
    char buf[32];
    int i = 0;
    
    if (num == 0) {
        safe_print("0");
        return;
    }
    
    int temp = num;
    while (temp > 0) {
        buf[i++] = '0' + (temp % 10);
        temp /= 10;
    }
    
    while (i > 0) {
        write(STDOUT_FILENO, &buf[--i], 1);
    }
}

int main(void) {
    StartOS(Counter);
    return 0;
}

TASK(Setupper, 2) {
    safe_print("=== System Starting ===\n");
    
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
    safe_print("Counter: started\n");
    safe_print("Counter: started\n");
    
    int counter = 0;
    while (counter < 5) {
        counter++;
        safe_print("Counter: iteration ");
        safe_print_int(counter);
        safe_print("/5\n");

        // Даем сигнал Printer'у через семафор (разблокирует Printer)
        V(SEM_PrintSignal);
        
        // Имитация работы
        for (volatile int i = 0; i < 100000; i++);
    }
    
    safe_print("Counter: finished all iterations\n");
    
    // Устанавливаем событие завершения для Killer
    SetEvent(Killer, EV_CounterDone);
    
    // Завершаем задачу Counter
    TerminateTask();
}

TASK(Printer, 1) {
    safe_print("Printer: started, waiting for signals...\n");
    
    for (int i = 0; i < 5; i++) {
        // БЛОКИРУЮЩИЙ вызов - ждет сигнала от Counter
        // Не использует Yield! Планировщик сам переключит контекст
        P(SEM_PrintSignal);
        
        // Критическая секция (печать)
        P(SEM_PrintMutex);
        safe_print("Printer: Printing message ");
        safe_print_int(i + 1);
        safe_print("/5!\n");
        V(SEM_PrintMutex);
    }
    
    safe_print("Printer: finished all prints\n");
    
    while (1) {
        safe_print("Printing!\n");
        Yield();
    }
}

TASK(Killer, 3) {
    safe_print("Killer: waiting for counter completion event...\n");
    
    for (volatile int i = 0; i < 1000000; i++);
    // БЛОКИРУЮЩИЙ вызов - ждет события от Counter
    // Не использует Yield! Планировщик сам переключит контекст
    WaitEvent(EV_CounterDone);

    safe_print("Killer: shutdown signal received, shutting down OS...\n");
    ShutdownOS();
    
    while(1); // Никогда не выполнится
}
