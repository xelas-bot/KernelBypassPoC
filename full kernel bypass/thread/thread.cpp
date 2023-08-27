#include "../defs.h"
#include "thread.h"

bool driver::thread::unlink()
{
	// Up to the reader to determine how to do /
	// implement your own method
	MY_KTHREAD* t = (MY_KTHREAD *) KeGetCurrentThread();
	t->SystemThread = 0;
	t->Alertable = 0;
	t->ApcQueueable = 0;
	t->ApcInterruptRequest = 0;

	return true;
}

bool driver::thread::link()
{
	// Up to the reader to determine how to do /
	// implement your own method
	MY_KTHREAD* t = (MY_KTHREAD*) KeGetCurrentThread();
	t->SystemThread = 1;
	t->Alertable = 1;
	t->ApcQueueable = 1;
	t->ApcInterruptRequest = 1;
	return true;
}

