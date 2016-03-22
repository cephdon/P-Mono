#include <stdarg.h>
#include <stdbool.h>
#include "PrtExecution.h"

typedef enum Scheduler_Task_Kind 
{
	SCHEDULER_MK_MACHINE,
	SCHEDULER_SEND,
	SCHEDULER_MACHINE_DONE
} Scheduler_Task_Kind;

static Scheduler_Task_Kind scheduler_task_kind;
static va_list* scheduler_task_args;

static void block_to_scheduler(Scheduler_Task_Kind kind, ...);
static void enqueue_machine(PRT_MACHINEINST* new_machine);

PRT_MACHINEINST_PRIV *
PrtMkMachineTest(
_Inout_  PRT_PROCESS_PRIV		*process,
_In_  PRT_UINT32				instanceOf,
_In_  PRT_VALUE					*payload
)
{
	PRT_MACHINEINST_PRIV *r;	
	block_to_scheduler (SCHEDULER_MK_MACHINE, process, instanceOf, payload, &r);
	return r;
}

void
PrtSend(
	_Inout_ PRT_MACHINEINST			*context,
	_In_ PRT_VALUE					*event,
	_In_ PRT_VALUE					*payload
)
{
	if (context->isModel)
	{
		context->process->program->modelImpls[context->instanceOf].sendFun(context, event, payload);
		return;
	}
	block_to_scheduler(SCHEDULER_SEND, (PRT_MACHINEINST_PRIV *)context, event, payload);
}

static void block_to_scheduler(Scheduler_Task_Kind kind, ...)
{
	va_list task_args;
    va_start(task_args, kind);
	// pthread_mutex_lock(&g_mutex);
	// self->should_run = false;
	scheduler_task_args = &task_args;
	scheduler_task_kind = kind;
	// pthread_cond_signal(&g_schedule);
	// while (!self->should_run) {
		// pthread_cond_wait(&self->cv, &g_mutex);
	// }
	va_end(*scheduler_task_args);
	// pthread_mutex_unlock(&g_mutex);
}

static void enqueue_machine(PRT_MACHINEINST* new_machine)
{
	
}

void scheduler_run(void)
{
	// pthread_mutex_lock(&g_mutex);
	while (true) {
		switch(scheduler_task_kind)
		{
			case SCHEDULER_MK_MACHINE: {
				PRT_PROCESS_PRIV *process = va_arg(*scheduler_task_args, PRT_PROCESS_PRIV*);
				PRT_UINT32 instanceOf = va_arg(*scheduler_task_args, PRT_UINT32);
				PRT_VALUE *payload = va_arg(*scheduler_task_args, PRT_VALUE*);
				PRT_MACHINEINST_PRIV **new_machine = va_arg(*scheduler_task_args, PRT_MACHINEINST_PRIV*);
				*new_machine = PrtMkMachinePrivate(process, instanceOf, payload);
				enqueue_machine(new_machine);
				break;
			}
			case SCHEDULER_SEND: {
				PRT_MACHINEINST *context = va_arg(*scheduler_task_args, PRT_MACHINEINST*);
				PRT_VALUE *event = va_arg(*scheduler_task_args, PRT_VALUE*);
				PRT_VALUE *payload = va_arg(*scheduler_task_args, PRT_VALUE*);
				PrtSendPrivate(context, event, payload);
				break;
			}
			case SCHEDULER_MACHINE_DONE: {
				break;
			}
		}
		// from all machines, choose next to run
		// thread->should_run = true;
		// pthread_cond_signal(&thread->cv);
		// pthread_cond_wait(&g_schedule, &g_mutex);
	}
	// pthread_mutex_unlock(&g_mutex);
}





