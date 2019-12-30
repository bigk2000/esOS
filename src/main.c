
#include "stm32f10x.h"
#include "esOS.h"
#include "SysTick.h"


/*

	Step-1: select device in IAR project settings;
	Step-2:	include corresponding device header file in esOS.h;
	Step-3:	set configs in "esOS_UserConfig.h";
	Step-4: compile project and run;

*/


OS_u32 wStkTask1[64];
OS_u32 wStkTask2[64];
OS_u32 wStkTask3[64];

OS_TaskTCB_t tTaskTCB_Task1;
OS_TaskTCB_t tTaskTCB_Task2;
OS_TaskTCB_t tTaskTCB_Task3;

extern CSysTick oSysTick;

extern OS_u32 uwStack_OS_HeartBeat[64];
extern OS_TaskTCB_t tTaskTCB_HeartBeat;

void HardwareInit();
void OS_HeartBeat();

extern "C" void SysTick_Handler();

void main()
{
  HardwareInit();
	
	OS_Init();
	
	OS_CreateTask(&tTaskTCB_Task1, Task1, wStkTask1, sizeof(wStkTask1)/4);
	OS_CreateTask(&tTaskTCB_Task2, Task2, wStkTask2, sizeof(wStkTask2)/4);
	OS_CreateTask(&tTaskTCB_Task3, Task3, wStkTask3, sizeof(wStkTask3)/4);
	
	OS_CreateTask(&tTaskTCB_HeartBeat, OS_HeartBeat, uwStack_OS_HeartBeat, sizeof(uwStack_OS_HeartBeat)/4);
	
	OS_Start();		// first task will run;
	
	while(1);	    // cpu should not reach here;
}

void HardwareInit()
{
  oSysTick.Constructor((vu32*)&(SysTick->VAL));
  
  // ---------------------------------------------------
  FLASH->ACR =  
               (0<<5)    // PRFTBS:  Prefetch buffer status
              |(1<<4)    // PRFTBE:  Prefetch buffer enable bit; 0 = Dis, 1 = En;
              |(0<<3)    // HLFCYA:  Flash half cycle access enable bit; 0 = Dis, 1 = En;
              |(2<<0);   // LATENCY: Flash latency; 0 = If SYSCLK¡Ê(0,24], 1 = (24,48], 2 = (48,72];
  
  // ---------------------------------------------------
  RCC->CR =  (0<<25)      // PLLRDY: PLL clock ready flag
            |(0<<24)      // PLLON:  PLL enable bit.
            |(0<<19)      // CSSON:  Clock(HSE) security system enable bit, 0 = off, 1 = on;
            |(0<<18)      // HSEBYP: 0 = external oscillator not bypassed, 1 = bypassed;
            |(0<<17)      // HSERDY: External high-speed clock ready flag
            |(0<<16)      // HSEON:  HSE clock enable bit.
            |(0x10<<3)    // HSITRIM[4:0]: Internal high-speed clock trimming, bits[4:0]
            |(1<<1)       // HSIRDY: Internal high-speed clock ready flag
            |(1<<0);      // HSION:  0 = HSI off, 1 = on.
  
  RCC->CFGR =  (0<<24)      // MCO:         MCO output config;0 = disable, 4 = SYSCLK, 5 = HSI, 6 = HSE, 7 = PLL;
              |(0<<22)      // USBPRE:      USB prescaler; 0 = PLL clock/1.5; 1 = PLL clock;
              |(0xe<<18)    // PLLMUL:      PLL multiplication factor
              |(0<<17)      // PLLXTPRE:    HSE divider for PLL entry; 0 = HSE clock not divided; 1 = HSE clock/2;
              |(0<<16)      // PLLSRC:      PLL source; 0 = HSI/2; 1 = HSE;
              |(2<<14)      // ADCPRE[1:0]: ADC prescaler
              |(0<<11)      // PPRE2[2:0]:  APB2 prescaler
              |(4<<8)       // PPRE1[2:0]:  APB1 prescaler
              |(0<<4)       // HPRE[3:0]:   AHB prescaler
              |(0<<2)       // SWS[1:0]:    System clock switch status,
              |(0<<0);      // SW[1:0]:     System clock selection; 0 = HSI; 1 = HSE; 2 = PLL output;
  
  // PLL as system clock;
  RCC->CR |= (1<<24);
  while((RCC->CR & (1<<25)) == 0);
  RCC->CFGR |= 2;
  
  
  // ---------------------------------------------------
  SysTick->LOAD = 0xffffff; // 
  SysTick->VAL  = 0;        // Clear counter value and COUNTFLAG;
  //SysTick->CALIB = ;      // Read-only
  
  SysTick->CTRL =  (0<<2)   // CLKSOURCE: Clock source, 0 = AHB/8, 1 = AHB;
                  |(1<<1)   // TICKINT:    0 = int disabled, 1 = enabled;
                  |(1<<0);  // ENABLE:     0 = Stop, 1 = Start;
}

void SysTick_Handler()
{
  oSysTick.OnOverflow();
}

OS_u64 OS_GetTimestamp()
{
  return oSysTick.GetTimestamp();
}

void OS_OnStkOverflow()
{
  
}

void OS_OnSysCrashed()
{
  
}