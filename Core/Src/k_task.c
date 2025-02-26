#include "main.h"
#include <stdio.h> //You are permitted to use this library, but currently only printf is implemented. Anything else is up to you!
#include <stdint.h>
#include "common.h"

#define THREAD_SWITCH_SVC 0x1

typedef unsigned int U32;
typedef unsigned short U16;
typedef char U8;
typedef unsigned int task_t;

#define EXPECTED_MAX_TASKS 16
#define TEST_STACK_SIZE 0x400
#define MAIN_STACK_SIZE 0x4000


#define THREAD_SWITCH_SVC 0x1

typedef unsigned int U32;
typedef unsigned short U16;
typedef char U8;
typedef unsigned int task_t;

#define EXPECTED_MAX_TASKS 16
#define TEST_STACK_SIZE 0x400
#define MAIN_STACK_SIZE 0x4000

static U32 bottom = 0;
// Global kernel-level data structures
// e.g for task of ID 0, tcb_array[0] is the TCB for that task
TCB tcb_array[MAX_TASKS] = {NULL};
TCB* current_task;
int current_tid_index = 0;
U8 kernel_initialized = 0;
U8 kernel_running = 0;
int task_counter = 0;
U32 MSP_INIT_VAL;


void f_test_task(void *arg) {
   printf("PASS: kernel kept its own copy of TCB\r\n");
   while (1); //does not yield
}

void f_print_fail(void *arg) {
   printf("FAIL: first task was clobbered\r\n");
   while (1);
}

int init_thread_stack(TCB *task)
{
    for (int i = 1; i < MAX_TASKS; i++) {

        if (tcb_array[i].state == DORMANT && tcb_array[i].stack_size >= task->stack_size) {
            task->stack_size = tcb_array[i].stack_size;
            task->state = READY;

            //difference from making a new task
            task->stackptr = tcb_array[i].stackptr;
            task->stack_high = tcb_array[i].stackptr;

            *(--task->stack_high) = 1 << 24;
            *(--task->stack_high) = (U32) (task->ptask);

            for (int i = 0; i < 14; i++) {
                *(--task->stack_high) = 0xCAFE;
            }

            tcb_array[i] = *task;
            return RTX_OK;
        }

        // this means we are at the bottom of our current running stack
        else if (tcb_array[i].state == NULL && tcb_array[i].state == 0){
            if (task_counter == 0) {
                task->stackptr = (int) MSP_INIT_VAL - MAIN_STACK_SIZE;
            } else {
                task->stackptr = tcb_array[i-1].stackptr - task->stack_size;
            }
            task->stack_high = task->stackptr;
            task->state = READY;
            *(--task->stack_high) = 1 << 24;
            *(--task->stack_high) = (U32) (task->ptask);
            // not enough space left in the stack for our currnt task
            for (int i = 0; i < 14; i++) {
                *(--task->stack_high) = 0xCAFE;
            }
            tcb_array[i] = *task;
            return RTX_OK;
        }
    }

    return -1;
}



void osKernelInit(void)
{
    if (kernel_initialized)
        return;

    kernel_initialized = 1;

    MSP_INIT_VAL = *(U32 **)0x0;
    current_task = NULL;
    bottom = (U32)MSP_INIT_VAL - MAIN_STACK_SIZE;
    // stackptr = (U32 *)((U32)MSP_INIT_VAL - THREAD_STACK_SIZE);


    // Initialize current task to NULL
}

int osCreateTask(TCB *task)
{

    // this must be dynamically allocated
    if (kernel_initialized == 0)
        return RTX_ERROR;
    
     // Task stack size must be greater than STACK_SIZE
    if (task->stack_size < STACK_SIZE)
    {
        return RTX_ERROR;
    }

    if (task->ptask == NULL) {
        return RTX_ERROR;
    }

    if (task_counter >= MAX_TASKS) {
        return RTX_ERROR;
    }

    // Task stack size must be multiple of 8
    if (task->stack_size % 8 != 0)
    {
        // adjust task->stack_size
        task->stack_size += (8 - (task->stack_size % 8));
    }
    task->tid = 0;
    for (int i = 1; i < 16; i++) {
        if (tcb_array[i].state == DORMANT) {
            task->tid = i;
            break;
        }
    }

    int return_value = init_thread_stack(task);
    if (return_value == -1){
        return RTX_ERROR;
    }
    task_counter++;

    return RTX_OK; // Max tasks reached
}

int osKernelStart(void)
{
    if (kernel_initialized == 0)
        return RTX_ERROR;

    if (kernel_running)
        return RTX_ERROR;

    kernel_running = 1;

    __asm("SVC #1");

    return RTX_OK;
}

void osYield(void) {
    if (kernel_initialized == 0)
        return;

    if (kernel_running == 0)
        return;

    __asm("SVC #2");

}

int osTaskInfo(task_t TID, TCB* task_copy) {
    if (kernel_initialized == 0)
        return RTX_ERROR;

    if (TID >= MAX_TASKS || TID == 0)
        return RTX_ERROR;

    if (tcb_array[TID].state == UNINITIALIZED)
        return RTX_ERROR;

    // Copy fields of task `TID` to `task_copy`
    int correct_tid = 0;
    for(int i = 1; i < MAX_TASKS; i++) {
    	printf("%d, %d\n", tcb_array[i].tid, TID);
        if(tcb_array[i].tid == TID) {
            correct_tid = i;
        }
    }
    task_copy->ptask = tcb_array[correct_tid].ptask;
    task_copy->stack_high = tcb_array[correct_tid].stack_high;
    task_copy->tid = tcb_array[correct_tid].tid;
    task_copy->state = tcb_array[correct_tid].state;
    task_copy->stack_size = tcb_array[correct_tid].stack_size;

    return RTX_OK;
}

task_t osGetTID(void) {
    if (kernel_initialized == 0)
        return TID_NULL;

    if (kernel_running == 0)
        return TID_NULL;

    return current_task->tid;
}

int is_empty() {
    for(int i = 1; i < 16; i++){
        if (tcb_array[i].state != DORMANT){
            return 0;
        }
    }
    return 1;
}

void scheduler(){
    if(!is_empty()){
        if(current_task != NULL && tcb_array[current_task->tid].state != DORMANT){
            tcb_array[current_task->tid].state = READY;
            tcb_array[current_task->tid].stack_high = __get_PSP();
        }
        int start_index = current_tid_index;
        do {
            current_tid_index = current_tid_index == 0 ? 1 : (current_tid_index + 1) % MAX_TASKS;
            if (tcb_array[current_tid_index].state == READY){
                tcb_array[current_tid_index].state = RUNNING;
                current_task = &tcb_array[current_tid_index];
                __set_PSP((uint32_t)tcb_array[current_task->tid].stack_high);
                return;
            }
        } while (current_tid_index != start_index);
    }
}

// NOTE: Currently, when a task exits, it is not removed from `tcb_array` - its state is set to `DORMANT`.
// In the future, we should handle removing tasks from `tcb_array` when they exit (i.e memory allocation).
int osTaskExit(void) {
    if (kernel_initialized == 0)
        return RTX_ERROR;

    if (kernel_running == 0)
        return RTX_ERROR;

    if (current_task->tid == 0)
        return RTX_ERROR;

    tcb_array[current_task->tid].state = DORMANT;

    task_counter--;
    __asm("SVC #2");

    return RTX_OK;
}

/**
 * @brief  The application entry point.
 * @retval int
 */
void SVC_Handler_Main(unsigned int *svc_args)
{
    // Get the SVC number from the instruction
    unsigned int svc_number = ((char *)svc_args[6])[-2];

    if (svc_number == THREAD_SWITCH_SVC)
    {

        firstThread();
    }
    // not a first thread switch
    else if (svc_number == 2){

        SCB->ICSR |= 1 << 28;
        __asm("isb");

    }
}
