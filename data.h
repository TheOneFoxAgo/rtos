#ifndef DATA_H
#define DATA_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// Здесь живут все наши структуры, чтобы не засорять rtos_api.h

// Все возможные состояния задачи.
enum TaskStatus {
  TASK_START = 0,  // Задачу ещё не запустили.
  TASK_REENTER,    // Задачу запускали, но она не завершилась.
  TASK_TERMINATE,  // Пришло время убить задачу.
};

// Тип данных для маски событий
typedef uint16_t TEventMask;

// Тип задачи
// Можете свободно добавлять свои поля по необходимости.
// Только добавляйте их в конец, чтобы не сломать макрос TASK.
struct TaskStorage {
  const char* name;           // Имя задачи.
  enum TaskStatus status;     // Статус.
  void (*body)(void);         // Тело задачи.
  size_t priority;            // Приоритет. И счётчик времени.
  struct TaskStorage* next;   // Следующая задача в очереди.
  void* stack;                // Стек задачи.
  void* mem;                  // Указатель выделенный malloc.
  TEventMask pending_events;  // Установленные события.
};

typedef struct TaskStorage* TTask;

// Простой семафор (P/V).
struct SemaphoreStorage {
  bool locked;                // true - занят, false - свободен
};

typedef struct SemaphoreStorage* TSemaphore;

#endif