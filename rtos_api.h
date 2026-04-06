#ifndef RTOS_API_H
#define RTOS_API_H

#include <stdnoreturn.h>
#include "data.h"

// Включить логирование.
extern int EnableLogging;

// Объявить новую задачу.
// Этот макрос объявит глобальную
// переменную с названием TaskID.
#define DeclareTask(TaskID) TTask TaskID;

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
void ActivateTask(TTask);

// Завершить задачу.
// Уберёт задачу из очереди. Когда-нибудь. Никаких гарантий не даю.
// ОБЯЗАТЕЛЬНО ВЫЗЫВАЙТЕ В КОНЦЕ ЗАДАЧИ! Забудете - всё. Уб.
noreturn void TerminateTask(void);

// Запустить ОС начав с задачи task.
int StartOS(TTask);

// Завершить работу ОС. Все ждущие задачи будут сняты без предупреждения.
noreturn void ShutdownOS(void);

// Внутренняя функция регистрации события (не для прямого вызова)
TEventMask __GenNewEventMask(void);

// Регистрирует событие и создаёт константу с уникальным битом в маске
#define DeclareEvent(EventID) \
  static const TEventMask EventID = (TEventMask)(1 << __COUNTER__)

// Устанавливает для указанной задачи события, задаваемые маской.
// Любые события, не заданные в маске, останутся неизменны.
void SetEvent(TTask, TEventMask);

// Возвращает текущее состояние маски установленных событий
// для указанной задачи (но не событий, которые задача ждет).
void GetEvent(TTask, TEventMask*);

// Если не установлено ни одного события, заданного маской,
// то текущая задача переводится в состояние ожидания.
// При выходе сбрасываются те события, которые вызвали этот выход.
void WaitEvent(TEventMask);

// Производит очистку установленных событий текущей задачи
void ClearEvent(TEventMask);

#endif
