#include <stdio.h>

#include "pale.h"
#include "gui.h"
#include "disk.h"
#include "io.h"
#include "rawtape.h"
#include "lynx128k.h"

UBYTE video_latch_last = 0;

void initmem()
{
  // do nothing at present, could dynamically alloc here
}

void get_machine_list(char *result)
{
  sprintf(result,"0 - 128k\n1 - 128k+disk");
}

void set_speed(int x)
{
  emu_speed=x;
  emu_cycles_scanline=(long)(emuspeeds[x]*384)/100; // = 64us Scanline Period
  emu_cycles_lineblank=(long)(emuspeeds[x]*132)/100; // = 22us LineBlank Period
  emu_cycles_vblank=(long)(emuspeeds[x]*240)/100;  //= 40us Vblank(IRQ LOW) Period
  emu_cycles=(long)(emuspeeds[x]*20400)/100;  // = 3.4ms Vblank period      
}

void set_bank_latch(unsigned char value)
{
  bank_latch=value;
}

void set_vid_latch(UBYTE value){

  video_latch=value;

  if (value & 0x10)
     show_alt_green=1;
  else
     show_alt_green=0;
        
  if (value & 0x20)
     show_allbanks=0;
  else
     show_allbanks=1;

  //Line Blanking monostable - freezes z80 till next scanline end
  if (value & 0x40 && Line_Blank==0)
  {
     Line_Blank=1;
     z80_stop_emulating();
  }       
}


unsigned Z80_RDMEM(dword A)
{
        char lbl[100];
        if((bank_latch & 0x08)==0)
        {
                if (A<0x6000)   
                        return(bank0[A]);
                if(bank0[0xe000]!=0xff) //Disk ROM can be turned off
                {
                       if((A>=0xe000)   && ((disk_options & 0x10)==0))
                                return(bank0[A]);
                }
        }
        //THIS IS THE CORRECT PRIORITY FOR CPM TO WORK
        if ((bank_latch & 0x02)==0x02)  // WORKS VIDEO RAM
        {
                        return(bank2[A]);  //other video banks read
        }
        
        if ((bank_latch & 0x04)==0)     //USER RAM
        {
                        return(bank1[A]);
        }

        //      sprintf(lbl," ERROR - couldnt find that memory address PC=%4X   VidLatch=%X BankLatch=%X Addr=%X:( ",get_pc(),video_latch,bank_latch,A);
        //      MessageBox(NULL,lbl,"PALE ",MB_YESNOCANCEL | MB_DEFBUTTON1);
        return(0xff);  //  ;>
}

void Z80_WRMEM(dword A,byte V)
{
        char lbl[100];

        if((bank_latch & 0x80)==0)
        {
                bank1[A]=V;                             //USER MEMORY WRITE
        }
        
	if((bank_latch & 0x40)==0x40) //WORKS (  but note that an else would mean writing to two banks is not catered for
        {
                bank2[A]=V;             //standard video  write to red/blue/green
        }               
                
}


/*
void outp(UWORD port, UBYTE value){
    port = port & 0x00FF;
        switch (port) {
                case 0x82:
                        set_bank_latch(value);
                        break;
                case 0x80:
                        set_vid_latch(value);
                        break;
                default:
                        if(debug==1)
                                printf("Out Port %02X (%02X) detected\n",port,value);
        }
}
*/


unsigned char Z80_In(unsigned int fred)

{
  lynx_common_inp(fred);
}


void Z80_Out(unsigned int port, unsigned char value)
{
   port = port & 0x00FF;
   switch (port) {
     case 0x82:
       set_bank_latch(value);
       break;
     case 0x80:
       set_vid_latch(value);
       break;
   }                
   lynx_common_outp(port,value); 
}



void Z80_Patch (Z80_Regs *Regs)   /* Called when ED FE occurs. Can be used */

                                   /* to emulate disk access etc.           */

{



}


/* This is called after IPeriod T-States */
/* have been executed. It should return  */
/* Z80_IGNORE_INT, Z80_NMI_INT or a byte */
/* identifying the device (most often    */
/* 0xFF)  */
int Z80_Interrupt(void)
{
  return(Z80_IGNORE_INT );
}

void update_keyrate()
{
   static unsigned int new_keyval,looper=0;
   if(bank2[0]!=0) //Check for CPM running, if so dont bother with the keyrate
   {
       if(emu_speed >2 )       //stop keyrepeat for faster emus
       {
            new_keyval=0x0c00*(emu_speed-1);
            bank1[0x6233]=new_keyval % 256;
            bank1[0x6234]=new_keyval / 256;
       }
       else
       {
            new_keyval=0x0c00;
            bank1[0x6233]=new_keyval % 256;
            bank1[0x6234]=new_keyval / 256;
       }
   }
}

extern unsigned char taprom[30];

void tap_patch_rom()
{
       unsigned int sav_ad;
        //LYNX 128 STUFF

        //These are for the TAPE load/save routines
        taprom[0]=bank0[0xd3d];
        bank0[0xd3d]=0xed;      //change Tape Byte output, just return 0 in A ?
        taprom[1]=bank0[0xd3e];
        bank0[0xd3e]=0x00;
        taprom[2]=bank0[0xd3f];
        bank0[0xd3f]=0xc9;      
        taprom[3]=bank0[0xb65];
        bank0[0xb65]=0xc9;      //disabled completely - Read Tape Sync

        taprom[4]=bank0[0xd76];
        bank0[0xd76]=0xed;      //change Read Bit, just return 1 in A
        taprom[5]=bank0[0xd77];
        bank0[0xd77]=0x01;
        taprom[6]=bank0[0xd78];
        bank0[0xd78]=0xc9;

        //NOW disable LOAD & MLOAD completely
//      bank0[0xc95]=0xc3;
//      bank0[0xc96]=0x95;
        //      bank0[0xc97]=0x0c;

        taprom[7]=bank0[0xc92];
        bank0[0xc92]=0xc3;
        taprom[8]=bank0[0xc93];
        bank0[0xc93]=0x92;
        taprom[9]=bank0[0xc94];
        bank0[0xc94]=0x0c;

//      bank0[0x5f2a]=0xc3;
//      bank0[0x5f2b]=0x62;
//      bank0[0x5f2c]=0x3f;
        taprom[10]=bank0[0x5f2a];
        bank0[0x5f2a]=0xc3;
        taprom[11]=bank0[0x5f2b];
        bank0[0x5f2b]=0x2a;
        taprom[12]=bank0[0x5f2c];
        bank0[0x5f2c]=0x5f;


        
        //Patch Save routine to output OUT 93,x trapped here as SAVE
        //jump back in at 0cfb
        sav_ad=0xbcb;
        taprom[13]=bank0[sav_ad+0];
        bank0[sav_ad+0]=0x20;
        taprom[14]=bank0[sav_ad+1];
        bank0[sav_ad+1]=0xf4;
        taprom[15]=bank0[sav_ad+2];
        bank0[sav_ad+2]=0x01;//ld BC,0093
        taprom[16]=bank0[sav_ad+3];
        bank0[sav_ad+3]=0x93;
        taprom[17]=bank0[sav_ad+4];
        bank0[sav_ad+4]=0x00;
        taprom[18]=bank0[sav_ad+5];
        bank0[sav_ad+5]=0xed;//out a (c)
        taprom[19]=bank0[sav_ad+6];
        bank0[sav_ad+6]=0x79;
        taprom[20]=bank0[sav_ad+7];
        bank0[sav_ad+7]=0x00;//never gets to these :)
        taprom[21]=bank0[sav_ad+8];
        bank0[sav_ad+8]=0x00;

        
        //LYNX 128 Patches
        //YEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEESS
//      bank0[0x1f8e]=0xc9;     //disables setup of variables area (at moment VTBL is too low and overwrites jumpblocks)


        //why these two bytes were wrong in the rom I dont know ?)
//      bank0[0x1746]=0x07;  // becomes VTBL at 6210            NOT NEEDED for Russells ROM but here anyway for ROM1281 from Martyn
//      bank0[0x1747]=0x67;             //6211
}

void tap_unpatch_rom()
{
        unsigned int sav_ad;
       //LYNX 128 STUFF

        //These are for the TAPE load/save routines
        bank0[0xd3d]=0xf5;      //change Tape Byte output, just return 0 in A ?
        bank0[0xd3e]=0xe5;
        bank0[0xd3f]=0xd5;      
        
        bank0[0xb65]=0xc5;      //disabled completely - Read Tape Sync

        bank0[0xd76]=0x01;      //change Read Bit, just return 1 in A
        bank0[0xd77]=0x80;
        bank0[0xd78]=0x00;

        //NOW disable LOAD & MLOAD completely
//      bank0[0xc95]=0xc3;
//      bank0[0xc96]=0x95;
        //      bank0[0xc97]=0x0c;

        bank0[0xc92]=0x2a;
        bank0[0xc93]=0xfa;
        bank0[0xc94]=0x61;

//      bank0[0x5f2a]=0xc3;
//      bank0[0x5f2b]=0x62;
//      bank0[0x5f2c]=0x3f;
        bank0[0x5f2a]=0x2a;
        bank0[0x5f2b]=0xee;
        bank0[0x5f2c]=0x61;
        
        //Patch Save routine to output OUT 93,x trapped here as SAVE
        //jump back in at 0cfb
        sav_ad=0xbcb;
        bank0[sav_ad+0]=0xcd;
        bank0[sav_ad+1]=0xb4;
        bank0[sav_ad+2]=0x0b;//ld BC,0093
        bank0[sav_ad+3]=0x13;
        bank0[sav_ad+4]=0x1a;
        bank0[sav_ad+5]=0xcd;//out a (c)
        bank0[sav_ad+6]=0xa6;
        bank0[sav_ad+7]=0x62;//never gets to these :)
        bank0[sav_ad+8]=0xc3;
}

