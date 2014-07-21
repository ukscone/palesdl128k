#ifndef __IO_H__
#define __IO_H__

extern unsigned int crtc_reg;
extern unsigned char CRTC_reg[18];

UBYTE lynx_common_inp( UWORD port);
void lynx_common_outp(UWORD port, UBYTE value);
void clearports();
//MOVE
extern unsigned char sound_port;
extern unsigned char video_latch,bank_latch;
extern unsigned char Line_Blank;
extern unsigned int scanline;
extern unsigned char show_allbanks,show_bank1,show_bank2,show_bank3,show_alt_green,speaker_enable;
void update_vid_bank_latches();

#endif 
