// PALE SDL PI Version 
//
// Pete Todd Mid Wales Uk, 2003 - 2012
// Russell Davis 2012 - 2014
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "pale.h"
#include "KOGEL/Z80.h"
#include "KOGEL/kogelmem.h"
#include "KOGEL/kogelio.h"
#include <SDL/SDL.h>
 
// This is a temporary endpoint for calls carried over from the DOSPALE PALEDOS emulators
// which haven't been written for the current system
// mostly  they will be GUI calls for displaying output 
#include "gui.h"
#include "io.h"
#include "keys.h"
#include "taps.h"
#include "sound.h"
#include "rom.h"
#include "rawtape.h"
#include "disk.h"
#include "fonts.h"
#include "lynx128k.h"
#include "video.h"

#define FPS_LIMITER_DELAY_START 1

//MEMORY IO STUFF
UBYTE    z80ports_in[0x10000];            /* I/O ports */
UBYTE    z80ports_out[0x10000];           /* I/O ports */
UBYTE    bank0[LYNX_MAXMEM];              /* Rom */
UBYTE    bank1[LYNX_MAXMEM];              /* User Ram */
UBYTE    bank2[LYNX_MAXMEM];              /* Red 0x0000/Blue 0x2000*/
UBYTE    bank3[LYNX_MAXMEM];              /* AltGreen 0x0000/Green 0x2000*/
UBYTE    bank4[LYNX_MAXMEM];

float emuspeeds[10]={25,50,100,200,400,800,1000,2000,5000,32000};
int emu_speed=2;
int hw_type=LYNX_HARDWARE_128;
int mc_type=0;
int ss_emu=0;
int run_emu=1;
int finish_emu=0;
int debug=0;
int SoundEnable=0;
int fps_counter=0;
int current_fps = 0;
int fps_updated = 0;

unsigned int fps_limiter_delay = 5;
int useFPSlimiter = 0;

int usePageFlipping = 0;
int useScanlineUpdates = 0;
int useDoubleSize = 1;
int useShowFPS = 0;
int nosplash = 0;

int trap_pc=0xffff;
double  time_per_fps_disp;
unsigned int last_fps_time;
unsigned int newtime;
int framecount;
unsigned int scanline;

long emu_cycles_scanline=256;
long emu_cycles_lineblank=88;
long emu_cycles_vblank=400;
long emu_cycles=50000;
long emu_cycles_temp;

unsigned int sound_cycles=0;
unsigned int tapecyc=0; 
int trace=0;
FILE *tracedump_file;
char *loadfilename;

extern void initmem();
extern void set_speed(int );

float get_pale_rev()
{
  return((float)PALE_REV);
}

void bit(unsigned char *vari,unsigned char posit,unsigned char valu)
{
  if (valu==0)
     *vari=(*vari & (255-((unsigned char)1 << posit)));
  else
     *vari=(*vari | ((unsigned char)1 << posit));
}

void init_fps()
{
  time_per_fps_disp = CLOCKS_PER_SEC * .5; // .5 seconds 
  last_fps_time = SDL_GetTicks();
  fps_counter=0;
  current_fps = 0;
  fps_updated = 0;
}

void single_step_emu()
{
  ss_emu=1;
}

void tick_fps()
{
        // called every frame (if enabled)
        newtime = SDL_GetTicks();
        framecount++;
	if ((newtime-last_fps_time) > 500) //time_per_fps_disp)
        {
            //update_fps_counter( (int)((CLOCKS_PER_SEC * framecount) / (newtime -last_fps_time)));
            //printf(" %d ",(int)((CLOCKS_PER_SEC * framecount) / (newtime -last_fps_time)));
		current_fps = framecount * 2;
		if(useShowFPS)
			draw_overlay2();
		framecount=0;
	        last_fps_time = newtime;
		fps_updated = 1;
        }
}



void initialise_Lynx(void)
{
        printf("In Init Lynx\n");
        memset(&bank0[0x0000], 0xFF, LYNX_MAXMEM);
        memset(&bank1[0x0000], 0xFF, LYNX_MAXMEM);
        memset(&bank2[0x0000], 0xFF, LYNX_MAXMEM);	// zeroed to avoid screen flash on boot
        memset(&bank3[0x0000], 0xFF, LYNX_MAXMEM);
        memset(&bank4[0x0000], 0xFF, LYNX_MAXMEM);
        clearports();
        load_romset(mc_type);
        printf("Loading the ROM\n");
        if(!load_lynx_rom())
        {
 	       printf("Couldnt load the built Lynx ROM\n");
                exit(1);
        }
        printf("Initialising Lynx Memory\n");
	
	//Must come after ROMset load so we know what the hardware is
	initmem();

        printf("Setting Tape mode\n");
        set_t_mode(tape_mode);
        printf("Init tapestuff\n");
        init_tapestuff();
        
        printf("Init diskstuff\n");
	init_diskstuff();

//        set_d_mode(0);
//	load_raw_disk("");
	
	z80_reset(); 

	fps_limiter_delay = FPS_LIMITER_DELAY_START;
        
	if(useShowFPS || useFPSlimiter)
	{
		printf("Initialising FPS counter\n");
        	init_fps();
        }
}


void set_machine(int x)
{
  mc_type=x;

  printf("Initialising Lynx - machine type: %u\n",x);
  initialise_Lynx();
        
  printf("Setting Initial Speed - emu_speed %u\n",emu_speed);
  set_speed(emu_speed);
}

void scan_arguments(int argc, char *argv[])
{
       int i;

	for(i = 1;i<argc;i++)
	{
	
	    if(strcmp(argv[i],"-flip") == 0)
	    {
	        usePageFlipping = 1;
	    }
	
	    if(strcmp(argv[i],"-scan") == 0)
	    {
	        useScanlineUpdates = 1;
	    }
	    if(strcmp(argv[i],"-sound") == 0)
	    {
	        SoundEnable = 1;
	    }
	    if(strcmp(argv[i],"-origsize") == 0)
	    {
	        useDoubleSize = 0;
	    }
	    if(strcmp(argv[i],"-showfps") == 0)
	    {
	        useShowFPS = 1;
	    }
	    if(strcmp(argv[i],"-speed") == 0)
	    {
		sscanf(argv[i+1],"%u",&emu_speed);
		if(emu_speed < 6)
	   	   set_speed(emu_speed);
	    }
	    if(strcmp(argv[i],"-mctype") == 0)
	    {
		sscanf(argv[i+1],"%u",&mc_type);
		if(mc_type < 6)
	   	   (mc_type);
	    }
	    if(strcmp(argv[i],"-fpslimit") == 0)
	    {
	        useFPSlimiter = 1;
	    }

	    if(strcmp(argv[i],"-snapshot") == 0)
	    {
	        loadfilename = argv[i+1];
	        printf("Load LSF Filename is %s\n",loadfilename);
	    }

	    if(strcmp(argv[i],"-trippy") == 0)
	    {
	        trippy_mode = 1;
	    }

	    if(strcmp(argv[i],"-nosplash") == 0)
	    {
	        nosplash = 1;
	    }


	    if(strcmp(argv[i],"-help") == 0)
	    {
//FIXME - refactor - merge with onscreen help output display
		printf("\n\nPaleSDL PI Pete Todd 2003-2012 Version:%.2f\n\noptions\n",PALE_REV);
		printf("-speed	0 - 6 (2 = 100%)\n");
		printf("-mctype	0 - 6 machine type\n");
		printf("-flip	use page flipping if available\n");
		printf("-scan   use accurate per scanline emulation (slow)\n");
		printf("-sound  enable sound output\n");
		printf("-origsize  original video size\n");
		printf("-showfps	show fps counter\n");
		printf("-fpslimit	autolimit to 25fps\n");
		printf("-snapshot <filename>	load LSF snapshot on boot\n");
		printf("-trippy		as described (try PARTB demo or YnxVaders)\n");
		printf("-nosplash	as described\n");
		printf("\n");
		printf("Uses sourcecode from Marcel de Kogel (z80) and John Elliott (Libdsk)\n");
		printf("and from the Open Source SDL and GNU utilities\n");
		printf("Many helped in bringing you the Pale emulator:\n");
		printf("Russell Davis, Martyn Smith, John Koushappas, Colin Clayman\n");
		printf("John Roberts, Bob Jones, Adrian Graham, Simon Owen, James Boillee\n");
		printf("and Tigger, Harriet, Ebony, Thomas, Jasper, Felix, The Squirt\n");
		printf("and The Mouse (see embedded biocrypt)\n");
		exit(0);
	    }
	
	}
}

extern  UBYTE video_latch_last ;

// ------------------------------------------------------------------ 
int main(int argc, char *argv[])
{
        FILE *handle;
        char *filenam;
        static char scan_lbl[10];
        char lbl[200];
        static int last_time;
        int f_delay=10;
        int this_time,wait_time,frame_time,divisor;

        char cmd[20];
 

	printf("\nPaleSDL PI Pete Todd 2003-2012 Version:%.2f\n",PALE_REV);
	printf("use -help for options\n");
	
	scan_arguments(argc, argv);

        if(SoundEnable)
	{
	        printf("Initialising sound\n");
		init_sound();
	}


        initialise_display();


  	printf("initialising ROM sets\n");

        init_ROMsets();
        if(!loadROMconfigfile())
        {
  		printf("Saving new ***ROM*** config file\n");
                saveROMconfigfile();
  		printf("Loading new ***ROM*** config file\n");
                loadROMconfigfile();
        }

 	set_machine(mc_type);
 
        init_keys();

	// Start with the end of the frame - forces a reload of the 6845 calculations
	// which are only done on a per frame basis (move the calc code to the beginning of vid code to change to scan perfect emu)
        scanline = 255;

        show_status =0;
        run_emu = 1;

	if(loadfilename!=NULL)
	{ 
		set_t_mode(LSF_TAPES);
		sprintf(lbl,"snaps/");
   	    	strcat(lbl,loadfilename);
		strcpy(loadfilename,lbl);
		if(load_LSF(loadfilename))
			printf("LSF loadfilename - loaded\n");
		else
			printf("LSF loadfilename - not loaded\n");
	}
	set_t_mode(TAP_TAPES);

	if(nosplash==0)
	{
		show_help(1);// show help screen as a splash, dont wait for an input
		SDL_Flip(screen);
		SDL_Delay(800);
	}
    	printf("Starting main loop\n");
	while (finish_emu==0) 
	{   
             //   CheckKeys();
                
                if(run_emu)
                {
                        //to get at least 22khz sampling on sound 250*50fps ish chek!
                        //NOTE!! This division will copmletely screw up the 
                        //point of setting a definite noof cycles
                        //so change this NOW!!!
                        
                        //Line_Blank is a global for the blanking monostable
                        //when portXX is set the z80 freezes till end of next scanline i.e. here!
                        //it is then reset

			Line_Blank=0;
			
			//This bit needs fixing for PENGO to work without tearing appearing
			//pengo works on performing video writes durin gthe line blanking period
			//when line blank bit is set the z80 should freeze until the start of the blanking periood
			// however pengo and ftext use lineblank writes

	                sound_cycles+=z80_emulate(emu_cycles_lineblank+emu_cycles_scanline);//22 us Line Blanking Execution
			if(SoundEnable)
				update_sound();




/*

			//These next two shouldn't be run here, certainly not if the emulate above got stopped by z80_stop_emulating()
			//which is set off by the line mono being written to (out port)
			
			// Check line lblank here - if its hnged then abnort next lot of emlatioin cycles
			if(Line_Blank == 0)sound_cycles+=z80_emulate(emu_cycles_scanline/4);//64 us Scanline execution
			if(SoundEnable)
				update_sound();
			if(Line_Blank == 0)sound_cycles+=z80_emulate(emu_cycles_scanline/4);//64 us Scanline execution
			if(SoundEnable)
				update_sound();
			if(Line_Blank == 0)sound_cycles+=z80_emulate(emu_cycles_scanline/4);//64 us Scanline execution
			if(SoundEnable)
				update_sound();
			if(Line_Blank == 0)sound_cycles+=z80_emulate(emu_cycles_scanline/4);//64 us Scanline execution
			if(SoundEnable)
				update_sound();
*/

                }
                else
                {
                        if(ss_emu)
                        {
                                z80_emulate(1);
                                ss_emu=0;
                        }
                }               

                // FRAME BLANKING PERIOD
                // FRAME BLANKING PERIOD
                // FRAME BLANKING PERIOD
                if((Line_Blank == 0) && draw_scanline(scanline++)==0)        //draw returns zero if its end of frame
                {
                        if(show_status)draw_overlay();

			end_frame();

			update_keyrate(); //Update the keyboard repetition and delay rates for diff emu speeds
                      
			CheckKeys();	
                        scanline = 0;
                        
			if(useShowFPS || useFPSlimiter)
				tick_fps();
                        
                        
			//The next bit is important for CPM to work 
         		z80_lower_IRQ();
	

                        //Should be 160 T States with IRQ held low by CURSOR 6845 signal
			if(run_emu)
                        {
                                 sound_cycles+=z80_emulate(emu_cycles_vblank);
 				 if(SoundEnable)
					update_sound();
                        }

                        
			z80_raise_IRQ(0xFF);
	
                        if(run_emu)// && (Line_Blank == 0))
                        {
				sound_cycles+=z80_emulate(emu_cycles);  //main impact of speedup
 				if(SoundEnable)
					update_sound();
                        }
			if(useFPSlimiter)
			{
			  if(fps_updated)
			  {
				fps_updated = 0;
				if(current_fps > 25)
				{
					// Limit to 20ms delay for os background 
					if((fps_limiter_delay+=1) > 20)
						fps_limiter_delay = 20;
				}
				else
				{
					if((fps_limiter_delay-=1) < 1)
						fps_limiter_delay = 1;
				}
				SDL_Delay(fps_limiter_delay);
			  }
			}
                }
	}
    	printf("Out of main loop\n");
        kill_disks();
  	close_keys();
        return 1;
}

void stop_emu()
{
  run_emu=0;
}

void start_emu()
{
  run_emu=1;
}

void quit_emu()
{
  finish_emu=1;
}

void reset_emu()
{
  initialise_Lynx();
}
