#include <setjmp.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include "data.h"
#include "defs.h"
#include "rtos_api.h"
#include "sys.h"
// Кишки планировщика задач. Тут треш и угар.

static size_t PriorityCounterAnchor;  // Значение счётчика для новых задач
static jmp_buf Scheduler;             // Контекст планировщика.

static size_t CurrentPriority;  // Приоритет текущей задачи.

static TTask TasksHeads[MAX_PRIORITIES];  // Массив начал списков задач.
static TTask TasksTails[MAX_PRIORITIES];  // Массив концов списков задач.

enum SchedulerSignal {
  SCHEDULER_INIT = 0,
  SCHEDULER_PICK_NEXT,          // Выполнить следующую задачу
  SCHEDULER_TERMINATE_CURRENT,  // Завершить текущую задачу
  SCHEDULER_EXIT,               // Выйти из планировщика
};

void Yield(void) {
  if (setjmp(CurrentTask->ctx) == 0) {
    longjmp(Scheduler, SCHEDULER_PICK_NEXT);
  }
}

// Волшебная функция. Скорее черная магия.
// 1 аргумент - указатель на новый стек.
// 2 аргумент - указатель на функцию, которую вызовем. С новым стеком.
extern noreturn void StackSwitch(void* stack, void (*function)(void));

// Вставить в конец очереди задач. Если next не null - уб
static void Push(TTask task, size_t priority) {
  if (TasksTails[priority] == NULL) {
    TasksHeads[priority] = task;
  } else {
    TasksTails[priority]->next = task;
  }
  TasksTails[priority] = task;
}

// Вытащить задачу из очереди. Если очередь пуста - Уб.
static TTask Pop(size_t priority) {
  TTask previous_head = TasksHeads[priority];
  if (previous_head->next == NULL) {
    TasksTails[priority] = NULL;
  }
  TasksHeads[priority] = previous_head->next;
  previous_head->next = NULL;
  return previous_head;
}

// Прокрутить очередь вперёд.
static void Cycle(size_t priority) {
  if (TasksHeads[priority] != TasksTails[priority]) {
    Push(Pop(priority), priority);
  }
}

// Сбрасываем задачу в изначальное состояние и освобождаем ресурсы
static void ClearTask(TTask task, size_t priority) {
  task->status = TASK_START;
  task->priority = priority;
  task->next = NULL;
  free(task->stack);
  task->stack = NULL;
}

// Увеличиваем счётчик времени
static void IncreaseTimeCounter(TTask task, size_t priority) {
  // Сбрасываем все счётчики, если хотя б кто-то
  // близко подобрался. Уроки Касилова пошли на пользу
  if (SIZE_MAX - 1 - priority <= task->priority) {
    for (size_t i = 0; i < MAX_PRIORITIES; ++i) {
      TTask task = TasksHeads[i];
      while (task != NULL) {
        task->priority = 0;
        task = task->next;
      }
    }
    PriorityCounterAnchor = 0;
  } else {
    task->priority += 1 + priority;
    PriorityCounterAnchor = task->priority;
  }
}

// Выбирает следующую задачу и приоритет.
// Выбранная задача гарантированно на вершине очереди.
static TTask PickNext(size_t* out_priority) {
  TTask best = NULL;
  size_t priority = 0;
  for (size_t i = 0; i < MAX_PRIORITIES; ++i) {
    TTask task = TasksHeads[i];
    if (best == NULL) {
      best = task;
      priority = i;
    } else if (task != NULL && task->priority < best->priority) {
      best = task;
      priority = i;
      break;
    }
  }
  *out_priority = priority;
  return best;
}

// Начать исполнение задач.
// Я готов лекцию на 2 часа выдать, как эта балембала работает.
void StartScheduler(void) {
  Log("Scheduler start\n");
  switch (setjmp(Scheduler)) {
    case SCHEDULER_TERMINATE_CURRENT: {
      // Гарантированно CurrentTask находится на вершине списка
      // Pop() нарушит эту гарантию только временно. Скоро восстановим.
      ClearTask(Pop(CurrentPriority), CurrentPriority);
      Log("Task %s is terminated\n", CurrentTask->name);
      // Обнуляем, чтобы дать понять, что не надо крутить очередь
      CurrentTask = NULL;
    }  // Умышленно нету break
    case SCHEDULER_INIT:
    case SCHEDULER_PICK_NEXT: {
      if (CurrentTask != NULL) {
        // Таску не выключили, а значит надо её отправить в конец.
        Cycle(CurrentPriority);
      }
      // Восстанавливаем гарантию, что CurrentTask всегда в начале своего
      // списка.
      CurrentTask = PickNext(&CurrentPriority);
      if (CurrentTask != NULL) {  // Если вернули NULL, значит задачи кончились.
        IncreaseTimeCounter(CurrentTask, CurrentPriority);
        if (CurrentTask->status == TASK_START) {
          Log("Starting task: %s\n", CurrentTask->name);
          CurrentTask->status = TASK_REENTER;
          CurrentTask->stack = malloc(STACK_SIZE);
          // Стек растёт вниз, поэтому отдаём конец стека, а не начало
          StackSwitch(CurrentTask->stack + STACK_SIZE - 1, CurrentTask->body);
        } else {
          Log("Reentering task: %s\n", CurrentTask->name);
          longjmp(CurrentTask->ctx, 1);  // Число ни на что не влияет.
        }
      }
    } break;
    case SCHEDULER_EXIT: {
      Log("Scheduler cleanup\n");
      // Очищаем стеки, чтобы не было утечек.
      for (size_t i = 0; i < MAX_PRIORITIES; ++i) {
        TTask task = TasksHeads[i];
        while (task != NULL) {
          Log("Force Terminating task: %s\n", task->name);
          TTask next = task->next;
          ClearTask(task, i);
          task = next;
        }
      }
    } break;
  };
  for (size_t i = 0; i < MAX_PRIORITIES; ++i) {
    TasksHeads[i] = NULL;
    TasksTails[i] = NULL;
  }
  PriorityCounterAnchor = 0;
  CurrentTask = NULL;
  Log("Scheduler exited\n");
}

noreturn void ShutDownScheduler(void) {
  longjmp(Scheduler, SCHEDULER_EXIT);
}

void ActivateTask(TTask task) {
  Log("Activating task: %s\n", task->name);
  task->status = TASK_START;
  int priority = task->priority;
  Push(task, priority);
  // Когда задачу приняли, это поле начинает использоваться
  // как счётчик процессорного времени
  task->priority = PriorityCounterAnchor;
  Log("Task %s is activated\n", task->name);
}

noreturn void TerminateTask(void) {
  // Нельзя прямо здесь очищать стек задачи, потому что
  // мы пока именно в нём и находимся. Надо прыгать в планировщик.
  Log("Begin task %s termination\n", CurrentTask->name);
  longjmp(Scheduler, SCHEDULER_TERMINATE_CURRENT);
}
