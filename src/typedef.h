
#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_
//=======================================================================================

//typedef unsigned char bool;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned long long u64;

typedef signed char s8;
typedef signed short s16;
typedef signed long s32;
typedef signed long long s64;

typedef volatile unsigned char vu8;
typedef volatile unsigned short vu16;
typedef volatile unsigned long vu32;
typedef volatile unsigned long long vu64;

typedef volatile signed char vs8;
typedef volatile signed short vs16;
typedef volatile signed long vs32;
typedef volatile signed long long vs64;

typedef  void (*pFn)();

//-----------------------------------------------------------------------------
typedef union
{
  u16 hByte;
  u8  bByte[2];
  struct
  {
    u8 bByteL;
    u8 bByteH;
  };
} hWord_t;

//-----------------------------------------------------------------------------
typedef union
{
  u32 wByte;
  u16 hByte[2];
  u8  bByte[4];
  struct
  {
    u16 hByteL;
    u16 hByteH;
  };
  struct
  {
    u8 bByteLL;
    u8 bByteLH;
    u8 bByteHL;
    u8 bByteHH;
  };
} Word_t;
//-----------------------------------------------------------------------------
typedef union
{
  u64 dwByte;
  u32 wByte[2];
  u16 hByte[4];
  u8  bByte[8];
  struct
  {
    u32 wByteL;
    u32 wByteH;
  };
  struct
  {
    u16 hByteLL;
    u16 hByteLH;
    u16 hByteHL;
    u16 hByteHH;
  };
} dWord_t;
//-----------------------------------------------------------------------------
typedef struct
{
  unsigned char Bits;
  struct
  {
    unsigned char Bit0:1;
    unsigned char Bit1:1;
    unsigned char Bit2:1;
    unsigned char Bit3:1;
    unsigned char Bit4:1;
    unsigned char Bit5:1;
    unsigned char Bit6:1;
    unsigned char Bit7:1;
  };
}Bit8_t;
//-----------------------------------------------------------------------------
typedef union
{
  unsigned short Bits;
  struct
  {
    unsigned short Bit0:1;
    unsigned short Bit1:1;
    unsigned short Bit2:1;
    unsigned short Bit3:1;
    unsigned short Bit4:1;
    unsigned short Bit5:1;
    unsigned short Bit6:1;
    unsigned short Bit7:1;
    unsigned short Bit8:1;
    unsigned short Bit9:1;
    unsigned short Bit10:1;
    unsigned short Bit11:1;
    unsigned short Bit12:1;
    unsigned short Bit13:1;
    unsigned short Bit14:1;
    unsigned short Bit15:1;
  };
}Bit16_t;
//-----------------------------------------------------------------------------
typedef union
{
  unsigned int Bits;
  struct
  {
    unsigned int Bit0:1;
    unsigned int Bit1:1;
    unsigned int Bit2:1;
    unsigned int Bit3:1;
    unsigned int Bit4:1;
    unsigned int Bit5:1;
    unsigned int Bit6:1;
    unsigned int Bit7:1;
    unsigned int Bit8:1;
    unsigned int Bit9:1;
    unsigned int Bit10:1;
    unsigned int Bit11:1;
    unsigned int Bit12:1;
    unsigned int Bit13:1;
    unsigned int Bit14:1;
    unsigned int Bit15:1;
    unsigned int Bit16:1;
    unsigned int Bit17:1;
    unsigned int Bit18:1;
    unsigned int Bit19:1;
    unsigned int Bit20:1;
    unsigned int Bit21:1;
    unsigned int Bit22:1;
    unsigned int Bit23:1;
    unsigned int Bit24:1;
    unsigned int Bit25:1;
    unsigned int Bit26:1;
    unsigned int Bit27:1;
    unsigned int Bit28:1;
    unsigned int Bit29:1;
    unsigned int Bit30:1;
    unsigned int Bit31:1;
  };
}Bit32_t;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//=======================================================================================
#endif