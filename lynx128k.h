#ifndef __LYNX128K_H__
#define __LYNX128K_H__

void initmem128k();
void update_vid_maps128k();
void outp_128(UWORD port, UBYTE value);
Uint8 getpixel_128(Uint16 offset, Uint8 bit);
Uint8 getbyte_128(Uint8 block,Uint16 offset, Uint8 bit);
unsigned int getword_128(Uint8 bank, Uint16 byte_offset, Uint8 byte);
void update_keyrate128K();
void bank_latch128k(unsigned char value);
void vid_latch128k(unsigned char value);

#endif
