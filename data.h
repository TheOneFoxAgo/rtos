#ifndef DATA_H
#define DATA_H

#include <stddef.h>
#include "setjmp.h"

// Здесь живут все наши структуры, чтобы не засорять rtos_api.h

// Все возможные состояния задачи.
enum TaskStatus {
  TASK_START = 0,  // Задачу ещё не запустили.
  TASK_REENTER,    // Задачу запускали, но она не завершилась.
};

// Тип задачи
// Можете свободно добавлять свои поля по необходимости.
// Только добавляйте их в конец, чтобы не сломать макрос TASK.
struct TaskStorage {
  const char* name;          // Имя задачи
  enum TaskStatus status;    // Статус
  void (*body)(void);        // Тело задачи
  size_t priority;           // Приоритет. И счётчик времени.
  struct TaskStorage* next;  // Следующая задача в очереди
  jmp_buf ctx;               // Сохранённый контекст задачи
  void* stack;               // Стек задачи.
};

typedef struct TaskStorage* TTask;

// наследие windows_rtos
// Скорее всего надо будет переделать, поскольку у нас семафоры
struct ResourceStorage {
  int task;
  int priority;
  char* name;
};

typedef struct ResourceStorage* TResource;

// Я думаю, принцип объявления данных понятен. Сделайте по аналогии.

#endif
