/******************************************************************************/
/* rtos_api.h                                                                 */
/*                                                                            */
/* Этот файл содержит объявления функций пользовательского API.               */
/* Его необходимо включать во все модули пользовательских приложений.         */
/*                                                                            */
/* Автор: Шаргин А. Ю.                                                        */
/*                                                                            */
/* Дата создания: 4 июля 2000 г.                                              */
/******************************************************************************/

#ifndef __RTOS_API_H
#define __RTOS_API_H

#include "hardware.h"
#include "log.h"

/******************************************************************************/
/* Макросы пользовательского API                                              */
/******************************************************************************/

#define DeclareTask(taskID)                    \
   extern const _TaskStruct taskID[1];         \
   WfDeclareName (taskID)

#define DeclareResource(resID, prior)          \
   WfDeclareName (resID);	               \
   enum { resID = prior };

#define ActivateTask(taskID)                   \
   WfAssignName (taskID);                      \
   _Activate(taskID->entry, taskID->priority)

#define StartOS(taskID)                        \
   WfAssignName (taskID);                      \
   _Start(taskID->entry, taskID->priority)

#define GetResource(resID)                     \
   WfAssignName (resID);                       \
   _Get (resID);

#define ReleaseResource(resID)                 \
   WfAssignName (resID);                       \
   _Release (resID);

#define TASK(taskID, priority)                 \
   void taskID##body(void);                    \
   const _TaskStruct taskID[1] =               \
      { {taskID##body, priority} };            \
   void taskID##body(void)

/******************************************************************************/
/* Типы данных                                                                */
/******************************************************************************/

/* Указатель на функцию типа 'void fn(void)' */
typedef void (*_pfn)(void);

/* Структура с информацией о задаче (точка входа и приоритет) */
typedef struct
{
   _pfn entry;
   char priority;
} _TaskStruct, *_PTaskStruct;

/* Тип задачи */
typedef _PTaskStruct TTask;

/* Тип ресурса */
typedef char TResource;

/******************************************************************************/
/* Декларации функций                                                         */
/******************************************************************************/

/* Функции ОС, непосредственно вызываемые макросами пользовательского API */
void _Start(_pfn task, char prior);
void _Activate(_pfn task, char prior);
void _Terminate(void);
void _Get(TResource resource);
void _Release(TResource resource);

/* Сервисы ОС */
void TerminateTask(void);
void ShutdownOS(void);

#endif
