#include <stdarg.h>
#include <stdio.h>
#include <stdnoreturn.h>
#include "rtos_api.h"
#include "sys.h"

int StartOS(TTask task) {
  Log("Start OS.\n");
  ActivateTask(task);
  StartScheduler();
  Log("OS Shutdown finished.\n");
  Log("End of session\n");
  return 0;
}

noreturn void ShutdownOS() {
  Log("Begin OS Shutdown.\n");
  ShutDownScheduler();
}

int Log(const char* format, ...) {
  int res = 0;
  if (EnableLogging) {
    va_list args;
    va_start(args, format);
    res = vfprintf(stderr, format, args);
    va_end(args);
  }
  return res;
}
