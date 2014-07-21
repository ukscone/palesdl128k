#ifndef __PALE_H__
#define __PALE_H__

#define USE_SDL 

#include "KOGEL/Z80.h"
#include "KOGEL/kogelmem.h"
#include "KOGEL/kogelio.h"

#include <SDL/SDL.h>
#ifndef UBYTE
#define UBYTE unsigned char
#define UWORD unsigned short
#endif

#define LYNX_HARDWARE_48 0
#define LYNX_HARDWARE_96 1
#define LYNX_HARDWARE_128 2
#define LYNX_HARDWARE_192 3
#define LYNX_HARDWARE_256 4
#define LYNX_HARDWARE_HACKSPECCY 5
#define LYNX_HARDWARE_JASPER 6
#define LYNX_MAXMEM 65536
#define LYNX_SPEED_START 50000

#define TAP_BASIC       0x42
#define TAP_BINARY      0x4d   //Lynx Binary format
#define TAP_DATA        0x41 //Level 9 Data


#define PALE_REV 2.10


extern int mc_type;

extern int hw_type;
extern int mach;
extern int show_status;
extern int show_status2;
extern int show_keycodes;

extern int debug;

/* allocate lynx/z80 memory banks/io ports */
extern UBYTE z80ports_in[0x10000];          /* I/O ports */
extern UBYTE z80ports_out[0x10000];          /* I/O ports */
extern UBYTE    bank0[LYNX_MAXMEM];               /* Rom */
extern UBYTE    bank1[LYNX_MAXMEM];               /* User Ram */
extern UBYTE    bank2[LYNX_MAXMEM];          /* Red 0x0000/Blue 0x2000*/
extern UBYTE    bank3[LYNX_MAXMEM];       /* AltGreen 0x0000/Green 0x2000*/
extern UBYTE    bank4[LYNX_MAXMEM];

extern unsigned int sound_cycles;
extern unsigned int tapecyc; 


extern long emu_cycles_scanline;
extern long emu_cycles_lineblank;
extern long emu_cycles_vblank;
extern long emu_cycles;

extern int finish_emu;
extern int run_emu;
extern int emu_speed;
extern int trace;
extern int trap_pc;
extern int SoundEnable;
extern int useLCDmode;
extern int usePageFlipping;
extern int useScanlineUpdates;
extern int useDoubleSize;

extern int current_fps;

extern float emuspeeds[];


float get_pale_rev();
void bit(unsigned char *vari,unsigned char posit,unsigned char valu);


void tracedump(UWORD pc);


UBYTE lynx_inp(UWORD port);
void lynx_outp(UWORD port, UBYTE value);
void load_Rom(void);
void gomain(void);
void set_speed(int x);
void set_machine(int x);

void initialise_Lynx(void);
void stop_emu();
void start_emu();
void quit_emu();
void reset_emu();
void single_step_emu();
void update_disksize_gui();


#endif
