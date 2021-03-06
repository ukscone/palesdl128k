/*** Z80Em: Portable Z80 emulator *******************************************/
/***                                                                      ***/
/***                                 Z80.h                                ***/
/***                                                                      ***/
/*** This file contains the function prototypes and variable declarations ***/
/***                                                                      ***/
/*** Copyright (C) Marcel de Kogel 1996,1997                              ***/
/***     You are not allowed to distribute this software commercially     ***/
/***     Please, notify me, if you make any changes to this file          ***/
/****************************************************************************/

#ifndef _Z80_H
#define _Z80_H

/****************************************************************************/
/*** Machine dependent definitions                                        ***/
/****************************************************************************/
/* #define DEBUG      */              /* Compile debugging version          */
/* #define X86_ASM    */              /* Compile optimised GCC/x86 version  */
/* #define LSB_FIRST  */              /* Compile for low-endian CPU         */
/* #define __64BIT__  */              /* Compile for 64 bit machines        */
/* #define __128BIT__ */              /* Compile for 128 bit machines       */

/****************************************************************************/
/* If your compiler doesn't know about inlined functions, uncomment this    */
/****************************************************************************/
#define INLINE static
#define LSB_FIRST		// VERY IMPORTANT TO GET THIS RIGHT !!


#ifndef EMU_TYPES
#define EMU_TYPES

/****************************************************************************/
/* sizeof(byte)=1, sizeof(word)=2, sizeof(dword)>=4                         */
/****************************************************************************/
typedef unsigned char  byte;
typedef unsigned short word;
typedef unsigned       dword;
typedef signed char    offset;

/****************************************************************************/
/* Define a Z80 word. Upper bytes are always zero                           */
/****************************************************************************/
typedef union
{
#ifdef __128BIT__
 #ifdef LSB_FIRST
	struct { byte l,h,h2,h3,h4,h5,h6,h7,
                 h8,h9,h10,h11,h12,h13,h14,h15; } B;
   struct { word l,h,h2,h3,h4,h5,h6,h7; } W;
   dword D;
 #else
   struct { byte h15,h14,h13,h12,h11,h10,h9,h8,
                 h7,h6,h5,h4,h3,h2,h,l; } B;
   struct { word h7,h6,h5,h4,h3,h2,h,l; } W;
   dword D;
 #endif
#elif __64BIT__
 #ifdef LSB_FIRST
	struct { byte l,h,h2,h3,h4,h5,h6,h7; } B;
   struct { word l,h,h2,h3; } W;
   dword D;
 #else
   struct { byte h7,h6,h5,h4,h3,h2,h,l; } B;
   struct { word h3,h2,h,l; } W;
   dword D;
 #endif
#else
 #ifdef LSB_FIRST
   struct { byte l,h,h2,h3; } B;
   struct { word l,h; } W;
   dword D;
 #else
   struct { byte h3,h2,h,l; } B;
   struct { word h,l; } W;
   dword D;
 #endif
#endif
} pair;

#endif /* EMU_TYPES */

/****************************************************************************/
/*** End of machine dependent definitions                                 ***/
/****************************************************************************/

#ifndef INLINE
#define INLINE static inline
#endif

/****************************************************************************/
/* The Z80 registers. HALT is set to 1 when the CPU is halted, the refresh  */
/* register is calculated as follows: refresh=(Regs.R&127)|(Regs.R2&128)    */
/****************************************************************************/
typedef struct
{
  pair AF,BC,DE,HL,IX,IY,PC,SP;
  pair AF2,BC2,DE2,HL2;
  unsigned IFF1,IFF2,HALT,IM,I,R,R2;
} Z80_Regs;

/****************************************************************************/
/* Set Z80_Trace to 1 when PC==Z80_Trap. When trace is on, Z80_Debug() is   */
/* called after every instruction                                           */
/****************************************************************************/
#ifdef DEBUG
extern int Z80_Trace;
extern int Z80_Trap;
void Z80_Debug(Z80_Regs *R);
#endif

extern int Z80_Running;      /* When 0, emulation terminates                */
//extern int Z80_IPeriod;      /* Number of T-states per interrupt            */
//extern int Z80_ICount;       /* T-state count                               */
extern int Z80_IRQ;          /* Current IRQ status. Checked after EI occurs */

#define Z80_IGNORE_INT  -1   /* Ignore interrupt                            */
#define Z80_NMI_INT     -2   /* Execute NMI                                 */

unsigned Z80_GetPC (void);         /* Get program counter                   */
void Z80_GetRegs (Z80_Regs *Regs); /* Get registers                         */
void Z80_SetRegs (Z80_Regs *Regs); /* Set registers                         */
void Z80_Reset (void);             /* Reset registers to the initial values */
unsigned int  Z80_Execute ();           /* Execute IPeriod T-States              */
word Z80 (void);                   /* Execute until Z80_Running==0          */
void Z80_RegisterDump (void);      /* Prints a dump to stdout               */


void Z80_SetWaitStates (int n);    /* Set number of memory wait states.     */
                                   /* This only affects opcode fetching, so */
                                   /* wait state adjustment is still        */
                                   /* necessary in Z80_RDMEM, Z80_RDOP_ARG, */
                                   /* Z80_RDSTACK and Z80_WRSTACK           */
//void Z80_Patch (Z80_Regs *Regs);   /* Called when ED FE occurs. Can be used */
                                   /* to emulate disk access etc.           */
//int Z80_Interrupt(void);           /* This is called after IPeriod T-States */
                                   /* have been executed. It should return  */
                                   /* Z80_IGNORE_INT, Z80_NMI_INT or a byte */
                                   /* identifying the device (most often    */
                                   /* 0xFF)                                 */
void Z80_Reti (void);              /* Called when RETI occurs               */
void Z80_Retn (void);              /* Called when RETN occurs               */

unsigned int get_ix ();


// MYSTUFF
//extern byte ip_byt;
//extern byte ip_byt_orig;
//extern int ip_port;
//extern int input_pending;
//extern byte ip_shift;
//extern unsigned long cyc;//for sound driver
//extern int cycl_d;//current noof cycles on z80 since last check
void set_IPeriod(int ii);
int get_IPeriod();
void set_cycl_d(int ii);
int get_cycl_d();

void set_ICount(int ii);

void Z80_RegisterDump_String (unsigned char [],unsigned char []);
void set_hl (unsigned int huj);
void set_de (unsigned int huj);
void  set_pc (unsigned int huj);
unsigned int get_de ();
void set_sound_running (int x);
unsigned int get_sound_running ();
unsigned int get_pc ();
void set_blank_mono(char oij);

unsigned int get_port84cyc();
void set_tapecyc(unsigned int ii);
unsigned int get_tapecyc();
//static byte enable_soundstream;
//extern byte tbyt;

//extern byte bank_latch;
//extern byte video_latch;
//extern byte port84;
void  set_regs (Z80_Regs i);
void  get_regs (Z80_Regs *i);




//HERE FROM RAZE
typedef enum {
  Z80_REG_AF=0,
  Z80_REG_BC,
  Z80_REG_DE,
  Z80_REG_HL,
  Z80_REG_IX,
  Z80_REG_IY,
  Z80_REG_PC,
  Z80_REG_SP,
  Z80_REG_AF2,
  Z80_REG_BC2,
  Z80_REG_DE2,
  Z80_REG_HL2,
  Z80_REG_IFF1,        /* boolean - 1 or 0 */
  Z80_REG_IFF2,        /* boolean - 1 or 0 */
  Z80_REG_IR,
  Z80_REG_IM,          /* 0, 1, or 2 */
  Z80_REG_IRQVector,   /* 0x00 to 0xff */
  Z80_REG_IRQLine      /* boolean - 1 or 0 */
} z80_register;


/* Fix this as you need it */
#ifndef UBYTE
  #define UBYTE unsigned char
#endif
#ifndef UWORD
  #define UWORD unsigned short
#endif

//FROM RAZE
UWORD z80_get_reg(z80_register reg);
void  z80_set_reg(z80_register reg, UWORD value);
void  z80_reset(void);
int   z80_emulate(int cycles);
void  z80_raise_IRQ(UBYTE vector);
void  z80_lower_IRQ(void);
//void  z80_cause_NMI(void);
void z80_stop_emulating(void);
int z80_get_cycles_elapsed(void);
 
/****************************************************************************/
/* Definitions of functions to read/write memory and I/O ports              */
/* You can replace these with your own, inlined if necessary                */
/****************************************************************************/
#include "Z80IO.h"

#endif /* _Z80_H */


