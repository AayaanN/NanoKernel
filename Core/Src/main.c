///* USER CODE BEGIN Header */
///**
// ******************************************************************************
// * @file           : main.c
// * @brief          : Main program body
// ******************************************************************************
// * @attention
// *
// * Copyright (c) 2023 STMicroelectronics.
// * All rights reserved.
// *
// * This software is licensed under terms that can be found in the LICENSE file
// * in the root directory of this software component.
// * If no LICENSE file comes with this software, it is provided AS-IS.
// *
// ******************************************************************************
// */
///* USER CODE END Header */
///* Includes ------------------------------------------------------------------*/
//#include "main.h"
//#include <stdio.h> //You are permitted to use this library, but currently only printf is implemented. Anything else is up to you!
//#include <stdint.h>
//#include "common.h"
//#include "k_task.h"
//
//
//#define THREAD_SWITCH_SVC 0x1
//
//typedef unsigned int U32;
//typedef unsigned short U16;
//typedef char U8;
//typedef unsigned int task_t;
//
//#define EXPECTED_MAX_TASKS 16
//#define TEST_STACK_SIZE 0x400
//
//void f_test_task(void *arg) {
//   printf("PASS: kernel kept its own copy of TCB\r\n");
//   while (1); //does not yield
//}
//
//void f_print_fail(void *arg) {
//   printf("FAIL: first task was clobbered\r\n");
//   while (1);
//}
//
//typedef struct task_control_block
//{
//    void (*ptask)(void *args); // entry address
//    U32 stack_high;            // starting address of stack (high address)
//    task_t tid;                // task ID
//    U8 state;                  // task's state
//    U16 stack_size;            // stack size. Must be a multiple of 8
//    // your own fields at the end
//} TCB;
//
//// Global kernel-level data structures
//// e.g for task of ID 0, tcb_array[0] is the TCB for that task
//static TCB *tcb_array[MAX_TASKS];
//static task_t current_task;
//
//static U8 kernel_initialized = 0;
//static U8 kernel_running = 0;
//
//void osKernelInit(void)
//{
//    if (kernel_initialized)
//        return;
//
//    kernel_initialized = 1;
//
//    // Initialize current task to NULL
//    current_task = TID_NULL;
//
//    // Initialize all TCBs to NULL
//    for (int i = 1; i < MAX_TASKS; i++)
//    {
//        tcb_array[i] = NULL;
//    }
//}
//
//int osCreateTask(TCB *task)
//{
//
//    // this must be dynamically allocated
//    if (kernel_initialized == 0)
//        return RTX_ERROR;
//
//    // Task stack size must be multiple of 8
//    if (task->stack_size % 8 != 0)
//    {
//        return RTX_ERROR;
//    }
//
//    // Task stack size must be greater than STACK_SIZE
//    if (task->stack_size < STACK_SIZE)
//    {
//        return RTX_ERROR;
//    }
//
//    // TODO: Return RTX_ERROR if no memory is available?
//
//    for (int i = 1; i < MAX_TASKS; i++)
//    {
//        if (tcb_array[i] == NULL)
//        {
//        	printf("An empty task was found\r\n");
//            tcb_array[i] = task;
//            task->tid = i;
//            printf("this task is being assigned %d\n", i);
//            return RTX_OK;
//        }
//    }
//
//    return RTX_ERROR; // Max tasks reached
//}
//
//int osKernelStart(void)
//{
//    if (kernel_initialized == 0)
//        return RTX_ERROR;
//
//    if (kernel_running)
//        return RTX_ERROR;
//
//    kernel_running = 1;
//
//    // Task scheduler - find the first `READY` task and start it
//    for (int i = 0; i < MAX_TASKS; i++) {
//        if (tcb_array[i]->state == READY) {
//            current_task = i;
//            tcb_array[i]->state = RUNNING;
//
//            // Load and start task by switching using system call
//            __asm("SVC #1");
//
//            break;
//        }
//    }
//}
//
//void osYield(void) {
//    if (kernel_initialized == 0)
//        return;
//
//    if (kernel_running == 0)
//        return;
//
//    tcb_array[current_task]->state = READY;
//
//    // Save current task's context
//    // __asm(
//    //     "MRS R0, PSP\n"           // Get current PSP value
//    //     "STMDB R0!, {R4-R11}\n"   // Store R4-R11 on task's stack
//    //     "MSR PSP, R0\n"           // Update PSP with new stack top
//    //     "MOV %0, R0\n"           // Output this new stack
//    //     : "=r"(tcb_array[current_task]->stack_high) // Save this output into current task's TCB
//    //     : : "r0"
//    // );
//
//    __asm("SVC #2");
//
//    // Find the next ready task
//    for (int i = 0; i < MAX_TASKS; i++) {
//        U32 next = (current_task + 1 + i) % MAX_TASKS;
//        if (tcb_array[next] != NULL && tcb_array[next]->state == READY) {
//            current_task = next;
//            tcb_array[next]->state = RUNNING;
//            break;
//        }
//    }
//
//    // TODO: What if no ready tasks are found?
//
//    // Execute the next task
//    __asm("SVC #1");
//}
//
//int osTaskInfo(task_t TID, TCB* task_copy) {
//    if (kernel_initialized == 0)
//        return RTX_ERROR;
//
//    if (TID >= MAX_TASKS)
//        return RTX_ERROR;
//
//    if (tcb_array[TID] == NULL)
//        return RTX_ERROR;
//
//    // Copy fields of task `TID` to `task_copy`
//    task_copy->ptask = tcb_array[TID]->ptask;
//    task_copy->stack_high = tcb_array[TID]->stack_high;
//    printf("stack high is %d/n", tcb_array[TID]);
//    task_copy->tid = tcb_array[TID]->tid;
//    task_copy->state = tcb_array[TID]->state;
//    task_copy->stack_size = tcb_array[TID]->stack_size;
//
//    return RTX_OK;
//}
//
//task_t osGetTID(void) {
//    if (kernel_initialized == 0)
//        return TID_NULL;
//
//    if (kernel_running == 0)
//        return TID_NULL;
//
//    return current_task;
//}
//
//// NOTE: Currently, when a task exits, it is not removed from `tcb_array` - its state is set to `DORMANT`.
//// In the future, we should handle removing tasks from `tcb_array` when they exit (i.e memory allocation).
//int osTaskExit(void) {
//    if (kernel_initialized == 0)
//        return RTX_ERROR;
//
//    if (kernel_running == 0)
//        return RTX_ERROR;
//
//    if (current_task == TID_NULL)
//        return RTX_ERROR;
//
//    tcb_array[current_task]->state = DORMANT;
//
//    // Find next ready task
//    for (int i = 0; i < MAX_TASKS; i++) {
//        U32 next = (current_task + 1 + i) % MAX_TASKS;
//        if (tcb_array[next] != NULL && tcb_array[next]->state == READY) {
//            current_task = next;
//            tcb_array[next]->state = RUNNING;
//
//            // Switch to next task
//            __asm("SVC #1");
//            break;
//        }
//    }
//
//    return RTX_OK;
//}
//
///**
// * @brief  The application entry point.
// * @retval int
// */
//void SVC_Handler_Main(unsigned int *svc_args)
//{
//    // Get the SVC number from the instruction
//    unsigned int svc_number = ((char *)svc_args[6])[-2];
//
//    if (svc_number == THREAD_SWITCH_SVC)
//    {
//        // Load R4-R11 from thread stack
//        __asm(
//            "LDMIA %0!, {r4-r11}\n" // Load registers from stack
//            "MSR PSP, %0\n"         // Update PSP with new stack pointer
//            "MOV R0, #2\n"          // Set up return to use PSP
//            "MSR CONTROL, R0\n"     // Switch to unprivileged mode and use PSP
//            "ISB\n"                 // Instruction barrier
//            "MOV LR, #0xFFFFFFFD\n" // Set up LR for return to thread mode using PSP
//
//            "BX LR\n"
//            : : "r"(tcb_array[current_task]->stack_high) // Input is the current task's stack
//            : "r0"            // Clobber r0
//        );
//    }
//    // not a first thread switch
//    else if (svc_number == 2){
//        SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
//        __asm(
//        "MRS R0, PSP\n"           // Get current PSP value
//        "STMDB R0!, {R4-R11}\n"   // Store R4-R11 on task's stack
//        "MSR PSP, R0\n"           // Update PSP with new stack top
//        "MOV %0, R0\n"           // Output this new stack
//        : "=r"(tcb_array[current_task]->stack_high) // Save this output into current task's TCB
//        : : "r0"
//    );
//    }
//}
//// __asm("isb");
//void print_continuously(void)
//{
//    while (1)
//    {
//        printf("Thread!\r\n");
//    }
//}
//
//U32 *stackptr;
//const U32 THREAD_STACK_SIZE = 0x400; // 1024 bytes
//
//void init_thread_stack(void)
//{
//    // Calculate initial stack pointer for thread from MSP_INIT_VAL
//    U32 *MSP_INIT_VAL = *(U32 **)0x0;
//    stackptr = (U32 *)((U32)MSP_INIT_VAL - THREAD_STACK_SIZE);
//
//    // Set up thread context on stack
//    *(--stackptr) = 1 << 24;                 // xPSR (Thumb mode)
//    *(--stackptr) = (U32)print_continuously; // PC, entry point of thread
//    // *(--stackptr) = 0xFFFFFFF9; // LR, return to thread mode using MSP (in theory we don't need this since our thread runs infinitely)
//
//    // Push dummy values for other registers (R14(LR),R12,R3,R2,R1,R0)
//    for (int i = 0; i < 14; i++)
//    {
//        *(--stackptr) = 0xA;
//    }
//}
//
////int main(void)
////{
////
////    /* MCU Configuration: Don't change this or the whole chip won't work!*/
////
////    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
////    HAL_Init();
////    /* Configure the system clock */
////    SystemClock_Config();
////
////    /* Initialize all configured peripherals */
////    MX_GPIO_Init();
////    MX_USART2_UART_Init();
////    /* MCU Configuration is now complete. Start writing your code below this line */
////
////    /* Infinite loop */
////    /* USER CODE BEGIN WHILE */
////    // uint32_t* MSP_INIT_VAL = *(uint32_t**)0x0;
////
////    // printf("MSP Init is: %p\r\n", MSP_INIT_VAL);//note the %p to print a pointer. It will be in hex
////
////    // __asm("SVC #17");
////
////    while (1)
////    {
////        /* USER CODE END WHILE */
////
////        //	  printf("Hello, world!\r\n");
////        /* USER CODE BEGIN 3 */
////    }
////    /* USER CODE END 3 */
////}
//int main(void) {
//  /* MCU Configuration: Don't change this or the whole chip won't work!*/
//
//  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
//  HAL_Init();
//  /* Configure the system clock */
//  SystemClock_Config();
//
//  /* Initialize all configured peripherals */
//  MX_GPIO_Init();
//  MX_USART2_UART_Init();
//  /* MCU Configuration is now complete. Start writing your code below this line */
//
//  uint32_t MSP_INT = *(uint32_t**)(0x00);
//
//  printf("The init val is %0x \r \n", MSP_INT);
//  while(1) {}
//
//  osKernelInit();
//
//  TCB mytask = (TCB){
//    .ptask = &f_test_task, //expect this to go into kernel's copy
//    .stack_size = TEST_STACK_SIZE, //expect this to go into kernel's copy
//    .tid = 0xff, //kernel to assign a TID between 1-15
//    .stack_high = 0, //expect kernel's copy to get an allocated stack address
//  };
//
//  if (osCreateTask(&mytask) != RTX_OK) { //if successful this should have updated the user's copy with the TID assigned
//    printf("FAIL: osCreateTask failed\r\n");
//    return 0;
//  }
//
//  TCB task_readback = (TCB){  //create a fresh object to ensure osTaskInfo did something
//     .ptask = 0,
//     .stack_size = 0,
//     .tid = 0xff,
//     .stack_high = 0
//  };
//
//  if ((mytask.tid >= EXPECTED_MAX_TASKS) || (mytask.tid == 0 )) { //expect between 1-15
//    printf("FAIL: osCreateTask did not update the input TCB with a valid TID \r\n\r\n");
//    return 0;
//  } else {
//    printf("PASS: osCreateTask updated the input TCB with a valid TID %u\r\n\r\n", mytask.tid);
//  	if (osTaskInfo(mytask.tid, &task_readback) != RTX_OK) {
//            printf("FAIL: osTaskInfo failed\r\n");
//            return 0;
//	}
//  }
//
//  printf("Values retrieved by osTaskInfo:\r\n");
//  printf("ptask=%p\r\n", task_readback.ptask);
//  printf("stack_high=0x%x\r\n", task_readback.stack_high);
//  printf("tid=%u\r\n", task_readback.tid);
//  printf("state=0x%x\r\n", task_readback.state);
//  printf("stack_size=0x%x\r\n", task_readback.stack_size);
//
//  // check population of TCB
//  if (task_readback.tid != mytask.tid)
//    printf("FAIL: Task ID mismatch\r\n");
//  else if (task_readback.stack_high == 0)
//    printf("FAIL: Stack high value is zero\r\n");
//  else if (task_readback.ptask != &f_test_task)
//    printf("FAIL: Task function pointer mismatch\r\n");
//  else if (task_readback.stack_size != TEST_STACK_SIZE)
//    printf("FAIL: Stack size mismatch\r\n");
//  else
//    printf("PASS: TCB populated correctly\r\n");
//
//
//
//  //create another task, everything the same except task function.
//  //This function should never start if the first task is working (because it doesn't yield)
//  mytask.ptask = &f_print_fail;
//  if (osCreateTask(&mytask) != RTX_OK) {
//    printf("FAIL: osCreateTask failed\r\n");
//    return 0;
//  }
//
//  osKernelStart();
//
//  while (1);
//}
