#ifndef RTOS_API_H
#define RTOS_API_H

#include <stdnoreturn.h>
#include "data.h"

// Объявить новую задачу.
// Этот макрос объявит глобальную
// переменную с названием TaskID.
#define DeclareTask(TaskID) TTask TaskID;

// Наследие Windows RTOS.
#define DeclareResource(ResID, priority) enum { ResID = priority }

// Определить ранее объявленную задачу.
// Супер хитрый макрос.
// Объявляет функцию __TaskID_body Определяет "хранилище" для
// состояния TaskID (__TaskID_storage) и записывает указатель
// на него в ранее объявленную переменную TaskID. Ну и в конце
// концов начинает определять Функцию __TaskID_body
#define TASK(TaskID, prior)                   \
  void __##TaskID##_body(void);               \
  struct TaskStorage __##TaskID##_storage = { \
      #TaskID,                                \
      TASK_START,                             \
      __##TaskID##_body,                      \
      prior,                                  \
  };                                          \
  TTask TaskID = &__##TaskID##_storage;       \
  noreturn void __##TaskID##_body(void)

// Активировать задачу.
// Сразу же поставит задачу в очередь на исполнение.
// Но начнёт исполнение она не сразу. Когда-нибудь.
void ActivateTask(TTask task);

// Завершить задачу.
// Уберёт задачу из очереди. Когда-нибудь. Никаких гарантий не даю.
// ОБЯЗАТЕЛЬНО ВЫЗЫВАЙТЕ В КОНЦЕ ЗАДАЧИ! Забудете - всё. Уб.
noreturn void TerminateTask(void);

// Запустить ОС начав с задачи task.
int StartOS(TTask task);

// Завершить работу ОС. Все ждущие задачи будут сняты без предупреждения.
noreturn void ShutdownOS(void);

// Наследие Windows RTOS. У этих функций кста интерфейс кривой,
// поскольку макросы чёрт знает как написаны. Можно написать нормально,
// но во-первых, я ресурсами не занимаюсь, а во-вторых, у нас ваще семафоры,
// а не ресурсы.
void GetResource(int priority, char* name);
void ReleaseResource(int priority, char* name);

#endif
