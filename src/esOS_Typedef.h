
#ifndef _OS_TYPEDEF_H_
#define _OS_TYPEDEF_H_


//=======================================================================================

typedef unsigned char BOOL;

typedef unsigned char OS_u8;
typedef unsigned short OS_u16;
typedef unsigned long OS_u32;
typedef unsigned long long OS_u64;

typedef signed char OS_s8;
typedef signed short OS_s16;
typedef signed long OS_s32;
typedef signed long long OS_s64;

typedef volatile unsigned char OS_vu8;
typedef volatile unsigned short OS_vu16;
typedef volatile unsigned long OS_vu32;
typedef volatile unsigned long long OS_vu64;

typedef volatile signed char OS_vs8;
typedef volatile signed short OS_vs16;
typedef volatile signed long OS_vs32;
typedef volatile signed long long OS_vs64;
//-----------------------------------------------------------------------------
#define EXT_C_    extern "C" { 
#define _EXT_C    }


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//=======================================================================================
#endif