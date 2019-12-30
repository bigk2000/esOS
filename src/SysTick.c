
#include "SysTick.h"


// this object must be constructed before CTimer-objects are initialized by compiler,
// so we construct it manually at the early stage after MCU reset,
// otherwise the CTimer objects can have wrong initial state;
// and, we must define oSysTick as __no_init to prevent being constructed by compiler.
__no_init CSysTick oSysTick;


/*************************************************************************************************
**************************************************************************************************/
CSysTick::CSysTick()
{
  
}
/*************************************************************************************************
**************************************************************************************************/
void CSysTick::Constructor(vu32 *puwSysTickCounter)
{
  puwCounter         = puwSysTickCounter;
  udwOverflowCounter = 0;
}
/*************************************************************************************************
**************************************************************************************************/
CSysTick::~CSysTick()
{

}
/*************************************************************************************************
CAUTION: interval between T1 and T2 should be far less than period of system tick timer;
situation that interruption occurs between T1 and T2 should be considered;
**************************************************************************************************/
u64 CSysTick::GetTimestamp() const
{
  u32 uwCounter[2];
  u64 udwCounterTmp;
  
  do
  {
    uwCounter[0] = *puwCounter;        // T1
    udwCounterTmp = udwOverflowCounter;
    uwCounter[1] = *puwCounter;        // T2
  }
  #if (SYS_TICK_COUNT_MODE == 0)
  while(uwCounter[1] > uwCounter[0]);
  #else
  while(uwCounter[1] < uwCounter[0]);
  #endif
  
  #if (SYS_TICK_COUNT_MODE == 0)
  u64 udwTimestamp = udwCounterTmp * (SYS_TICK_PERIOD_VAL /SYS_TICK_CLK_FREQ) + (SYS_TICK_PERIOD_VAL - uwCounter[1]) /SYS_TICK_CLK_FREQ;
  #else
  u64 udwTimestamp = udwCounterTmp * (SYS_TICK_PERIOD_VAL /SYS_TICK_CLK_FREQ) + uwCounter[1] /SYS_TICK_CLK_FREQ;
  #endif

  return (udwTimestamp);
}
/*************************************************************************************************
**************************************************************************************************/
/*inline void CSysTick::OnOverflow()
{
  dwOverflowCounter++;
}*/
/*************************************************************************************************
**************************************************************************************************/
void CSysTick::Delay(u32 wUs) const
{
  u64 udwTimestamp;
  u64 udwTimestampMark = GetTimestamp();
  
  do
  {
    udwTimestamp = GetTimestamp();
  }
  while(udwTimestamp < udwTimestampMark + wUs);
}
/*************************************************************************************************
**************************************************************************************************/
void CSysTick::DelayMs(u16 n) const
{
  Delay(n *1000);
}
/*************************************************************************************************
**************************************************************************************************/
void CSysTick::Delay_ms(u16 n) const
{
  Delay(n *1000);
}
/*************************************************************************************************
CAUTION: the pause duration must be less than system tick timer period, and interruption should be
taken into the duration ;
**************************************************************************************************/
#pragma optimize = speed
void CSysTick::Pause(u8 ubUs) const
{
  u32 uwCounter[2];
  u32 uwInterval = 0;
  
  uwCounter[0] = *puwCounter;
  for(;;)
  {
    uwCounter[1] = *puwCounter;

    #if (SYS_TICK_COUNT_MODE == 0)
    if(uwCounter[0] >= uwCounter[1])
      uwInterval = uwCounter[0] - uwCounter[1];
    else //if(uwCounter[0] < uwCounter[1])
      uwInterval = uwCounter[0] + SYS_TICK_PERIOD_VAL - uwCounter[1];
    #else
    if(uwCounter[0] <= uwCounter[1])
      uwInterval = uwCounter[1] - uwCounter[0];
    else //if(uwCounter[0] > uwCounter[1])
      uwInterval = uwCounter[1] + SYS_TICK_PERIOD_VAL - uwCounter[0];
    #endif

    if(uwInterval > ubUs * SYS_TICK_CLK_FREQ)
      break;
  }
}
/*************************************************************************************************
**************************************************************************************************/
CTimer::CTimer(u32 uwInt)
{
  uwInterval = uwInt;
  udwTimestampMark = oSysTick.GetTimestamp();
}
/*************************************************************************************************
**************************************************************************************************/
CTimer::~CTimer()
{

}
/*************************************************************************************************
**************************************************************************************************/
void CTimer::SetInterval(u32 wInt)
{
  uwInterval = wInt;
}
/*************************************************************************************************
**************************************************************************************************/
void CTimer::SetIntervalMs(u16 hInt)
{
  SetInterval(hInt *1000);;
}
/*************************************************************************************************
**************************************************************************************************/
void CTimer::Restart()
{
  udwTimestampMark = oSysTick.GetTimestamp();
}
/*************************************************************************************************
**************************************************************************************************/
bool CTimer::IsExpired()
{
  u64 dwTimestampNew = oSysTick.GetTimestamp();
  
  if(dwTimestampNew >= udwTimestampMark + uwInterval)
  {
    udwTimestampMark = dwTimestampNew;
    return true;
  }
  
  return false;
}

/*************************************************************************************************
**************************************************************************************************/
CStageTimer::CStageTimer(u32 uwStageDuty, u32 uwStagePeriod)
{
  uwDuty = uwStageDuty;
  uwPeriod = uwStagePeriod;
  udwTimestampMark = oSysTick.GetTimestamp();
}
/*************************************************************************************************
**************************************************************************************************/
bool CStageTimer::GetStage()
{
  u64 udwTimestampNew = oSysTick.GetTimestamp();

  if(udwTimestampNew < udwTimestampMark + uwDuty)
    return false;
  else if(udwTimestampNew >= udwTimestampMark + uwPeriod)
  {
    udwTimestampMark = udwTimestampNew;
    return false;
  }
  else
    return true;
}
/*************************************************************************************************
**************************************************************************************************/
void CStageTimer::SetDuty(u32 uwStageDuty)
{
  uwDuty = uwStageDuty;
}
/*************************************************************************************************
**************************************************************************************************/
void CStageTimer::SetPeriod(u32 uwStagePeriod)
{
  uwPeriod = uwStagePeriod;
}
/*************************************************************************************************
**************************************************************************************************/


/*************************************************************************************************
**************************************************************************************************/


/*************************************************************************************************
**************************************************************************************************/


/*************************************************************************************************
**************************************************************************************************/


/*************************************************************************************************
**************************************************************************************************/


/*************************************************************************************************
**************************************************************************************************/


/*************************************************************************************************
**************************************************************************************************/


/*************************************************************************************************
**************************************************************************************************/


/*************************************************************************************************
**************************************************************************************************/





