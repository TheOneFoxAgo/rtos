#include "sys.h"
#include "rtos_api.h"

#include <unistd.h> // Функция sleep. Чисто для примера.
#include <stdio.h>

DeclareTask(Sample);
DeclareTask(Counter);
DeclareTask(Killer);

DeclareEvent(ShuttingDown);

int main(void)
{
	StartOS(Sample);
	return 0;
}

TASK(Sample, 2) {
	ActivateTask(Counter);
	ActivateTask(Killer);
	TerminateTask();
}

TASK(Counter, 1) {
	int counter = 0;
	while (counter < 5) {
		sleep(1);
		printf("Counter: %d\n", counter);
		// Эта функция не входит в api, поэтому её в тестах упоминать не надо.
		// Я здесь просто показываю, как можно "ждать" с помощью yield.
		yield();
		counter += 1;
	}
	SetEvent(Killer, ShuttingDown);
	TerminateTask(); // Не забывайте в конце каждой таски.
}

TASK(Killer, 3) {
	WaitEvent(ShuttingDown);
	ShutdownOS(); // Ну это исключение. После Shutdown можно таску и не завершать.
}
