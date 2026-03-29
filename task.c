/******************************************************************************/
/* task.c                                                                     */
/*                                                                            */
/* Этот файл содержит функции, реализующие подсистему управления задачами.    */
/*                                                                            */
/* Автор: Шаргин А. Ю.                                                        */
/*                                                                            */
/* Дата создания: 20 июля 2000 г.                                             */
/******************************************************************************/

#include  "rtos_api.h"
#include  "sys.h"

/******************************************************************************/
/* Внутренние функции ОСРВ.                                                   */
/******************************************************************************/

/* Планировщик */
void _Schedule(char task)
{
   char Cur, Old;
   char prior;

   Cur = gTaskQueueHead;
   Old = _NULL;
   prior = gTaskQueue[task].priority;

   /* Находим хвост подсписка задач с приоритетом prior. */
   while (Cur != _NULL && gTaskQueue[Cur].priority >= prior)
   {
      Old = Cur;
      Cur = gTaskQueue[Cur].link;
   }

   /* Вставляем задачу в список. */
   gTaskQueue[task].link = Cur;
   if (Old == _NULL)
   {
      /* Вставка в голову списка. */
      gTaskQueueHead = task;
   }
   else
   {
      /* Вставка в середину или в хвост списка. */
      gTaskQueue[Old].link = task;
   }
}

/* Диспетчер */
ISR(_Dispatch)
{
   if(gTaskQueueHead == _NULL)
   {
      /* Список задач пуст. Завершаем работу. */
      ShutdownOS();
   }

   if(gRunningTask != _NULL)
   {
      /* Текущая задача была прервана. Сохраняем контекст. */
      gTaskQueue[gRunningTask].context = GetSP();
   }

   /* Назначаем на выполнение задачу, стоящую в голове списка. */
   gRunningTask = gTaskQueueHead;

   WfSwitchTask (gRunningTask);

   /* Переключаем задачи. */
   if(gTaskQueue[gRunningTask].temp == _NULL)
   {
      /* Задача назначается на выполнение в первый раз. */
      /* Вызываем её как функцию. */
      gTaskQueue[gRunningTask].temp = gTaskQueue[gRunningTask].priority;
      SetSP(gReturnSP - gRunningTask*TASK_STACK);
      EI();
      ((_pfn)gTaskQueue[gRunningTask].context)();
   }
   else
   {
      /* Иначе просто подменяем контекст и выходим. */
      SetSP(gTaskQueue[gRunningTask].context);
   }

   return;
}

/******************************************************************************/
/* Сервисы ОС                                                                 */
/******************************************************************************/

void _Activate(_pfn task, char prior)
{
   /* Идентификатор задачи - её адрес в массиве. */
   char NewPlace;

   DI();

   /* Запоминаем позицию, в которую будет размещена новая задача. */
   NewPlace = gFreeTask;

   /* Ищем свободное место в массиве задач. */
   gFreeTask = gTaskQueue[gFreeTask].link;
   gTaskQueue[NewPlace].priority = prior;

   /* Добавляем новую задачу в список задач. */
   _Schedule(NewPlace);

   /* Запоминам точку входа новой задачи. */
   gTaskQueue[NewPlace].temp = _NULL;
   gTaskQueue[NewPlace].context = (unsigned)task;

   WfActivateTask (NewPlace, prior);

   /* Если нужно, вызываем диспетчер. */
   if(NewPlace == gTaskQueueHead)
   {
      _Dispatch();
   }

   EI();
}

void TerminateTask(void)
{
   char OurTask;

   DI();

   /* Удаляем задачу из списка задач. Помещаем структуру в список свободных. */
   OurTask = gTaskQueueHead;
   gTaskQueueHead = gTaskQueue[OurTask].link;
   gTaskQueue[OurTask].link = gFreeTask;
   gFreeTask = OurTask;

   /* Сбрасываем указатель на текущую задачу. */
   gRunningTask = _NULL;

   WfTerminateTask (OurTask);

   /* Вызываем диспетчер. */
   _Dispatch();
}