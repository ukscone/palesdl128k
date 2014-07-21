#ifndef __KOGELIO_H__
#define __KOGELIO_H__


unsigned char Z80_In(unsigned int fred);
void Z80_Out(unsigned int fred,unsigned char mabel);

void Z80_Patch (Z80_Regs *Regs);
int Z80_Interrupt(void);


#endif
