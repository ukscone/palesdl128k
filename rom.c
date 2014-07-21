#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#include "rom.h"
#include "pale.h"
#include "gui.h"

unsigned char rompath[200];

unsigned char buffer[LYNX_MAXMEM];	//leave room for 64k rom ;)
unsigned char lynx_rom[LYNX_MAXMEM];	// lynx ROM copy - updated to allow for many roms

char romset_files[8*8][40];
unsigned char romset_hwtypes[8];


void create_dummy_rom()
{
  FILE*  handle;
  unsigned int  ret,size_read,f,g,h,size_written;
  handle = fopen( "none","wb");
  for(f=0;f<0x2000;f++)
    buffer[f]=0xFF;
  if(!fwrite( buffer,0x2000,1,handle))
  {
    gui_error(":( Couldnt write the Dummy ROM file - duh!");
    return;
  }
  fclose(handle);
}


int build_rom(const char* r0,const char* r1,const char* r2,const char* r3,const char* r4,const char* r5,const char* r6,const char* r7, const char* rpath)
{
        FILE* handle;
        FILE* handle2;
        unsigned int size_read,f,g,h,size_written;
        unsigned int cdd,retrn,romend;
        int ret;
        char lbl[300];
        char nfil[40];


        for (f=0;f<strlen((const char*)rpath);f++)
                rompath[f]=rpath[f];
        rompath[f]=0;

        //Create a dummy 8k ROM thats empty
        create_dummy_rom();

        //Open up all 8 roms involved in the image
        //and copy them to the current rom in use by the emulator

        //Open up output file
        handle = fopen( "current.rom","wb");
        if(handle!=NULL )
        {
                h=0;
                printf("Opening the ROMs\n");
		//Now open up each of the 8 ROM files
                for(g=0;g<8;g++)
                {
                        if(g==0)strcpy(nfil,r0);
                        if(g==1)strcpy(nfil,r1);
                        if(g==2)strcpy(nfil,r2);
                        if(g==3)strcpy(nfil,r3);
                        if(g==4)strcpy(nfil,r4);
                        if(g==5)strcpy(nfil,r5);
                        if(g==6)strcpy(nfil,r6);
                        if(g==7)strcpy(nfil,r7);
                        printf("%s,",nfil);
                        handle2 = fopen((const char *) nfil,  "rb" );
                        if( handle2 != NULL )
                        {
                                if(  !fread( &buffer,0x2000,1,handle2 ) )
                                {
                                        fclose( handle2 );
                                        fclose( handle );
                                        sprintf(lbl,":( Encountered a bad ROM size for ROM number %d, size was %d",g,size_read);
                                        gui_error(lbl);
                                        return(0);
                                }
                                fclose(handle2);
                                for(f=0;f<0x2000;f++)
                                        lynx_rom[g*0x2000+f]=buffer[f];
                        }
                        else
                        {
                                        fclose( handle );
                                        sprintf(lbl,":( Sorry, couldn't find ROM number %d - %s",g,nfil);
                                        gui_error(lbl);
                                        return(0);
                        }
                    	printf("\n");
   
                }
                //Now copy the file into the current rom - simple write
                if(  !fwrite( lynx_rom,0x10000,1,handle) )
                {
                        gui_error(":( Couldnt write the current.rom file - duh!");
                        return(0);
                }
                fclose(handle);
        }
        else
        {
                gui_error(":( Couldn't open up the current.rom file");
                return(0);
        }
        return(1);
}

int load_lynx_rom()
{
        FILE*  handle,handle2;
        unsigned int  size_read;
        unsigned int cdd,f,romend;
        int ret;        
        char lbl[200];

        handle = fopen("current.rom", "rb" );
        if( handle != NULL )
        {
          if( !fread( &buffer,1,sizeof( buffer ),handle ) )
          {
                        gui_error("Found current.rom but not the right size!");
                    fclose( handle );
                        return(0);
          }
          fclose( handle );
        }
        else
        {
                gui_error("Couldnt Load current.rom for working");
                return(0);
        }
        romend=0x10000;
        for(f=0;f<romend;f++)
                bank0[f]=buffer[f];
        return(1);
}

void saveROMconfigfile()
{
        FILE *otpf;
                char lbl[40];
                int f,g;
                otpf=fopen("128k.cfg","wt");
        if(otpf==NULL)
        {
                sprintf(lbl,"Cannot open ROM Config file for writing");
                                gui_error(lbl);
                return;
        }
        fprintf(otpf,"===================================================================\n");
        fprintf(otpf,"PALE rom configuration file\n");
        fprintf(otpf,"===================================================================\n");
                for(f=0;f<8;f++)
                {
                        for(g=0;g<8;g++)
                        fprintf(otpf,"SET%1dROM%1d=%s\n",f,g,romset_files[f*8+g]);
                fprintf(otpf,"HWTYPE%1d=%1d\n",f,romset_hwtypes[f]);
                }
        printf("Saved ROM Config file\n");
	fclose(otpf);
}

int loadROMconfigfile()
{
        int tempi1, tempi2;
                static int ftimes=1;
                static int rgain=1;
                int currline=0;
        unsigned long tempiul;
                float tempf;
        char *lval,label[200];
        char bytes[3];
        FILE *inpf;
                char lbl[60];

        inpf=fopen("128k.cfg","rt");
        if(inpf==NULL)     //Check to see if we can open requested file
        {
                sprintf(lbl,"could not open ROM Config file for reading");
                                gui_error(lbl);
                return(0);
        }
        else
        {
                while(fgets(label,200,inpf)!=NULL)
                {
                        lval=strstr(label,"SET");
                        if(lval!=NULL)
                        {
                                sscanf(lval+3,"%d",&tempi1);
                               sscanf(lval+7,"%d",&tempi2);
                                                                //This is to remove any newline chaaracter that may be at end of line

//NOTE THIS ASSUMES THAT THERE IS CRLF at EOL -   THIS ISNT ALWAYS THE CASE PETE!!


                                                                strncpy(romset_files[tempi1*8+tempi2],lval+9,strlen(lval+9)-1);

                        }
                        lval=strstr(label,"HWTYPE");
                        if(lval!=NULL)
                        {
                                sscanf(lval+6,"%d",&tempi1);
                               sscanf(lval+8,"%d",&tempi2);
                                                                romset_hwtypes[tempi1]=tempi2;
                        }
                }
        }
        fclose(inpf);
                return(1);
}

void init_ROMsets()
{
  int f,g;
  for(f=0;f<8;f++)
  {
    for(g=0;g<8;g++)
      sprintf(romset_files[f*8+g],"none");
    romset_hwtypes[f]=0;
  }
}

void load_romset(int f)
{
  int ret;
  //Load in the specific ROM for the mctype
  ret=build_rom(romset_files[f*8],romset_files[f*8+1],romset_files[f*8+2],
                romset_files[f*8+3],romset_files[f*8+4],romset_files[f*8+5],
                romset_files[f*8+6],romset_files[f*8+7],".");
  hw_type=romset_hwtypes[f];
}
