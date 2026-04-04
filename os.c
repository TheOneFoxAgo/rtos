#include <stdnoreturn.h>
#include "rtos_api.h"
#include "sys.h"

int StartOS(TTask task)
{
	Log("Start OS.\n");
	ActivateTask(task);
	StartScheduler();
	return 0;
}

noreturn void ShutdownOS()
{
	Log("Shutdown OS.\n");
	ShutDownScheduler();
}
