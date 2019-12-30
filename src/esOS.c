
/* Includes -------------------------------------------------------------------------------------*/
#include "_Readme_first_.h"  // it's recommended read this header file first before using esOS;
#include "esOS.h"

/* Configs --------------------------------------------------------------------------------------*/

/* Defines --------------------------------------------------------------------------------------*/
/* Externs --------------------------------------------------------------------------------------*/

/* Functions ------------------------------------------------------------------------------------*/
void OS_StkOverflow();
void OS_SysCrashed();
void OS_HeartBeat();
void OS_InitTaskStack(OS_TaskTCB_t *ptNewTaskTCB, void (*pTaskFunc)());
OS_u32  OS_GetFastServeTask(OS_TaskTCB_t **pptTaskTCB);


/* Variables ------------------------------------------------------------------------------------*/
static OS_u16 OS_uhTotalTaskCnt;
static OS_u16 OS_uhBeatCounter;
static OS_u16 OS_uhIdleCounter;

static OS_FastServeTask_t OS_tFastServeTask;

OS_TaskTCB_t *OS_ptCurrTaskTCB;
OS_TaskTCB_t *OS_ptNextTaskTCB;


OS_TaskTCB_t tTaskTCB_HeartBeat;
OS_u32 uwStack_OS_HeartBeat[64];

/*************************************************************************************************
// only tasks created by OS_CreateTask() can get cpu control and run;
**************************************************************************************************/
void OS_CreateTask(OS_TaskTCB_t *ptNewTaskTCB, void (*pTaskFunc)(), OS_u32 *pwStkTop, OS_u32 wStkSize)
{
  static OS_TaskTCB_t* ptLastTaskTCB;
  
  OS_uhTotalTaskCnt++;
  
  // chain all tasks;
  if(OS_uhTotalTaskCnt <= 1)
  {
    ptNewTaskTCB->tTaskTCB_Next = ptNewTaskTCB;
    OS_ptCurrTaskTCB            = ptNewTaskTCB; // cpu starts execution from the first created task ;
  }
  else
  {
    ptNewTaskTCB ->tTaskTCB_Next = ptLastTaskTCB->tTaskTCB_Next;
    ptLastTaskTCB->tTaskTCB_Next = ptNewTaskTCB;
  }
  
  ptLastTaskTCB             = ptNewTaskTCB;
  
  ptNewTaskTCB->uwStkSize   = wStkSize;
  ptNewTaskTCB->puwStkTop   = pwStkTop;
  ptNewTaskTCB->puwStkBtm   = pwStkTop + wStkSize;
  ptNewTaskTCB->puwStkLoLmt = pwStkTop + wStkSize/8;
  
  ptNewTaskTCB->puwStkTop[0] = OS_STK_CHECK_VALUE;
  ptNewTaskTCB->puwStkTop[1] = OS_STK_CHECK_VALUE;
  
  ptNewTaskTCB->swStatus          = 0;
  ptNewTaskTCB->uhHeartBeat       = 0;
  
  ptNewTaskTCB->uwInterval        = 0;
  ptNewTaskTCB->udwTimestampMark  = OS_GetTimestamp();
  
#if (OS_TRACK_TASK_EXECUTION_TIME > 0)
  ptNewTaskTCB->udwTimestampStart = OS_GetTimestamp();
#endif
  
  OS_InitTaskStack(ptNewTaskTCB, pTaskFunc);
}

/*************************************************************************************************
**************************************************************************************************/
void OS_InitTaskStack(OS_TaskTCB_t *ptNewTaskTCB, void (*pTaskFunc)())
{
  // (1) - stack space for R4-R11, manually pushed;
  // (2) - stack space for R0-R3, R12, LR, PC, xPSR, automaticlly pushed by cpu;
  // (3) - stack space for S0-S15, FPSCR, automaticlly pushed by cpu;
  // (4) - one reserved word pushed/poped into stack if FPU used, automaticlly pushed by cpu;
  // (5) - stack space for S16-S31, manually pushed;
#if (OS_FPU_USED > 0)
  ptNewTaskTCB->puwStkPtr = ptNewTaskTCB->puwStkBtm - 8/*(1)*/-8/*(2)*/-17/*(3)*/-1/*(4)*/-16/*(5)*/;
  *(ptNewTaskTCB->puwStkBtm - 1 -1/*(4)*/) = __get_FPSCR();      // FPSCR
  *(ptNewTaskTCB->puwStkBtm - 18-1/*(4)*/) = 0x01000000;         // xPSR
  *(ptNewTaskTCB->puwStkBtm - 19-1/*(4)*/) = (OS_u32)pTaskFunc;  // PC
  *(ptNewTaskTCB->puwStkBtm - 20-1/*(4)*/) = (OS_u32)pTaskFunc;  // LR
#else
  ptNewTaskTCB->puwStkPtr = ptNewTaskTCB->puwStkBtm - 8/*(1)*/-8/*(2)*/;
  *(ptNewTaskTCB->puwStkBtm - 1) = 0x01000000;         // xPSR
  *(ptNewTaskTCB->puwStkBtm - 2) = (OS_u32)pTaskFunc;  // PC
  *(ptNewTaskTCB->puwStkBtm - 3) = (OS_u32)pTaskFunc;  // LR
#endif
  
#if (OS_TRACK_USED_STACK_SIZE > 0)
  ptNewTaskTCB->uwStkSizeUsed = 0;
#endif
  
  ptNewTaskTCB->pfnEntry      = pTaskFunc;
}
/*************************************************************************************************
Note : 
**************************************************************************************************/
void OS_CheckTaskStack(OS_u32 puwStkPtrNow)   // inline this function to increase the accuracy of stack consumption calculaton;
{
#if (OS_FPU_USED > 0)
  OS_u32 uwStkPtrLater = puwStkPtrNow -8*4/*(1)*/-8*4/*(2)*/-17*4/*(3)*/-1*4/*(4)*/-16*4/*(5)*/;
#else
  OS_u32 uwStkPtrLater = puwStkPtrNow -8*4/*(1)*/-8*4/*(2)*/;
#endif
  
  if(uwStkPtrLater < (OS_u32)OS_ptCurrTaskTCB->puwStkLoLmt)
  {
    OS_ptCurrTaskTCB->swStatus |= OS_ERR_STK_REACH_LMT;
  }
  
#if (OS_STK_OVERFLOW_CHECK > 0)
  if((puwStkPtrNow < (OS_u32)OS_ptCurrTaskTCB->puwStkTop)  // if current stack pointer has already excessed its stack top;
     || (OS_ptCurrTaskTCB->puwStkTop[0] != OS_STK_CHECK_VALUE)  // or the value at stack top is overwriten
     || (OS_ptCurrTaskTCB->puwStkTop[1] != OS_STK_CHECK_VALUE)) // 
  {
    OS_SysCrashed();  // system crashed, do some emergency actions in OS_SysCrashed();
  }
  else if(uwStkPtrLater < (OS_u32)OS_ptCurrTaskTCB->puwStkTop)  // if stack pointer will excess its stack top after PendSV isr triggerd;
  {
    OS_SelectMSP();  // select MSP as stack, in case that the following function pushes registers;
    OS_InitTaskStack(OS_ptCurrTaskTCB, OS_StkOverflow);
    OS_ptCurrTaskTCB->swStatus |= OS_ERR_STK_OV_FLOW;
    OS_SelectPSP();  // back to PSP;
    
    // as current task can cause stack overflow, OS_StkOverflow() takes its
    // place and get cpu control to avoid further damage;
    OS_StartNewTask();  // start execution from the newly created task;
  }
#endif
}

/*************************************************************************************************
**************************************************************************************************/
void OS_GetNextTask()
{
  if(OS_ptCurrTaskTCB->swStatus >= 0) // if task stack overflows, its heart beat counter will stop;
    OS_ptCurrTaskTCB->uhHeartBeat++;
  
  OS_TaskTCB_t *ptTaskTCB = OS_ptCurrTaskTCB->tTaskTCB_Next;
  
  // find a ready task;
  // if all tasks are sleeping, the loop will go on till one task is ready;
  while(1)
  {
    static OS_TaskTCB_t *ptSuspendedTaskTCB = 0;
    OS_TaskTCB_t *ptFastServeTaskTCB;
    
    if(OS_GetFastServeTask(&ptFastServeTaskTCB))
    { // there are some tasks are requiring fast-serve;
      // backup the task which is suspended;
      if(ptSuspendedTaskTCB == 0)
      {
        if(ptTaskTCB == ptFastServeTaskTCB)
          ptSuspendedTaskTCB = ptTaskTCB->tTaskTCB_Next;
        else
          ptSuspendedTaskTCB = ptTaskTCB;
      }
      else if(ptSuspendedTaskTCB == ptFastServeTaskTCB)
      {
        ptSuspendedTaskTCB = ptSuspendedTaskTCB->tTaskTCB_Next;
      }
      
      OS_ptNextTaskTCB = ptFastServeTaskTCB;
      
      break;
    }
    else
    {
      if(ptSuspendedTaskTCB)
      { // restore the task which is suspended;
        ptTaskTCB = ptSuspendedTaskTCB;
        ptSuspendedTaskTCB = 0;
      }
      
      // check whether the task is sleeping;
      if((ptTaskTCB->uwInterval == 0) || ((OS_GetTimestamp() - ptTaskTCB->udwTimestampMark) >= ptTaskTCB->uwInterval))
      {
        OS_ptNextTaskTCB = ptTaskTCB;
        break;
      }
      
      ptTaskTCB = ptTaskTCB->tTaskTCB_Next;
      
      // increase idle counter if a task is sleeping;
      OS_uhIdleCounter++;
    }
  }
  
#if (OS_TRACK_TASK_EXECUTION_TIME > 0)
  OS_u64 udwTimeStamp = OS_GetTimestamp();
  
  OS_u32 uwExecTime = udwTimeStamp - OS_ptCurrTaskTCB->udwTimestampStart;
  if(uwExecTime > OS_ptCurrTaskTCB->uwExecutionTimeMax)
    OS_ptCurrTaskTCB->uwExecutionTimeMax = uwExecTime;
  
  OS_ptCurrTaskTCB->uwExecutionTime   = uwExecTime;
  OS_ptNextTaskTCB->udwTimestampStart = udwTimeStamp;
#endif
}
/*************************************************************************************************
specify a task to run immeditely after next task switch;

Notes:
  1, global int disabled temporarily;
  2, DO NOT add fast-serve task too fast and/or too many, otherwise non-fast-serve task can lose CPU
     for a sufficient long time;
**************************************************************************************************/
void OS_AddFastServeTask(OS_TaskTCB_t *ptTaskTCB)
{
  OS_u32 uwRegTmp = OS_SaveCpuIRQ();  // global int disabled
  
  OS_tFastServeTask.ptTaskTCB[OS_tFastServeTask.uhIndexIn++] = ptTaskTCB;
  
  if(OS_tFastServeTask.uhIndexIn >= FAST_SERVE_TASK_MAX_CNT)
    OS_tFastServeTask.uhIndexIn = 0;
  
  if(OS_tFastServeTask.uhTaskCnt < FAST_SERVE_TASK_MAX_CNT)
    OS_tFastServeTask.uhTaskCnt++;
  
  if(OS_tFastServeTask.uhTaskCnt >= FAST_SERVE_TASK_MAX_CNT)
    OS_tFastServeTask.uhIndexOut = OS_tFastServeTask.uhIndexIn;
  
  OS_RestoreCpuIRQ(uwRegTmp);   // global int restored
}
/*************************************************************************************************

Notes:
  global int disabled temporarily;
**************************************************************************************************/
OS_u32 OS_GetFastServeTask(OS_TaskTCB_t **pptTaskTCB)
{
  if(OS_tFastServeTask.uhTaskCnt == 0)
    return 0;
  
  OS_u32 uwRegTmp = OS_SaveCpuIRQ();  // global int disabled
  
  *pptTaskTCB = OS_tFastServeTask.ptTaskTCB[OS_tFastServeTask.uhIndexOut++];
  
  if(OS_tFastServeTask.uhIndexOut >= FAST_SERVE_TASK_MAX_CNT)
    OS_tFastServeTask.uhIndexOut = 0;
  
  OS_tFastServeTask.uhTaskCnt--;
  
  OS_RestoreCpuIRQ(uwRegTmp);   // global int restored
  
  return 1;
}
/*************************************************************************************************
**************************************************************************************************/
void OS_WakeupTask(OS_TaskTCB_t *ptTaskTCB)
{
  ptTaskTCB->uwInterval = 0;
}
/*************************************************************************************************
**************************************************************************************************/
void OS_Init()
{
  OS_uhTotalTaskCnt = 0;
  OS_uhBeatCounter  = 0;
  OS_uhIdleCounter  = 0;
  
  OS_tFastServeTask.uhTaskCnt  = 0;
  OS_tFastServeTask.uhIndexIn  = 0;
  OS_tFastServeTask.uhIndexOut = 0;
}
/*************************************************************************************************
**************************************************************************************************/
//void OS_TaskSwitch()
//{
//  OS_CheckTaskStack(OS_GetCurrentPSP());
//  OS_GetNextTask();
//  (*(int*)0xE000ED04) = 0x10000000;
//}
/*************************************************************************************************
**************************************************************************************************/
#if (OS_TRACK_TASK_EXECUTION_TIME > 0)
OS_u64 OS_GetTaskExecutedTime()
{
  return OS_GetTimestamp() - OS_ptCurrTaskTCB->udwTimestampStart;
}
#endif
/*************************************************************************************************
**************************************************************************************************/
void OS_Sleep(OS_u32 uwIntv)
{
  OS_ptCurrTaskTCB->udwTimestampMark = OS_GetTimestamp();
  OS_ptCurrTaskTCB->uwInterval = uwIntv;
  
  OS_TaskSwitch();
  
  OS_ptCurrTaskTCB->uwInterval = 0;
}
/*************************************************************************************************
**************************************************************************************************/
void OS_HeartBeat()
{
  while(1)
  {
    OS_uhBeatCounter++;
    //OS_Sleep(1000000);
    OS_TaskSwitch();
  }
}
/*************************************************************************************************
**************************************************************************************************/
void OS_StkOverflow()
{
  // do some emergency actions here !!
  OS_OnStkOverflow();
  
  while(1)
  {
    OS_Sleep(1000 * 1000);
    //OS_TaskSwitch();
  }
}
/*************************************************************************************************
**************************************************************************************************/
void OS_SysCrashed()
{
  // do some emergency actions here !!
  OS_OnSysCrashed();
  
  while(1);
}
/*************************************************************************************************
**************************************************************************************************/
void Task1()
{
  static int i1 = 0;
  int k1 = 0;
  //int wData[80];
  i1++;
  while(1)
  {
    i1++;
    k1++;
    OS_TaskSwitch();
    i1++;
    k1++;
    OS_TaskSwitch();
    //wData[0]++;
  }
}
void Task2()
{
  static int i2 = 0;
  int k2 = 0;
  i2++;
  while(1)
  {
    i2++;
    k2++;
    OS_TaskSwitch();
    i2++;
    k2++;
  }
}
void Task3()
{
  static int i3 = 0;
  i3++;
  while(1)
  {
    i3++;
    OS_TaskSwitch();
  }
}

/*************************************************************************************************
**************************************************************************************************/
