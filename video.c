#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <SDL/SDL.h>

#include "pale.h"
#include "io.h"
#include "gui.h"
#include "fonts.h"
#include "lynx128k.h"
#include "video.h"

//#define VIDEO_METHOD SDL_SWSURFACE 
//|SDL_RESIZABLE
//|SDL_HWPALETTE
#define VIDEO_METHOD SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_HWPALETTE
//|SDL_RLEACCEL 
//|SDL_ASYNCBLIT  - for multiproc systems


int StretchEnable=0;    //Enables VIdeo Stretch function

SDL_Surface *screen;	//The main reference to the screen buffer

int trippy_mode = 0;
int isFullscreen = 0;

SDL_Color LynxPalette[16] = {{0,0,0,0},{0,0,255,0},{255,0,0,0},{255,0,255,0}, \
        {0,255,0,0},{0,255,255,0},{255,255,0,0},{255,255,255,0},{0,0,0,0},{0,0,128,0}, \
	{128,0,0,0},{128,0,128,0},{0,128,0,0},{0,128,128,0},{128,128,0,0},{128,128,128,0}};

void clearcrtc()
{
    for(int f=0;f<18;f++)
        CRTC_reg[f]=0;
}

void video_fullscreen(int x)
{
    isFullscreen = x;
        
    if(x == 1)
    {
        //Do This twice to make sure w2k blacksout the border area
        if(useDoubleSize == 1)
	{
	    screen = SDL_SetVideoMode(1024,512, 8,  VIDEO_METHOD | SDL_FULLSCREEN);
	}
	else
	{
	    screen = SDL_SetVideoMode(512,256, 8,  VIDEO_METHOD | SDL_FULLSCREEN);
	}
    }
    else
    {
        if(useDoubleSize  == 1)
	{
            screen = SDL_SetVideoMode(1024,512, 8,  VIDEO_METHOD);
	}
	else
	{
	    screen = SDL_SetVideoMode(512,256, 8,  VIDEO_METHOD);
	}
    }
    SDL_DisplayFormat(screen);
    SDL_SetColors(screen, LynxPalette, 0, 8);
}

void set_screenres()
{
    if(useDoubleSize  == 1)
    {
        screen = SDL_SetVideoMode(1024,512, 8,  VIDEO_METHOD);
    }
    else
    {
        screen = SDL_SetVideoMode(512,256, 8,  VIDEO_METHOD);
    }
    if ( screen == NULL )
    {
        gui_error("Couldnt get Video MODE");
        exit(1);
    }

    if ( SDL_DisplayFormat(screen)==NULL )
    {
        gui_error("Couldnt get Video MODE");
        exit(1);
    }

    // Set palette
    SDL_SetColors(screen, LynxPalette, 0, 8);
}

void initialise_display(void)
{
    char lbl[200];
 
    // Initialize the SDL library (Video/Sound/Timer)
    if( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0 )
    {
        printf("Couldn't initialize SDL: %s\n", SDL_GetError());
        gui_error("Couldnt get Video MODE");
	exit(1);
    }

    isFullscreen = 0;
    set_screenres();

    // Set window title
    sprintf(lbl,"PaleSDL-Pi Ver:%.2f",PALE_REV);
    SDL_WM_SetCaption(lbl,NULL);

    printf("Starting fonts\n");

    init_fonts(64,32);
}


void end_frame()
{
    if(useScanlineUpdates == 0)
    {
	if(usePageFlipping == 1)
	{
	    SDL_Flip(screen);
	}
	else
	{
	    if(useDoubleSize == 1)
	    {
	        SDL_UpdateRect(screen, 0,0,1024,512);

	    }
	    else
	    {
	        SDL_UpdateRect(screen, 0,0,512,256);
	    }
	}
    }
}

//Returns ZERO if end of screen
int draw_scanline(int scanline)
{
	SDL_Rect retRect;
	Uint8 *bits, bank, skip, temp,temp2;
	unsigned int memy, memx, memy2;
	static int hor_disp_pix,hor_disp_bytes,vrt_disp_pix;
	int ret,line,scptr;
        static int vrt_disp_6845pixperchar,vrt_disp_chars;
	unsigned char *bnk2;
 	unsigned char scline[1024];
     	static int sync_width,vert_sync_pos;
	unsigned vid_start;

	

 
// FIXME - this next line works for the Level 9 adventures, but with artifact lines (the flashing pink mush is accurate I think , check)
// it is linked to the line blanking monostable variable 
//if(Line_Blank==1 && (video_latch & 0x20)!=0x20)
// if((show_bank2==1 || show_bank3==1) &&
 if((video_latch & 0x60)!=0x20)
 {
// - this test is now done at a higher level in the main emu loop

  	        if(!trippy_mode)
		{
			for(int wxa=0;wxa<hor_disp_pix ;wxa++)
		        {
			  scline[wxa]= 0 ;
			}
		}
		
		if(scanline>vrt_disp_pix)
		{
			//end_frame();
			//hor_disp_bytes=32;
			hor_disp_bytes=CRTC_reg[1];
			if(hor_disp_bytes>64)hor_disp_bytes=64;
	            	if(hor_disp_bytes<2)hor_disp_bytes=2;
	     
			//hor_disp_pix=hor_disp_bytes*8;
			if(useDoubleSize == 1)
			{
					hor_disp_pix = 1024;
			}
			else			
			{
					hor_disp_pix = 512;
			}
		/*
			vrt_disp_6845pixperchar=CRTC_reg[9]+1;  //usually 3+1 = 4
			vrt_disp_chars=CRTC_reg[6];  //usually 3f, 40 in Pengo
			vrt_disp_pix=vrt_disp_chars*vrt_disp_6845pixperchar;
			if(vrt_disp_pix<32)vrt_disp_pix=32;
			if(vrt_disp_pix>256)vrt_disp_pix=256;
			
			if (CRTC_reg[3]>0x37)
				sync_width=(CRTC_reg[3]-0x37)/4;
			else
				sync_width=0;
			
			if (CRTC_reg[6]>0x3f)
				vert_sync_pos=(CRTC_reg[6]-0x3f)/4;
			else
				vert_sync_pos=0;
		*/

//FIXME - debug showing fps
			vrt_disp_pix= 247;
	
			return(0);//end of frame
		}

		vid_start=(CRTC_reg[12]*256+CRTC_reg[13])*4 ;//0000 for 48/96  4000h for 128

		vid_start=vid_start%0x4000;//for 128k,
 	    
		memy=scanline*hor_disp_bytes+vid_start;

		for(bank = 0;bank < 3;bank++)
		{
			skip = 0;

			// Get (the start) pointer to the scanline LHS in the Lynx memory
			memy=memy%0x4000;
	                if(show_allbanks==1)
	                {
	                        switch(bank)
                                {
                                        case 0:
                                                bnk2 =  (unsigned char *)&bank2[0x4000+memy];  //red
                                                break;
                                        case 1:
                                                bnk2 =  (unsigned char *)&bank2[0x0000+memy]; //blue
                                                break;
                        		case 2:
                                                if(show_alt_green==1)
                                			bnk2 =  (unsigned char *)&bank2[0xc000+memy];  //alt green mirror
                                		else
                                			bnk2 =   (unsigned char *)&bank2[0x8000+memy];  //green mirror
                                                break;
                                }
	                }
	                else
                                skip =  1;
	
		         // by here bnk2 holds the start of the scanline (8 bit pointer)
	
			temp = 7-bank;  // to get colours the correct way way round
			scptr = 0;
			for(int wx=0;wx<hor_disp_bytes;wx++)
			{
				for(int wx2=0;wx2<8;wx2++)
				{
			           if(skip == 0) 
				   {
				        temp2 = ( ((*(bnk2+wx)) << wx2 ) & 0x80 ) >> temp;
					scline[scptr++]+= temp2;
					if(useDoubleSize == 1)
						scline[scptr++]+= temp2;
				   }
	/*
				   else
			           {
				        temp2 = 0;		// blank out if 'not shown'
					scline[scptr++]+= temp2;
					if(useDoubleSize == 1)
						scline[scptr++]+= temp2;
				   }  */
				}
			}
		}

		if(useDoubleSize == 1)
		{
		     memcpy((Uint8 *)screen->pixels+(scanline*2*hor_disp_pix),scline,hor_disp_pix);
		     memcpy((Uint8 *)screen->pixels+((scanline*2+1)*hor_disp_pix),scline,hor_disp_pix);
		}
		else
		{
		     memcpy((Uint8 *)(screen->pixels)+(scanline*hor_disp_pix),scline,hor_disp_pix);
		}

/*
		if(useScanlineUpdates == 1)
		{
			if(useDoubleSize == 1)
			{
				SDL_UpdateRect(screen, 0,scanline,hor_disp_pix,2);
			}
			else
			{
				SDL_UpdateRect(screen, 0,scanline,hor_disp_pix,1);
			}
		}
*/

	}	
	return(1);
}      
