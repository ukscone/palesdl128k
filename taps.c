#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pale.h"
#include "taps.h"
#include "keys.h"
#include "io.h"
#include "gui.h"


unsigned char lbuffer[LYNX_MAXMEM];//leave room for 'big' prog
int file_id; //type offile loaded
unsigned int tape_mode=TAP_TAPES;
unsigned char taprom[30];

extern void set_vid_latch(unsigned char);


int load_lynx_tap(const char* fn,const char* fn2,int tape_type)
{
        FILE *lhandle;
        FILE *lhandle2;
        unsigned int  buf_p=0,quot=0,size_read,load_address;
        char lbl[100];
        unsigned char csum;
        unsigned int cdd,f,ret;
        unsigned int tap_leng,exec_address, buf_pale;
        char csum_ok[10];


        pump_string("mload\"\"\x0d");

        
        // open a file for input                  
        lhandle = fopen((const char *) fn, "rb" );
        if(lhandle!=NULL)
        {
                if( !(size_read = fread( lbuffer,1,0x10000,lhandle)) )
                {
                        return(0);
                }
                else
                {
                  
                  fclose( lhandle );
                  if( size_read == -1 )
                                        return(0);
                }
        }
        else
        {
                sprintf(lbl,"Couldn't Open TAP file %s",fn);
                gui_error(lbl);
                return(0);
        }

        if(tape_type==0) //standard tape with name
        {
                //Get filename - skip to second "
                while (quot<2)
                {
                        if(lbuffer[buf_p++]=='"')
                                quot++;
                }
                lbuffer[buf_p-1]='\0';
                lbuffer[0]=' ';         //wipe out first " ready for printing
        }

        //If next char is A5 we forgot to remove it from the TAP file !
        //we could have either 4d 'M' mc or 42 'B' basic, *** or 41 - 'A' level 9 data ***
        //If next char is 42 only then we have a basic proggy
        if(lbuffer[buf_p]==0xa5)
        {
                buf_p++;
                file_id=lbuffer[buf_p];
                buf_p++;
        }
        else
        {
                file_id=lbuffer[buf_p];
                buf_p++; //skip over the 42 ( or 4D) B or M (when no a5 there - everyone but me !)
        }

        //Get Length
        if(file_id==TAP_BASIC)
        {
                load_address=bank1[0x61fa]+256*bank1[0x61fb];//should be 694D
                tap_leng=lbuffer[buf_p]+256*lbuffer[buf_p+1];
                buf_p+=2;
        }
        else if(file_id==TAP_BINARY)
        {
                tap_leng=lbuffer[buf_p]+256*lbuffer[buf_p+1];
                load_address=lbuffer[buf_p+2]+256*lbuffer[buf_p+3];
                buf_p+=4;
        }else //DATA - swap dest and length
        {
                load_address=lbuffer[buf_p]+256*lbuffer[buf_p+1];
                tap_leng=lbuffer[buf_p+2]+256*lbuffer[buf_p+3];
                buf_p+=4;
        }

        buf_pale=load_address;
        //Get Prog
        csum=0;
        for(f=0;f<tap_leng;f++)
        {
                csum+=lbuffer[buf_p];//only used for binary MLOADed progs & Level 9 Data
                bank1[buf_pale++]=lbuffer[buf_p++];
        }

        //dec ptr to point to last byte of prog in memory
        buf_pale--;

        if(file_id==TAP_BASIC)
        {
                //MessageBox(NULL,"Updating 61fc","PALE Debug",MB_ICONINFORMATION);

                //Update end of program pointer in the BASIC os 
                bank1[0x61fc]=buf_pale % 256;
                bank1[0x61fd]=buf_pale / 256;
        }

        sprintf(csum_ok,"No Checksum");
        //skip over next two bytes if binary file - csum related
        if(file_id==TAP_BINARY || file_id==TAP_DATA)
        {
                sprintf(csum_ok,"Checksum OK");
                if(csum!=lbuffer[buf_p++])
                {
                        //MessageBox(NULL,"Bad Checksum Possibly for this File?","PALE Debug",MB_ICONINFORMATION);
                        sprintf(csum_ok,"Checksum BAD");
                }
                if(lbuffer[buf_p++]==0x4e)
                {  }
                        //                      MessageBox(NULL,"Got a funny N byte thing after the checksum!","PALE Debug",MB_ICONINFORMATION);
        }
        
        //Get Execution Addr
        if(file_id==TAP_BASIC || file_id==TAP_BINARY)
                exec_address=lbuffer[buf_p]+256*lbuffer[buf_p+1];
        else
                exec_address=0;

        if(file_id==TAP_BASIC || file_id==TAP_BINARY)
                sprintf((char*)fn2,"Name:%s \n ID:%02x \n Start %04x \n End %04x \n Length %04x \n Run %04x\n %s",lbuffer,file_id,load_address,buf_pale,tap_leng-1,exec_address,csum_ok);
        else  //Data
                sprintf((char*)fn2,"Name: n/a \n ID: %04x \n Start %04x \n End %04x \n Length %04x \n Run %04x",file_id,load_address,buf_pale,tap_leng-1);


/*
        if(breakin==1)
        {
                set_hl(buf_pale);
                set_de(0);
                set_pc(0xcfb);  //jump back into the ROM load routine - this one to the prompt (but MEM not set correctly)
        }

*/

        if(exec_address!=0 && file_id!=TAP_DATA)
        {
                if(file_id==TAP_BINARY)
                {
                        z80_set_reg(Z80_REG_HL,exec_address);//as the ROM does it
                        z80_set_reg(Z80_REG_PC,exec_address);  //jump to the invaders routine :))
                }
                else
                {
                        z80_set_reg(Z80_REG_HL,buf_pale);//end byte of program
                        z80_set_reg(Z80_REG_DE,exec_address);
                        z80_set_reg(Z80_REG_PC,0xcc1);  //jump back into the ROM load routine
                }
        }
        else

        {
                z80_set_reg(Z80_REG_HL,buf_pale);
                z80_set_reg(Z80_REG_DE,exec_address);
                z80_set_reg(Z80_REG_PC,0xcfb);  //jump back into the ROM load routine - this one to the prompt (but MEM not set correctly)
        }
        return(1);
}



int save_lynx_tap()
{
        unsigned int  buf_p=0,quot=0,size_read,load_address;
        unsigned char buffer[65536];//leave room for 'big' proggy ;)
        char lbl[200],plab[200],pnam[200],fnam[200];
        unsigned char csum;
        unsigned int cdd,f,ret,e1,e2,e3;
        unsigned int tap_leng,exec_address, buf_pale,end_address,start_address,prog_size;
        FILE*  handle2;
    int size_written,end_nam;
        //jump here from ROM save routine
        //get filename and any parameters
        //save header
        //save program between bounds
        //for(f=0;f<10;f++)
        //{
        //      plab[f]=pale_mem[e1+f];         
        //}
        //plab[10]='/0';




        e1=z80_get_reg(Z80_REG_DE);
        quot=0;buf_p=0;
        //Get filename - skip to second "
        while (quot<2)
        {
                plab[buf_p]=bank1[e1+buf_p];
                if(bank1[e1+buf_p++]=='"')
                        quot++;
        }
        plab[buf_p-1]='\0';
        plab[0]=' ';            //wipe out first " ready for save

        e2=strlen(plab);
        for(f=0;f<e2;f++)
                pnam[f]=plab[f+1];

        strcat(pnam,".TAP");    

	sprintf(fnam,"./tap/");
	strcat(fnam,pnam);

        printf(lbl,"DE points to %s, pnam is %s",plab,pnam);
        printf(lbl,"fnam is %s",fnam);
	//sprintf(lbl,"DE points to %s, pnam is %s",plab,pnam);
        //MessageBox(NULL,lbl,"PALE ",MB_YESNOCANCEL | MB_DEFBUTTON1);

    /* open a file for output             */
    handle2 = fopen((const char *) fnam,"wb");
    if( handle2 != NULL )
        {
                //write header - the filename
                buffer[0]='"';
                for(f=1;f<e2;f++)
                {
                                buffer[f]=plab[f];              
                }
                end_nam=f;
                buffer[end_nam]='"';
                buffer[end_nam+1]=0x42; //BASIC proggy designator
                start_address=bank1[0x61fa]+256*bank1[0x61fb];//should be 694D
                end_address=bank1[0x61fc]+256*bank1[0x61fd];
                //write tape length (end of program pointer-load address)
                tap_leng=end_address-start_address+1;
                buffer[end_nam+2]=tap_leng % 256;
                buffer[end_nam+3]=tap_leng / 256;
                //write Prog
                csum=0;
                for(f=0;f<tap_leng;f++)
                {
                        buffer[end_nam+4+f]=bank1[start_address+f];
                }
                //write Execution Addr
                //              exec_address=buffer[buf_p]+256*buffer[buf_p+1];
                e2=z80_get_reg(Z80_REG_IX);

        //      sprintf(lbl,"Hello Pete,IX is %4X",e2);
        //      MessageBox(NULL,lbl,"PALE ",MB_YESNOCANCEL | MB_DEFBUTTON1);
                                e1=bank1[0x61fc]+256*bank1[0x61fd];

                                
//WHY OH WHY DONT ASK ME WHY BUT THE LYNX
//LIKES TO HAVE 1600 bytes added to this address !
                                
//e3=1600+((0x10000+e2)-e1)%0xffff;             // rom does IX-(61fc/d)
//                              buffer[end_nam+4+f]=e3 % 256;
//                              buffer[end_nam+4+f+1]=e3 / 256;//these seem to retrurn control okay - should be IX ? based though
//                              buffer[end_nam+4+f+2]=e3 / 256;

                buffer[end_nam+4+f]=0;          //exec addr
                buffer[end_nam+4+f+1]=0;
                buffer[end_nam+4+f+2]=0;        //copy byte

                
                
                prog_size=tap_leng+end_nam+4+3+1;       
                // OKAY, actually write the file to disk
                if( !fwrite( buffer,prog_size,1,handle2))
                {
                        gui_error(":( Couldnt write the TAP file");
                }
                
                //sprintf(lbl,"Hello Pete,I Wrote %d bytes",size_written);
                //MessageBox(NULL,lbl,"PALE ",MB_YESNOCANCEL | MB_DEFBUTTON1);
                fclose( handle2 );
        }
        //return to ROM
        z80_set_reg(Z80_REG_PC,0xc59);  //  do a ret jump back into the ROM 
        return(1);
}

extern void tap_patch_rom();
extern void tap_unpatch_rom();


void set_t_mode(int ff)
{
        unsigned int sav_ad;

        tape_mode=ff;



        if (tape_mode==TAP_TAPES)       //TAP
        {
 		tap_patch_rom();
        }
        else
        {
 		tap_unpatch_rom();
        }
}


int load_LSF(char *fn)
{
        FILE *handle2;
        unsigned int Addr,Count;
        unsigned char Chr,c,vid;
       int len, eof,f;
        char lbl[100];
printf("lsf: fn=%s\n",fn);
   handle2 = fopen((const char *)fn,"rb");
    if( handle2 != NULL )
        {
            //Read in the registers
                z80_set_reg(Z80_REG_AF,(fgetc(handle2)<<8)+fgetc(handle2));
                z80_set_reg(Z80_REG_BC,(fgetc(handle2)<<8)+fgetc(handle2));
                z80_set_reg(Z80_REG_DE,(fgetc(handle2)<<8)+fgetc(handle2));
                z80_set_reg(Z80_REG_HL,(fgetc(handle2)<<8)+fgetc(handle2));
                z80_set_reg(Z80_REG_IX,(fgetc(handle2)<<8)+fgetc(handle2));
                z80_set_reg(Z80_REG_IY,(fgetc(handle2)<<8)+fgetc(handle2));
                z80_set_reg(Z80_REG_PC,(fgetc(handle2)<<8)+fgetc(handle2));
                z80_set_reg(Z80_REG_SP,(fgetc(handle2)<<8)+fgetc(handle2));
                z80_set_reg(Z80_REG_AF2,(fgetc(handle2)<<8)+fgetc(handle2));
                z80_set_reg(Z80_REG_BC2,(fgetc(handle2)<<8)+fgetc(handle2));
                z80_set_reg(Z80_REG_DE2,(fgetc(handle2)<<8)+fgetc(handle2));
                z80_set_reg(Z80_REG_HL2,(fgetc(handle2)<<8)+fgetc(handle2));
                z80_set_reg(Z80_REG_IFF1,fgetc(handle2));
                z80_set_reg(Z80_REG_IFF2,fgetc(handle2));
                z80_set_reg(Z80_REG_IR,(fgetc(handle2)<<8)+fgetc(handle2));
                z80_set_reg(Z80_REG_IM,fgetc(handle2));
                z80_set_reg(Z80_REG_IRQVector,fgetc(handle2));
                z80_set_reg(Z80_REG_IRQLine,fgetc(handle2));

                vid=fgetc(handle2);
                bank_latch=fgetc(handle2);

                set_vid_latch(vid); //must do this to set up proper bank shown bits


                for(f=0;f<18;f++)
                        CRTC_reg[f]=fgetc(handle2);

        eof=0;
 

                Addr=0;
        while(!eof && Addr<0x10000)
        {
                c=fgetc(handle2);
                if (c!=0xED) bank1[Addr++]=c;
                else
                {
                        len=fgetc(handle2);
                        if (!len) eof=1;
                        else
                        {
                                c=fgetc(handle2);
                                while(len--) bank1[Addr++]=c;
                        }
                }
                if (feof(handle2))      eof=1;
        }
//sprintf(lbl,"Bank1 Addr=%d",Addr);
//gui_error(lbl);

                Addr=0;
        while(!eof && Addr<0x10000)
        {
                c=fgetc(handle2);
                if (c!=0xED) bank2[Addr++]=c;
                else
                {
                        len=fgetc(handle2);
                        if (!len) eof=1;
                        else
                        {
                                c=fgetc(handle2);
                                while(len--) bank2[Addr++]=c;
                        }
                }
                if (feof(handle2)) eof=1;
        }

                Addr=0;
        while(!eof && Addr<0x10000)
        {
                c=fgetc(handle2);
                if (c!=0xED) bank3[Addr++]=c;
                else
                {
                        len=fgetc(handle2);
                        if (!len) eof=1;
                        else
                        {
                                c=fgetc(handle2);
                                while(len--) bank3[Addr++]=c;
                        }
                }
                if (feof(handle2)) eof=1;
        }

                fclose(handle2);
        }
        return(1);
}


//LYNX SNAP FORMAT FILES
int save_LSF(char *fn)
{
        FILE *handle2;
        unsigned int Addr,Count,f;
        unsigned char Chr;
        char lbl[100];
    handle2 = fopen((const char *)fn,"wb");
    if( handle2 != NULL )
        {
                fputc(z80_get_reg(Z80_REG_AF)>>8,handle2);
                fputc(z80_get_reg(Z80_REG_AF)&0x00FF,handle2);
                fputc(z80_get_reg(Z80_REG_BC)>>8,handle2);
                fputc(z80_get_reg(Z80_REG_BC)&0x00FF,handle2);
                fputc(z80_get_reg(Z80_REG_DE)>>8,handle2);
                fputc(z80_get_reg(Z80_REG_DE)&0x00FF,handle2);
                fputc(z80_get_reg(Z80_REG_HL)>>8,handle2);
                fputc(z80_get_reg(Z80_REG_HL)&0x00FF,handle2);
                fputc(z80_get_reg(Z80_REG_IX)>>8,handle2);
                fputc(z80_get_reg(Z80_REG_IX)&0x00FF,handle2);
                fputc(z80_get_reg(Z80_REG_IY)>>8,handle2);
                fputc(z80_get_reg(Z80_REG_IY)&0x00FF,handle2);
                fputc(z80_get_reg(Z80_REG_PC)>>8,handle2);
                fputc(z80_get_reg(Z80_REG_PC)&0x00FF,handle2);
                fputc(z80_get_reg(Z80_REG_SP)>>8,handle2);
                fputc(z80_get_reg(Z80_REG_SP)&0x00FF,handle2);
                fputc(z80_get_reg(Z80_REG_AF2)>>8,handle2);
                fputc(z80_get_reg(Z80_REG_AF2)&0x00FF,handle2);
                fputc(z80_get_reg(Z80_REG_BC2)>>8,handle2);
                fputc(z80_get_reg(Z80_REG_BC2)&0x00FF,handle2);
                fputc(z80_get_reg(Z80_REG_DE2)>>8,handle2);
                fputc(z80_get_reg(Z80_REG_DE2)&0x00FF,handle2);
                fputc(z80_get_reg(Z80_REG_HL2)>>8,handle2);
                fputc(z80_get_reg(Z80_REG_HL2)&0x00FF,handle2);
                fputc(z80_get_reg(Z80_REG_IFF1),handle2);
                fputc(z80_get_reg(Z80_REG_IFF2),handle2);
                fputc(z80_get_reg(Z80_REG_IR)>>8,handle2);
                fputc(z80_get_reg(Z80_REG_IR)&0x00FF,handle2);
                fputc(z80_get_reg(Z80_REG_IM),handle2);
                fputc(z80_get_reg(Z80_REG_IRQVector),handle2);
                fputc(z80_get_reg(Z80_REG_IRQLine),handle2);
                fputc(video_latch,handle2);
                fputc(bank_latch,handle2);

                for(f=0;f<18;f++)
                        fputc(CRTC_reg[f],handle2);

                 

            Addr=0x00;
            while(Addr<0x10000)
            {
                    Chr=bank1[Addr];
                    Count=1;
        
                    while((bank1[Addr+Count]==Chr) && ((Addr+Count)<=0x10000))
                            Count++;
        
                    if (Count>240) Count=240;
        
                    if ( Count>3 || Chr==0xed)
                    {
                            fputc(0xed,handle2);
                            fputc(Count,handle2);
                    }
                    else    Count=1;
        
                    fputc(Chr,handle2);
                    Addr+=Count;
            }
        //    fputc(0xed,handle2);
         //   fputc(0x00,handle2);

            Addr=0x00;
            while(Addr<0x10000)
            {
                    Chr=bank2[Addr];
                    Count=1;
        
                    while((bank2[Addr+Count]==Chr) && ((Addr+Count)<=0x10000))
                            Count++;
        
                    if (Count>240) Count=240;
        
                    if ( Count>3 || Chr==0xed)
                    {
                            fputc(0xed,handle2);
                            fputc(Count,handle2);
                    }
                    else    Count=1;
        
                    fputc(Chr,handle2);
                    Addr+=Count;
            }
          //  fputc(0xed,handle2);
          //  fputc(0x00,handle2);

            Addr=0x00;
            while(Addr<0x10000)
            {
                    Chr=bank3[Addr];
                    Count=1;
        
                    while((bank3[Addr+Count]==Chr) && ((Addr+Count)<=0x10000))
                            Count++;
        
                    if (Count>240) Count=240;
        
                    if ( Count>3 || Chr==0xed)
                    {
                            fputc(0xed,handle2);
                            fputc(Count,handle2);
                    }
                    else    Count=1;
        
                    fputc(Chr,handle2);
                    Addr+=Count;
            }
            fputc(0xed,handle2);
            fputc(0x00,handle2);
                fclose(handle2);
        }
        return(1);
}





