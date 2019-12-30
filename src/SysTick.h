
#ifndef __SYS_TICK_H__
#define __SYS_TICK_H__

#include "typedef.h"
#include "SysTick_UserConfig.h"

/*                      !!!!!!!!!!!!!!!!!!

1, do not disable tick timer irq and glabol irq before call any sys-tick function;
2, max timestamp: 584,621 years on the following conditions: counter clock frequence is 8MHz, period/reloaded value is 0xffffff;
3, set user defined parameters first;
4, max resolution is 1us;

*/

/*************************************************************************************************
pwSysTickCounter: pointer of system tick timer counter;

**************************************************************************************************/
class CSysTick
{
public:
  void Constructor(vu32 *puwSysTickCounter);
  u64  GetTimestamp() const;   // return timestamp in 1us-per-digit;
  void Pause(u8 n) const;      // delay n us
  void Delay(u32 n) const;     // delay n us
  void DelayMs(u16 n) const;   // delay n ms
  void Delay_ms(u16 n) const;  // delay n ms
  void OnOverflow() {udwOverflowCounter++;}; // call this function on tick timer overflows;
  
  CSysTick();
  //virtual ~CSysTick();  // no virtual attribute because CSysTick object is defined as __no_init, 
  ~CSysTick();            // otherwise, the virtual function pointer can have an illegal value;

protected:
  
  vu32 *puwCounter;
  vu64 udwOverflowCounter;
  
private:

};

/*************************************************************************************************
min interval is 1us;
**************************************************************************************************/
class CTimer
{
public:
  void Restart();
  bool IsExpired();
  void SetInterval(u32 uwInt);  // set N us as interval;
  void SetIntervalMs(u16 uhInt);  // set N ms as interval;
  
  CTimer(u32 wInt = 1000/* 1us per digit */);
  virtual ~CTimer();
  
protected:
  
  u32 uwInterval;
  u64 udwTimestampMark;

private:

};

/*************************************************************************************************
**************************************************************************************************/
class CStageTimer
{
public:
  bool GetStage();  // return 0 if < duty, return 1 if >duty and <period;
  void SetDuty(u32 uwStageDuty);
  void SetPeriod(u32 uwStagePeriod);
  
  CStageTimer(u32 uwStageDuty, u32 uwStagePeriod);
  virtual ~CStageTimer() {};
  
protected:

  u32 uwDuty;
  u32 uwPeriod;
  u64 udwTimestampMark;
  
private:
  
};

/*************************************************************************************************
**************************************************************************************************/


/*************************************************************************************************
**************************************************************************************************/


/*************************************************************************************************
**************************************************************************************************/




#endif




