
    #include "esOS_UserConfig.h"

    NAME OS_CPU
    
    EXTERN OS_ptCurrTaskTCB
    EXTERN OS_ptNextTaskTCB
    EXTERN OS_GetNextTask
    
    PUBLIC PendSV_Handler
    PUBLIC OS_Start
    PUBLIC OS_StartNewTask
    //PUBLIC OS_TaskSwitch
    PUBLIC OS_GetCurrentPSP
    PUBLIC OS_SelectMSP
    PUBLIC OS_SelectPSP
    PUBLIC OS_SaveCpuIRQ
    PUBLIC OS_RestoreCpuIRQ
    
    
    SECTION .text:CODE:ROOT:REORDER(2)
    
    THUMB
    
/*************************************************************************************************

  !!!!! DO NOT USE the following instructions for cortex-m4 !!!!!

      LDR SP, [Rn],#imm
      LDR SP, [Rn,#imm]!
      LDR SP, [Rn,#imm]
      LDR SP, [Rn]
      LDR SP, [Rn,Rm]

**************************************************************************************************/
PendSV_Handler:
    // at this point, R0-R3, R12, LR, PC, xPSR have been pushed into stack automaticlly by cpu.
    // for core with FPU, S0-S15 and FPSCR in FPU have already been pushed into stack by cpu, too;
    
    ;NOP  // for cortex-m4, the FPU data can be corrupted if there are less than 3 instructions in ISR , :( ...
    ;NOP
    ;NOP
    ;NOP
    
    MRS    R0, PSP        
  IF (OS_CPU_CORTEX_M >= 3)
    STMDB  R0!, {R4-R11}   // save R4-R11 to task stack
  ELSE
    SUBS   R0, R0, #8*4
    STMIA  R0!, {R4-R7}    // save R4-R11 to task stack
    MOV    R4, R8          // Store the high registers of CPU;
    MOV    R5, R9
    MOV    R6, R10
    MOV    R7, R11
    STMIA  R0!, {R4-R7}
    SUBS   R0, R0, #8*4
  ENDIF
  
  IF (OS_FPU_USED > 0)
    VSTMDB  R0!, {S16-S31}        // remaining FPU registers are saved to task stack
  ENDIF
    
    LDR    R1, =OS_ptCurrTaskTCB  // get current task stack pointer
    LDR    R1, [R1]
    STR    R0, [R1]               // save current PSP to Task control block
    
//  esOS can not track the used stack space allocated in a sub-function which does not invoke OS_TaskSwitch(),
//  thus, the consumed stack space are not included in the track size;
  IF (OS_TRACK_USED_STACK_SIZE > 0)
    ADDS   R1, #4              // get stack bottom position
    LDR    R2, [R1]
    SUBS   R2, R2, R0
    LSRS   R2, R2, #2          // calculate current stack size, in word
    ADDS   R1, #4
    LDR    R3, [R1]            // get the saved max stack size
    CMP    R2, R3
    BLO    skip
    STR    R2, [R1]            // save the max one
  skip:
  ENDIF
  
  IF (OS_MARK_STACK_TOP > 0)
    LDR    R1, =0xAAAAAAAA     // mark the stack top
    SUBS   R0, #4
    STR    R1, [R0]
  ENDIF
    
    LDR    R0, =OS_ptNextTaskTCB    // get next task stack pointer
    LDR    R2, =OS_ptCurrTaskTCB    // ptCurrTaskTCB = *ptNextTaskTCB
    LDR    R0, [R0]
    STR    R0, [R2]
    LDR    R0, [R0]
    
  IF (OS_FPU_USED > 0)
    VLDMIA  R0!, {S16-S31}     // if remaining FPU registers were saved to stack, restore them
  ENDIF
  
  IF (OS_CPU_CORTEX_M >= 3)
    LDMIA  R0!, {R4-R11}
  ELSE
    ADDS   R0, R0, #8*2
    LDMIA  R0!, {R4-R7}
    MOV    R8, R4
    MOV    R9, R5
    MOV    R10, R6
    MOV    R11, R7
    SUBS   R0, R0, #8*4
    LDMIA  R0!, {R4-R7}
    ADDS   R0, R0, #8*2
  ENDIF
    
    MSR    PSP, R0                // restore task stack pointer to PSP
    
    DSB
    
    BX     LR


/*************************************************************************************************
**************************************************************************************************/
//OS_TaskSwitch:
//    PUSH  {R0-R1, LR}
//    BL    OS_GetNextTask      // get next task to run, DIY it if necessary;
//    LDR    R0, =0xE000ED04    // trigger PendSV isr to context switch;
//    LDR    R1, =0x10000000
//    STR    R1, [R0]
//    POP    {R0-R1, PC}

/*************************************************************************************************
**************************************************************************************************/
OS_SaveCpuIRQ:
    MRS     R0, PRIMASK
    CPSID   I
    BX      LR


OS_RestoreCpuIRQ:
    MSR     PRIMASK, R0
    BX      LR
/*************************************************************************************************
**************************************************************************************************/
OS_GetCurrentPSP:
    MRS   R0, PSP
    BX    LR
/*************************************************************************************************
**************************************************************************************************/
OS_SelectPSP:
    MRS    R0, CONTROL        // select PSP as stack
  IF (OS_CPU_CORTEX_M >= 3)
    ORR    R0, R0, #0x2
  ELSE
    LDR    R1, =0x2
    ORRS   R0, R1
  ENDIF
    MSR    CONTROL, R0
    BX     LR

/*************************************************************************************************
**************************************************************************************************/
OS_SelectMSP:
    MRS    R0, CONTROL        // select MSP as stack
  IF (OS_CPU_CORTEX_M >= 3)
    AND    R0, R0, #0xFFFFFFFD
  ELSE
    LDR    R1, =0xFFFFFFFD
    ANDS   R0, R1
  ENDIF
    MSR    CONTROL, R0
    BX     LR

/*************************************************************************************************
**************************************************************************************************/
OS_Start:
    CPSID  I                        // disable int to switch SP to PSP;
    
    LDR    R0, =0xE000ED08          // initialize msp to its bottom
    LDR    R0, [R0]
    LDR    R0, [R0]
    MSR    MSP, R0                  // save first task stack pointer to SP
    
    LDR    R0, =0xE000ED22          // lowest PendSV_Handler priority
    LDR    R1, =0xF0
    STRB   R1, [R0]
    
    LDR    R0, =OS_ptCurrTaskTCB    // get first task stack pointer
    LDR    R0, [R0]
    LDR    R0, [R0]
    MSR    PSP, R0                  // save first task stack pointer to SP

    MRS    R0, CONTROL              // select PSP as stack
  IF (OS_CPU_CORTEX_M >= 3)
    ORR    R0, R0, #0x2
  ELSE
    LDR    R1, =0x2
    ORRS   R0, R1
  ENDIF
    MSR    CONTROL, R0
    
  IF (OS_FPU_USED > 0)
    VPOP   {S16-S31}
  ENDIF
  
  IF (OS_CPU_CORTEX_M >= 3)
    POP    {R4-R11}
  ELSE
    POP    {R4-R7}
    POP    {R0-R3}
    MOV    R8, R0
    MOV    R9, R1
    MOV    R10, R2
    MOV    R11, R3
  ENDIF

    // PendSV_Handler isr return simulation procedure
  IF (OS_CPU_CORTEX_M >= 3)
    POP    {R0-R3, R12, LR}  // 
  ELSE
    POP    {R0-R3}
    POP    {R0-R1}
    MOV    LR, R1
  ENDIF
  
    POP    {R0-R1}       // pop PC and xPSR
  IF(OS_FPU_USED > 0)
    VPOP   {S0-S15}      // pop FPU registers
    POP    {R0}          // pop FPSCR
    POP    {R1}          // pop reserved word
    VMSR   FPSCR, R0     // restore FPSCR
  ENDIF
    
    LDR    R0, =0        // clear
    LDR    R1, =0
    
    CPSIE  I             // enable irq in processor level
    
    BX     LR            // Let's ROCK and ROLL !!
    
/*************************************************************************************************
**************************************************************************************************/
OS_StartNewTask:
    LDR    R0, =OS_ptCurrTaskTCB  // get first task stack pointer
    LDR    R0, [R0]
    LDR    R0, [R0]
    MSR    PSP, R0                // save first task stack pointer to SP
    
  IF (OS_FPU_USED > 0)
    VPOP  {S16-S31}
  ENDIF
  
  IF (OS_CPU_CORTEX_M >= 3)
    POP    {R4-R11}
  ELSE
    POP    {R4-R7}
    POP    {R0-R3}
    MOV    R8, R0
    MOV    R9, R1
    MOV    R10, R2
    MOV    R11, R3
  ENDIF

    // PendSV_Handler isr return simulation procedure
  IF (OS_CPU_CORTEX_M >= 3)
    POP    {R0-R3, R12, LR}  // 
  ELSE
    POP    {R0-R3}
    POP    {R0-R1}
    MOV    LR, R1
  ENDIF
    POP    {R0-R1}       // pop PC and xPSR
  IF(OS_FPU_USED > 0)
    VPOP   {S0-S15}      // pop FPU registers
    POP    {R0}          // pop FPSCR
    POP    {R1}          // pop reserved word
    VMSR   FPSCR, R0     // restore FPSCR
  ENDIF
    
    LDR    R0, =0        // clear
    LDR    R1, =0
    
    BX     LR            // Let's ROCK and ROLL !!
/*************************************************************************************************
**************************************************************************************************/

    END

