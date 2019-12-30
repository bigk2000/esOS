
#ifndef __ESOS_H__
#define __ESOS_H__

//#include "stm32f0xx.h"
//#include "stm32f10x.h"
//#include "stm32f30x.h"

#include "esOS_Typedef.h"
#include "esOS_UserConfig.h"

/*************************************************************************************************
**************************************************************************************************/
struct OS_TaskTCB_t
{
  OS_u32 *puwStkPtr;      // MUST be first member, DO NOT reorder its position!
  OS_u32 *puwStkBtm;      // MUST be second member, DO NOT reorder its position!
#if (OS_TRACK_USED_STACK_SIZE > 0)
  OS_u32 uwStkSizeUsed;   // MUST be third member, DO NOT reorder its position!
#endif
  
  OS_u32 uwStkSize;
  OS_u32 *puwStkTop;
  OS_u32 *puwStkLoLmt;
  void   (*pfnEntry)();
  
  OS_TaskTCB_t *tTaskTCB_Next;
  
  OS_u16 uhTaskID;
  OS_u16 uhHeartBeat;
  OS_s32 swStatus;
  
  OS_u32 uwInterval;
  OS_u64 udwTimestampMark;
  
#if (OS_TRACK_TASK_EXECUTION_TIME > 0)
  OS_u32 uwExecutionTime;
  OS_u32 uwExecutionTimeMax;
  OS_u64 udwTimestampStart;
#endif
};

struct OS_FastServeTask_t
{
  OS_u16         uhTaskCnt;
  OS_u16         uhIndexIn;
  OS_u16         uhIndexOut;
  OS_TaskTCB_t*  ptTaskTCB[FAST_SERVE_TASK_MAX_CNT];
};


/*************************************************************************************************
**************************************************************************************************/
#define OS_ERR_STK_REACH_LMT    (1<<0)
#define OS_ERR_STK_OV_FLOW      (1<<31)

#define OS_STK_CHECK_VALUE      (0x1234ABCD)

#define OS_TaskSwitch()         (OS_CheckTaskStack(OS_GetCurrentPSP()), OS_GetNextTask(), (*(int*)0xE000ED04) = 0x10000000)
#define TS                      OS_TaskSwitch
#define OS_Delay                OS_Sleep
#define OS_TASK(TaskName)       __stackless void TaskName(void)


/*************************************************************************************************
**************************************************************************************************/
void OS_Init();
void OS_CreateTask(OS_TaskTCB_t *ptNewTaskTCB, void (*pTaskFunc)(), OS_u32 *pwStkTop, OS_u32 wStkSize);

void OS_Sleep(OS_u32 uwIntv);

void OS_WakeupTask(OS_TaskTCB_t *ptTaskTCB);
void OS_AddFastServeTask(OS_TaskTCB_t *ptTaskTCB); // Notes: global int disabled temporarily;

#if (OS_TRACK_TASK_EXECUTION_TIME > 0)
OS_u64 OS_GetTaskExecutedTime();
#endif


// the following APIs are defined in assemble file;
// use extern "C" to disable function name mangling in C++,
// otherwise assemble file can't reference mangled name as the mangled name is unsightly and unknown;
#ifdef __cplusplus
 extern "C" {
#endif 

void   OS_Start();
void   OS_StartNewTask();
void   OS_SelectMSP();
void   OS_SelectPSP();
OS_u32 OS_SaveCpuIRQ();
void   OS_RestoreCpuIRQ(OS_u32 uwRegTmp);
OS_u32 OS_GetCurrentPSP();

#ifdef __cplusplus
 }
#endif

void OS_GetNextTask();
void OS_CheckTaskStack(OS_u32 puwStkPtrNow);


// user must define the following functions
extern void   OS_OnStkOverflow();
extern void   OS_OnSysCrashed();
extern OS_u64 OS_GetTimestamp();

extern OS_TaskTCB_t tTaskTCB_HeartBeat;
extern OS_u32 uwStack_OS_HeartBeat[64];

// tasks for test;
void Task1();
void Task2();
void Task3();
/*************************************************************************************************
**************************************************************************************************/




/*************************************************************************************************
**************************************************************************************************/







#endif