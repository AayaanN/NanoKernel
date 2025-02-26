  .syntax unified
  .cpu cortex-m4
  .fpu softvfp
  .thumb

.global SVC_Handler
.global PendSV_Handler
.global firstThread

.thumb_func
PendSV_Handler:
	MRS R0, PSP // Get the current PSP value
    STMDB R0!, {R4-R11} // Store R4-R11 on the stack
    MSR PSP, R0
    BL scheduler // Call the scheduler to switch threads
    MRS R0, PSP // Get the updated PSP value
    MOV LR, #0xFFFFFFFD // Set EXC_RETURN to return to Thread mode, using PSP
    LDMIA R0!, {R4-R11} // Load R4-R11 from the stack
    MSR PSP, R0 // Update PSP
    BX LR // Branch to the address in LR (returns to Thread mode)

.thumb_func

SVC_Handler:

	tst lr, #4

	ite eq

	mrseq r0, MSP

	mrsne r0, PSP
	B SVC_Handler_Main

.thumb_func

firstThread:
    BL scheduler
	MRS R0, PSP
    LDMIA R0!, {R4-R11} // Restore R4-R11 from the stack
    MSR PSP, R0 // Update PSP
	MOV LR, #0xFFFFFFFD
    BX LR // Branch to the address in LR (returns to Thread mode)



