

/*****************************************************************************************
      Introductions to esOS

  esOS = extremely simple OS
  esOS = efficiency & small OS
******************************************************************************************/
/*
// ---------------------------------------
Features of esOS:
  1, extremely small, a few of APIs, code size is smaller than 1K, nearly no ram needed by OS itself;
  2, high efficiency in context swich, switch from one task to another takes less than 2us;
  3, fast-serve mechnism to accelerate response of external event;
  4, coodinative scheduling or polling scheduling which is better in reliability and pridictability;
  5, temporary/local variables supported;
  6, runs on any ARMv7-M based device;
  7, programmer can specify different stack size for each task;
  8, implemention of protection on task stack overflow and run-time task statistics;

// ---------------------------------------
Disadvantages:
  1, advanced task management not supported, 
     including dynamic task creation/deletion, task priority, massage, mailbox, 
     semaphore, and so on....
  2, task returning is not supported, so every task must be an infinite loop;
  3, esOS does not handle arguments passed to/from task in its argument list;

// ---------------------------------------
Instruction to use:
  0, use "esOS_UserConfig.h" file to set your configs before using esOS;
  1, do not task switch in ISR;
  2, only tasks created by OS_CreateTask() can get cpu control and run;
  3, create all tasks before OS_start() invoked;
  4, task should be an infinite loop, return from a task is prohibited;
  5, task should be void type, and esOS does not handle arguments passed to/from task, 
     that is, every task must look like this: void Task(void);
  6, cpu start execution from the first created task;
  7, to minimize stack usage, pushing s16-s31 of FPU is not recommended(forget 
     about this if ram is enough); in return, finish the float-point calculation 
     before task switch;
  8, temporary/local variables supported;
  9, as esOS does not privide mutual exclusion semaphore feature, 
     it's programmer's responsibiliy to guarantee the integrity of shared resources/data;
     it's recommended finish shared resources/data accessing before task switch;
  10, as task stacks are rare resources, do not allocate a large mount of local 
     variables(eg. local array), otherwise task stack can overflow, and in consequence 
     that task will be terminated(replaced by OS_StkOverflow);
  11, OS implements a fast-serve mechnism that a task requiring fast-serve will get control
     of cpu immediately after next task switch; to used it ,invoke OS_AddFastServeTask() on
     an external event;
  12, do some emergency actions in OS_StkOverflow() and OS_SysCrashed();
  13, define OS_OnSysCrashed() if necessary;
  14, use 'OS_TASK(TaskName)' to define a task to minimize stack size;

Notes:
  the following configs are set after OS_Start() calling;
  1, PSP used, thread mode selected;
  2, global irq enabled to task switch(disable global irq will disable task switch);
  3, PendSV_Handler priority is set to lowest(DO NOT change its priority);

// ---------------------------------------

  
// ---------------------------------------
Task stack size calculation:
  use the equation: S = X + A.
     S = Stack size
     X = stack used by the deepest function invoked by its task, this value depends 
       on user applicaitons;
     A = (8+8) when FPU not used(OS_FPU_USED = 0);
       = (8+8+17+1+16) when FPU used;

// ---------------------------------------
APIs:
  void OS_Init();
  void OS_Start();
  void OS_TaskSwitch();
  void OS_CreateTask();
  void OS_Sleep();
  void OS_WakeupTask();
  void OS_AddFastServeTask();

// ---------------------------------------
Demo:
      void main()
      {
        // user initializing code 
        
        OS_Init();
        
        OS_CreateTask(Task1, wStkTask1, 128);
        OS_CreateTask(Task2, wStkTask2, 128);
        
        OS_Start();    // first task will run;
        
        while(1) {};  // cpu should not reach here;

      }

      void Task1()
      {
        OS_Sleep(1000);    // sleep 1ms
        while(1)
        {
          // user code
          OS_TaskSwitch();
          // user code
        }
      }
      
      void Task2()
      {
        while(1)
        {
          // user code
          OS_Sleep(1000);    // sleep 1ms
          // user code
          OS_TaskSwitch();
          // user code
        }
      }
  
// ---------------------------------------
Compiler:
  1, IAR Embeded Workbench IDE for ARM V7.60.2

// ---------------------------------------
Initial Release:
  2016/12/23;(Well, Merry Christmas to everyone, and to esOS...)

*/
