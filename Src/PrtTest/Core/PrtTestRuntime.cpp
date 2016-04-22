#include <stdarg.h>
#include <stdbool.h>
#include <assert.h>
#include "Prt.h"
#include "PrtExecution.h"
#include "PrtUser.h"
#include "PrtTestRuntime.h"
#include <vector>
#include <random>

typedef enum Scheduler_Task_Kind 
{
    SCHEDULER_MK_MACHINE,
    SCHEDULER_SEND,
    SCHEDULER_MACHINE_QUEUE_EMPTY,
    THREAD_INIT_DONE
} Scheduler_Task_Kind;

static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t g_schedule = PTHREAD_COND_INITIALIZER;
static bool scheduler_should_run = PRT_FALSE;

typedef struct Scheduler_Machine_Info
{
    PRT_MACHINEINST_PRIV *machine;
    pthread_cond_t should_run_cv;
    PRT_BOOLEAN should_run;
} Scheduler_Machine_Info;

static std::vector<Scheduler_Machine_Info*> running_machines;
static Scheduler_Machine_Info* current_machine;

static Scheduler_Task_Kind scheduler_task_kind;
static va_list* scheduler_task_args;

static void block_to_scheduler(Scheduler_Task_Kind kind, ...);

typedef Scheduler_Machine_Info* (Machine_Scheduler_Fn)();

PRT_MACHINEINST *
PrtMkMachineTest(
_Inout_  PRT_PROCESS            *process,
_In_  PRT_UINT32                instanceOf,
_In_  PRT_VALUE                 *payload
)
{
    PRT_MACHINEINST *r; 
    block_to_scheduler (SCHEDULER_MK_MACHINE, process, instanceOf, payload, &r);
    return r;
}

void
PrtSendTest(
    _Inout_ PRT_MACHINEINST         *context,
    _In_ PRT_VALUE                  *event,
    _In_ PRT_VALUE                  *payload
)
{
    block_to_scheduler(SCHEDULER_SEND, context, event, payload);
}

static void block_to_scheduler(Scheduler_Task_Kind kind, ...)
{
    va_list task_args;
    va_start(task_args, kind);
    Scheduler_Machine_Info* self_info = current_machine;
    pthread_mutex_lock(&g_mutex);
    self_info->should_run = PRT_FALSE;
    scheduler_should_run = PRT_TRUE;
    scheduler_task_args = &task_args;
    scheduler_task_kind = kind;
    pthread_cond_signal(&g_schedule);
    while (!self_info->should_run) {
        pthread_cond_wait(&self_info->should_run_cv, &g_mutex);
    }
    va_end(task_args);
    pthread_mutex_unlock(&g_mutex);
}

typedef struct PrtRunStateMachineStartArgs
{
    PRT_MACHINEINST_PRIV    *context;
    PRT_BOOLEAN             doDequeue;
} PrtRunStateMachineStartArgs;

static void* _PrtRunStateMachineStart(void* _args);

/*  This should always be called by the scheduler 
    The scheduler should also hold g_mutex on calling this function. */
void PrtRunStateMachine(
_Inout_ PRT_MACHINEINST_PRIV    *context,
_In_ PRT_BOOLEAN                doDequeue
)
{
    /* Register machine onto scheduler queue */
    pthread_t new_thread;
    Scheduler_Machine_Info *new_machine_info = (Scheduler_Machine_Info*)PrtMalloc(sizeof(Scheduler_Machine_Info));
    new_machine_info->machine = context;
    pthread_cond_init(&new_machine_info->should_run_cv, NULL);
    running_machines.push_back(new_machine_info);
    /* Create new thread and run machine */
    PrtRunStateMachineStartArgs args;
    args.context = context;
    args.doDequeue = doDequeue;
    current_machine = new_machine_info;
    PrtUnlockMutex(context->stateMachineLock);
    pthread_create(&new_thread, NULL, _PrtRunStateMachineStart, &args);
    /* Wait for thread creation to finish, g_mutex should be owned by the scheduler now */
    scheduler_should_run = PRT_FALSE;
    while(!scheduler_should_run)
    {
        pthread_cond_wait(&g_schedule, &g_mutex);
    }
    assert(scheduler_task_kind == THREAD_INIT_DONE);
}

static void* _PrtRunStateMachineStart(void* _args)
{
    PrtRunStateMachineStartArgs* args = (PrtRunStateMachineStartArgs*)_args;
    PRT_MACHINEINST_PRIV *self = args->context;
    PRT_BOOLEAN doDequeue = args->doDequeue;
    self->isRunning = PRT_TRUE;
    block_to_scheduler(THREAD_INIT_DONE);
    PrtLockMutex(self->stateMachineLock);
    _PrtRunStateMachine(self, doDequeue);
    Scheduler_Machine_Info *self_info = current_machine;
    block_to_scheduler(SCHEDULER_MACHINE_QUEUE_EMPTY);
    PrtFree(self_info);
    return NULL;
}

void scheduler_run(Machine_Scheduler_Fn *choose_machine)
{
    pthread_mutex_lock(&g_mutex);
    while (true) {
        switch(scheduler_task_kind)
        {
            case SCHEDULER_MK_MACHINE: {
                PRT_PROCESS *process = va_arg(*scheduler_task_args, PRT_PROCESS*);
                PRT_UINT32 instanceOf = va_arg(*scheduler_task_args, PRT_UINT32);
                PRT_VALUE *payload = va_arg(*scheduler_task_args, PRT_VALUE*);
                PRT_MACHINEINST **new_machine = va_arg(*scheduler_task_args, PRT_MACHINEINST**);
                *new_machine = PrtMkMachine(process, instanceOf, payload);
                break;
            }
            case SCHEDULER_SEND: {
                PRT_MACHINEINST *context = va_arg(*scheduler_task_args, PRT_MACHINEINST*);
                PRT_VALUE *event = va_arg(*scheduler_task_args, PRT_VALUE*);
                PRT_VALUE *payload = va_arg(*scheduler_task_args, PRT_VALUE*);
                PrtSend(context, event, payload);
                break;
            }
            case SCHEDULER_MACHINE_QUEUE_EMPTY: {
                running_machines.erase(
                    std::remove(running_machines.begin(), 
                                running_machines.end(), 
                                current_machine), 
                    running_machines.end()
                );
                current_machine->should_run = PRT_TRUE;
                pthread_cond_signal(&current_machine->should_run_cv);
                break;
            }
            default: {
                break;
            }
        }
        // from all machines, choose next to run
        if (running_machines.size() == 0)
            break;
        current_machine = choose_machine();
        scheduler_should_run = PRT_FALSE;
        current_machine->should_run = PRT_TRUE;
        pthread_cond_signal(&current_machine->should_run_cv);
        while(!scheduler_should_run)
        {
            pthread_cond_wait(&g_schedule, &g_mutex);
        }
    }
    pthread_mutex_unlock(&g_mutex);
}

Scheduler_Machine_Info* LIFO_Scheduler_Fn()
{
    return running_machines.back();
}

Scheduler_Machine_Info* FIFO_Scheduler_Fn()
{
    return running_machines.front();
}

std::random_device rd;
std::mt19937 rng(rd());

Scheduler_Machine_Info* Random_Scheduler_Fn()
{
    std::uniform_int_distribution<int> uni(0, running_machines.size() - 1);
    return running_machines[uni(rng)];
}

void ErrorHandler(PRT_STATUS status, PRT_MACHINEINST *ptr) 
{
    if (status == PRT_STATUS_ASSERT)
    {
        fprintf_s(stdout, "exiting with PRT_STATUS_ASSERT (assertion failure)\n");
        exit(1);
    }
    else if (status == PRT_STATUS_EVENT_OVERFLOW)
    {
        fprintf_s(stdout, "exiting with PRT_STATUS_EVENT_OVERFLOW\n");
        exit(1);
    }
    else if (status == PRT_STATUS_EVENT_UNHANDLED)
    {
        fprintf_s(stdout, "exiting with PRT_STATUS_EVENT_UNHANDLED\n");
        exit(1);
    }
    else if (status == PRT_STATUS_QUEUE_OVERFLOW)
    {
        fprintf_s(stdout, "exiting with PRT_STATUS_QUEUE_OVERFLOW \n");
        exit(1);
    }
    else if (status == PRT_STATUS_ILLEGAL_SEND)
    {
        fprintf_s(stdout, "exiting with PRT_STATUS_ILLEGAL_SEND \n");
        exit(1);
    }
    else
    {
        fprintf_s(stdout, "unexpected PRT_STATUS in ErrorHandler: %d\n", status);
        exit(2);
    }
}

void Log(PRT_STEP step, PRT_MACHINEINST *context) { PrtPrintStep(step, context);  }


void _scheduler_run_entry(Scheduler_Task_Kind kind, ...)
{
    va_list task_args;
    va_start(task_args, kind);
    scheduler_task_args = &task_args;
    scheduler_task_kind = kind;
    scheduler_run(LIFO_Scheduler_Fn);
    va_end(task_args);
}

void TestMain(PRT_PROGRAMDECL *program, PRT_UINT32 main_machine)
{
    PRT_DBG_START_MEM_BALANCED_REGION
    {
        PRT_PROCESS *process;
        PRT_GUID processGuid;
        PRT_VALUE *payload;
        processGuid.data1 = 1;
        processGuid.data2 = 0;
        processGuid.data3 = 0;
        processGuid.data4 = 0;
        process = PrtStartProcess(processGuid, program, ErrorHandler, Log);
        payload = PrtMkNullValue();
        PRT_MACHINEINST *r; 
        _scheduler_run_entry(SCHEDULER_MK_MACHINE, process, main_machine, payload, &r);
        PrtFreeValue(payload);
        PrtStopProcess(process);
    }
    PRT_DBG_END_MEM_BALANCED_REGION
}