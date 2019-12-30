

#ifndef __OS_USER_CONFIG_H__
#define __OS_USER_CONFIG_H__


/*************************************************************************************************
      User config section
**************************************************************************************************/

/* ----------------------------------------------------------
max number of tasks that can require fast-serve;
when excesses this value, the earliest task requiring fast-serve is overwritten;
tasks requiring fast-serve can get control of cpu immediately after next task switch;
*/
#define FAST_SERVE_TASK_MAX_CNT        (8)

/* ----------------------------------------------------------
select processor the OS runs on:
  0 = cortex-m0, cortex-m0+
  1 = cortex-m1, cortex-ms1
  3 = cortex-m3
  4 = cortex-m4
  7 = cortex-m7

Theoretically speaking, this OS can run on any ARMv7-M based device;
*/
#define OS_CPU_CORTEX_M      (3)


/* ----------------------------------------------------------
select whether FPU used, 
programmer must match OS_FPU_USED value with project settings(if FPU enabled in 
project settings, define OS_FPU_USED to 1, and vice versa);

  nonzero value = FPU used, another 18 words will push into stack.
*/
#define OS_FPU_USED          (0)


/* ----------------------------------------------------------
select whether to check stack overflow;

  nonzero value = check, recommended setting;
*/
#define OS_STK_OVERFLOW_CHECK         (1)


/* ----------------------------------------------------------
select whether to track the max stack size used, useful for debug;
  nonzero value = track, size stored in wStkSizeUsed, side effect: task switches a little slightly slower;

if a sub-function of a task allocates a large amount of stack space, and the sub-function does not invoke
OS_TaskSwitch(), the stack comsumed by this sub-function are not included in the track size;
*/
#define OS_TRACK_USED_STACK_SIZE      (1)


/* ----------------------------------------------------------
select whether to count a task execution time;
  nonzero value = count,
*/
#define OS_TRACK_TASK_EXECUTION_TIME  (0)


/* ----------------------------------------------------------
select whether to mark the task stack top(assign 0xA5A51234 to there), useful for debug;
  nonzero value = mark, side effect: additional 4 bytes needed on stack;
*/
#define OS_MARK_STACK_TOP             (1)


/* ----------------------------------------------------------
*/
/*************************************************************************************************
      User config check
**************************************************************************************************/
#if (OS_CPU_CORTEX_M < 4)
  #if (OS_FPU_USED > 0)
    #error  " FPU not present on this processor ! "
  #endif
#endif


/*************************************************************************************************
**************************************************************************************************/


#endif