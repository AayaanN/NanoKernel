/*
 * k_task.h
 *
 *  Created on: Jan 5, 2024
 *      Author: nexususer
 *
 *      NOTE: any C functions you write must go into a corresponding c file that you create in the Core->Src folder
 */

#include "main.h"
#include <stdio.h> //You are permitted to use this library, but currently only printf is implemented. Anything else is up to you!
#include <stdint.h>
#include "common.h"

#ifndef INC_K_TASK_H_
#define INC_K_TASK_H_

int init_thread_stack(TCB *task);

void osKernelInit(void);

int osCreateTask(TCB *task);

int osKernelStart(void);

void osYield(void);

int osTaskInfo(task_t TID, TCB* task_copy);

task_t osGetTID(void);

int is_empty();

void scheduler();

int osTaskExit(void);

void SVC_Handler_Main(unsigned int *svc_args);




#endif /* INC_K_TASK_H_ */
