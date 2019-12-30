
#ifndef __SYS_TICK_USER_CONFIG_H__
#define __SYS_TICK_USER_CONFIG_H__




/*************************************************************************************************
  User defined parameters;
  define them before using sys-tick functions;
**************************************************************************************************/
#define SYS_TICK_COUNT_MODE      (0)           // 0 = down-count mode, other value = up-count mode
#define SYS_TICK_CLK_FREQ        (8)           // unit is MHz;
#define SYS_TICK_PERIOD_VAL      (0xffffff+1)  // this value must be far more than SYS_TICK_CLK_FREQ or can be divisible by SYS_TICK_CLK_FREQ for accuracy;






/*************************************************************************************************
**************************************************************************************************/

/*************************************************************************************************
**************************************************************************************************/

#endif