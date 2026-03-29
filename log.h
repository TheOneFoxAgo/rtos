/******************************************************************************/
/* log.h                                                                      */
/*                                                                            */
/* Этот файл содержит объявления функций, обеспечивающих запись в журнал      */
/* системных событий, а также функции, полезные при тестировании              */
/*                                                                            */
/* Автор: Кожевников К.А.                                                     */
/*                                                                            */
/* Дата создания: 29 сентября 2000 г.                                         */
/******************************************************************************/

#ifndef  __LOG_H
#define  __LOG_H

/******************************************************************************/
/* Вспомогательные функции                                                    */
/******************************************************************************/

/* Задержка на несколько тиков */
void  DELAY (int t);

/******************************************************************************/
/* Функции записи в фурнал соответствующих системных событий                  */
/******************************************************************************/

#ifdef __LOGGING__

   void  WfInitLog         (char* filename);

   #define  WfDeclareName(Name)                                                \
      char* Name##name = #Name

   void  WfAssignName      (char* name);
   void  WfActivateTask    (char TaskNo, char priority);
   void  WfTerminateTask   (char TaskNo);
   void  WfSwitchTask      (char TaskNo);
   void  WfGetResource     (char TaskNo, char priority, char ceiling);
   void  WfReleaseResource (char TaskNo, char priority);
   void  WfStartOS         (void);
   void  WfShutdownOS      (void);

#else

   #define  WfInitLog(filename)
   #define  WfDeclareName(name)
   #define  WfAssignName(name)
   #define  WfActivateTask(TaskNo, priority)
   #define  WfTerminateTask(TaskNo)
   #define  WfSwitchTask(TaskNo)
   #define  WfGetResource(TaskNo, priority, ceiling)
   #define  WfReleaseResource(TaskNo, priority)
   #define  WfStartOS()
   #define  WfShutdownOS()

#endif

#endif
