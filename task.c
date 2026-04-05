#include <setjmp.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include "data.h"
#include "defs.h"
#include "rtos_api.h"
#include "sys.h"
// Кишки планировщика задач. Тут треш и угар.

static TTask CurrentTask;                 // Исполняемая в данный момент задача
static size_t CurrentPriority;            // Приоритет исполняемой задачи
static jmp_buf Scheduler;                 // Контекст планировщика.
static TTask TasksHeads[MAX_PRIORITIES];  // Массив начал списков задач.
static TTask TasksTails[MAX_PRIORITIES];  // Массив концов списков.
// Волшебная функция. Скорее черная магия.
// 1 аргумент - указатель на новый стек.
// 2 аргумент - указатель на функцию, которую вызовем. С новым стеком.
extern noreturn void StackSwitch(void* stack, void (*function)(void));

enum SchedulerSignal {
  SCHEDULER_INIT = 0,
  SCHEDULER_PICK_NEXT,   // Выполнить следующую задачу
  SCHEDULER_TERMINATE_TASK,  // Завершить текущую задачу
  SCHEDULER_EXIT,            // Выйти из планировщика
};

void yield(void) {
  if (setjmp(CurrentTask->ctx) == 0) {
    longjmp(Scheduler, SCHEDULER_PICK_NEXT);
  }
}

void PushToEnd(TTask task, size_t priority) {
  TTask tail = TasksTails[priority];
  if (tail != NULL) {
    tail->next = task;
  } else {
    TasksHeads[priority] = task;
  }
  TasksTails[priority] = task;
}

void PickNext(void) {
  for (size_t i = 0; i < MAX_PRIORITIES; ++i) {
    TTask task = TasksHeads[i];
    if (CurrentTask == NULL) {
      CurrentTask = task;
      CurrentPriority = i;
    } else if (task != NULL && task->priority < CurrentTask->priority) {
      CurrentTask = task;
      CurrentPriority = i;
      break;
    }
  }
}

// Сбрасываем задачу в изначальное состояние и освобождаем ресурсы
void ResetTask(TTask task, size_t priority) {
  task->status = TASK_START;
  task->priority = priority;
  task->next = NULL;
  free(task->stack);
  task->stack = NULL;
}

// Увеличиваем счётчик времени
void IncreaseTimeCounter(TTask task, size_t priority) {
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
  } else {
    task->priority += 1 + priority;
  }
}

// Начать исполнение задач.
// Я готов лекцию на 2 часа выдать, как эта балембала работает.
void StartScheduler(void) {
  Log("Scheduler start\n");
  switch (setjmp(Scheduler)) {
    case SCHEDULER_TERMINATE_TASK: {
      ResetTask(CurrentTask, CurrentPriority);
      Log("%s is terminated\n", CurrentTask->name);
      CurrentTask = NULL;
    }  // Умышленно нету break
    case SCHEDULER_INIT:
    case SCHEDULER_PICK_NEXT: {
      if (CurrentTask != NULL) {
        PushToEnd(CurrentTask, CurrentPriority);
      }
      PickNext();
      if (CurrentTask != NULL) {
        IncreaseTimeCounter(CurrentTask, CurrentPriority);
        TasksHeads[CurrentPriority] = CurrentTask->next;
        if (CurrentTask->status == TASK_START) {
          Log("Starting task: %s\n", CurrentTask->name);
          CurrentTask->status = TASK_REENTER;
          CurrentTask->stack = malloc(STACK_SIZE);
          StackSwitch(CurrentTask->stack + STACK_SIZE - 1, CurrentTask->body);
        } else {
          Log("Reentering task: %s\n", CurrentTask->name);
          longjmp(CurrentTask->ctx, 1); // Число ни на что не влияет.
        }
      }
    } break;
    case SCHEDULER_EXIT: {
      Log("Scheduler cleanup\n");
      // Очищаем всё что осталось.
      for (size_t i = 0; i < MAX_PRIORITIES; ++i) {
        TTask task = TasksHeads[i];
        while (task != NULL) {
          TTask next = task->next;
          ResetTask(task, i);
          task = next;
        }
        TasksHeads[i] = NULL;
        TasksTails[i] = NULL;
      }
    } break;
  };
  Log("Scheduler exited\n");
}

noreturn void ShutDownScheduler(void) {
  longjmp(Scheduler, SCHEDULER_EXIT);
}

void ActivateTask(TTask task) {
  Log("Activating task: %s\n", task->name);
  task->status = TASK_START;
  int priority = task->priority;
  PushToEnd(task, priority);
  // Когда задачу приняли, это поле начинает использоваться
  // как счётчик процессорного времени
  task->priority = 0;
  Log("Task %s is activated\n", task->name);
}

noreturn void TerminateTask(void) {
  // Как ни странно, нельзя очищать стек функции прямо здесь,
  // потому что мы пока, мы в этом стеке и находимся.
  // Если очистим - моментальное уб. Поэтому прыгаем в планировщик.
  // Он находится в другом стеке, и поэтому может спокойно
  // очистить этот.
  Log("Begin %s termination\n", CurrentTask->name);
  longjmp(Scheduler, SCHEDULER_TERMINATE_TASK);
}
