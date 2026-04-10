#include <stdarg.h>
#include "logging.h"
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

void ShutdownOS() {
  Log("Begin OS Shutdown.\n");
  ShutDownScheduler();
}
