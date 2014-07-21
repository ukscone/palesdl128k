#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

#include <SDL/SDL.h>

#include "pale.h"
#include "gui.h"
#include "keys.h"
#include "KOGEL/Z80Dasm.h"
#include "KOGEL/Z80.h"
#include "directory.h"

extern int bank_latch;

#define CHAR_WIDE   8
#define CHAR_HIGH   8
#define MAX_CHAR  256

#define CHAR_SIZE ((CHAR_HIGH * CHAR_WIDE) / 8)

#define CHAR_FILL 0xDB

// Global Variables
unsigned char charmap[MAX_CHAR][CHAR_SIZE];


// Function Prototypes
//void init_fonts(void);
int  load_font(const char* fname);
void showchar(int curr_char, int xpos, int ypos);

void draw_memmap();

int my_screen_w,my_screen_h;


int horiz_vidbytes;

// extern SDL_Surface *screen;
// deja tried this before...
extern SDL_Surface *screen;
extern void end_frame();
 
int disas_bank =0 ;


unsigned char byte_reverse(unsigned char value)
{
    value = (value & 0x0f) << 4 | (value & 0xf0) >> 4;
    value = (value & 0x33) << 2 | (value & 0xcc) >> 2;
    value = (value & 0x55) << 1 | (value & 0xaa) >> 1;
    return value;
}


int load_font(const char* fname)
{
    FILE *fp;
    int i,j;

    fp = fopen(fname, "rb");

    for (i = 0; i < MAX_CHAR; i++) {
        fread(&charmap[i][0], CHAR_SIZE, 1, fp);
        for(j=0;j<CHAR_SIZE;j++)
            charmap[i][j] = byte_reverse(charmap[i][j]);
    }

    fclose(fp);

    return 1;
}

void init_fonts(unsigned int msw,unsigned int msh)
{
    memset(charmap, 0, (MAX_CHAR * CHAR_SIZE));
    load_font("vga8x8.fnt");

	my_screen_w = msw;
	my_screen_h = msh;
	horiz_vidbytes = msw;
}

int mycursor_x = 0;
int mycursor_y = 0;
unsigned char mycolour_foreground = 7;
unsigned char mycolour_background = 0;

void showchar(int curr_char, int xpos, int ypos)
{
    unsigned char *bytes;
    
    int i, j, temp,memx,bank,retbyte,scanline;
    unsigned char paper,ink;
    
    unsigned char *vmem;

	if(useDoubleSize)
	{
	    vmem =  ((Uint8 *)screen->pixels ) +(ypos * CHAR_HIGH * 2 * horiz_vidbytes * 16) + xpos*16;
	}
	else
	{
	    vmem =  ((Uint8 *)screen->pixels ) +(ypos * CHAR_HIGH * horiz_vidbytes * 8) + xpos*8;
	}


    for(scanline = 0;scanline<CHAR_HIGH;scanline++)
    {
        //get pointer to character
        retbyte =  charmap[curr_char][scanline];
	if(useDoubleSize)
	{
	        bytes=vmem+(scanline*2*horiz_vidbytes * 16);   
		for(i = 0; i< 8;i++)
		{
			if(retbyte & (0x80 >> i) )
			{
		  	    *((unsigned char *)(bytes+i*2)) = mycolour_foreground;
		  	    *((unsigned char *)(bytes+i*2+1)) = mycolour_foreground;
		  	    *((unsigned char *)(bytes+horiz_vidbytes * 16+i*2)) = mycolour_foreground;
		  	    *((unsigned char *)(bytes+horiz_vidbytes * 16+i*2+1)) = mycolour_foreground;
			}
			else
			{
		  	    *((unsigned char *)(bytes+i*2)) = mycolour_background;
		  	    *((unsigned char *)(bytes+i*2+1)) = mycolour_background;
		  	    *((unsigned char *)(bytes+horiz_vidbytes * 16+i*2)) = mycolour_background;
		  	    *((unsigned char *)(bytes+horiz_vidbytes * 16+i*2+1)) = mycolour_background;
			}
		}
	}
	else
	{
	        bytes=vmem+(scanline*horiz_vidbytes*8);   
		for(i = 0; i< 8;i++)
		{
			if(retbyte & (0x80 >> i) )
		  	    *((unsigned char *)(bytes+i)) = mycolour_foreground;
			else
		  	    *((unsigned char *)(bytes+i)) = mycolour_background;
		}
	}
    }
    
//    for(memx=0;memx<512;memx++)
  //      *((Uint8 *)(bytes++)) = *((Uint8 *)(bytes-512));
 //   SDL_UpdateRect(screen, xpos*8,ypos * CHAR_HIGH,(xpos+1)*8,(ypos+1) * CHAR_HIGH);
}

void my_paint(unsigned char x1,unsigned char x2,unsigned char y1,unsigned char y2)
{
    unsigned char *bytes;
    unsigned char write_byte;
    int i, j, temp,memx,bank,retbyte,scanline,vert_h,n;
    unsigned char *vmem;
    unsigned int pix_height,bytes_width,bytes_high;
    char lbl[200];
  
 	if(useDoubleSize)
	{
	   vmem = ((Uint8 *)screen->pixels) +(y1 * CHAR_HIGH * 2 * horiz_vidbytes * 16);
    	}
	else
	{
	   vmem = ((Uint8 *)screen->pixels) +(y1 * CHAR_HIGH * horiz_vidbytes * 8);
	}
        bytes_high = y2 - y1;
        pix_height = bytes_high * CHAR_HIGH;
        bytes_width = x2 - x1;

 	if(useDoubleSize)
	{
	        for(scanline = 0;scanline<pix_height*2;scanline++)
	        {
	            bytes=vmem+(scanline*horiz_vidbytes*16)+(x1*16); 
	            for(n = 0;n < bytes_width * 16;n++)
	            {
	                *((unsigned char *)(bytes+n)) = mycolour_foreground ;
	            }
	        }    
	 	   if(!usePageFlipping)
			SDL_UpdateRect(screen, x1*16,y1*16,x2*16,y2*16);
      	}
	else
	{
	        for(scanline = 0;scanline<pix_height;scanline++)
	        {
	            bytes=vmem+(scanline*horiz_vidbytes*8)+(x1*8); 
	            for(n = 0;n < bytes_width * 8;n++)
	            {
	                *((unsigned char *)(bytes+n)) = mycolour_foreground;
	            }
	        }    
	        if(!usePageFlipping)
		 	SDL_UpdateRect(screen, x1*8,y1*8,x2*8,y2*8);
	}
 

}

void my_move(unsigned char x,unsigned char y)
{
    mycursor_x = x;
    mycursor_y = y;
}

void my_print(const char* text)
{
    char c;
    int c_pos = 0;

    c = text[c_pos++];
    while(c!='\0')
    {
        switch(c)
        {
            case '\b'://backspace
                if(mycursor_x > 0)mycursor_x--;
                break;
            case '\t'://tab
                if(mycursor_x < (my_screen_w-7))mycursor_x +=5;
                break;
            case '\n'://newline
                mycursor_x = 0;
                mycursor_y ++;
                break;
            case '\f'://form feed -  here used as set foreground colour
                c = text[c_pos++];
                mycolour_foreground = c - '0';
                break;
            case '\a'://beep -  here used as set background colour
                c = text[c_pos++];
                mycolour_background = c - '0';
                break;

            default:
		showchar(c,mycursor_x,mycursor_y);
                mycursor_x ++;
                break;
        }
        
        if(mycursor_x > my_screen_w)
        {
            mycursor_x = 0;
            mycursor_y ++;
        }
        if(mycursor_y > my_screen_h)
            mycursor_y = 0;

        c = text[c_pos++];
    }

   if(!usePageFlipping) 
   {
	   if (useDoubleSize)
		 SDL_UpdateRect(screen, 0,0,512,512);
	   else
		 SDL_UpdateRect(screen, 0,0,256,256);
   }
}

void my_input(char *prompt,char *text)
{
    char c;
    char lbl[200];
    char charout[4];
    int text_ptr = 0;
    //Get input string from the user

    char cursor[5] = {'<',' ','\b','\b','\0'};

    sprintf(lbl,"%s",prompt);
    my_print(lbl);
    my_print(cursor);

    end_frame();

// for the console terminal
//    c = getchar();
//    scanf("%s",text);
//    return;

  
    c = my_getkey();
    while(c != 13 && c != 27)
    {
	if(c==0)
	{
	}
	
	else if(c == 8)//backspace
        {
            if(text_ptr > 0)
            {
                text_ptr--;
                my_print("\b");
                my_print(cursor);
            }
        }
        else
        {
            text[text_ptr++] = c;
            sprintf(charout,"%c",c);        
            my_print(charout);
            my_print(cursor);
            //my_print(" -");
            //sprintf(lbl," %d ",(unsigned int)c);        
            //my_print(lbl);
    
            if(text_ptr>50) break;
        }
	end_frame();
        c = my_getkey();
    }
    text[text_ptr] = '\0';
}

void show_help(int waitforinput)
{
    char name[200];
    char lbl[200];

//      my_show_speed();

//vidmem = VIDEO_PAGE1;
//Another way of doing it
//vidmem = vga_mem;

    my_print("\f0");
    my_paint(0,32,5,31);
    my_move(0,5);
    my_print("\f4____________________________");  
	my_move(0,7);
        my_print("        \f6P\f4a\f2l\f3e\f5SDL\n\f6        Version:"); 
        sprintf(lbl,"%.2f",PALE_REV);
        my_print(lbl);
  
/*if(SDL_MUSTLOCK(screen) == 0)  
    my_print("\nMustlock is 0\n");
	else
    my_print("\nMustlock is nonzero\n");
*/
    my_print("\f7\a0\n\n");
    my_print("F1  - Show Help\n");
    my_print("F2  - Show Status\n");
    my_print("F3  - Pause Emu\n");
    my_print("F4  - Toggle Speed\n");

    my_print("F5  - Change Machine\n");
    my_print("F6  - Load TAP File\n");
    my_print("F7  - Load LDF Disk File\n");
    my_print("F8  - Toggle Fullscreen mode\n");

    my_print("F9  - Save LSF Snapshot\n");
    my_print("F10 - Load LSF Snapshot\n");
    my_print("F11 - Hard Reset Machine\n");
    my_print("F12 - Quit Pale\n\n");
    my_print("'-help' for more information\n");

    my_print("\f7\a1");
    my_print("\n\n    \f6Peter Todd 2003 - 2012\n\n");
    my_print("Kogel (z80) Elliott (Libdsk)\n");
    my_print("\f7\a0\n");
    my_print("\f4____________________________");  

    my_move(0,29);
//    if(waitforinput==0)
  //     my_input("Press Return ",name);
}



void my_show_directory(char *pathext,unsigned int highlight,char *name)
{
    char cmdresult[4000];
    my_print("\f0");
    my_paint(0,31,0,30);
    my_print("\f2\a0");
    my_move(0,0);
    get_directory(pathext,cmdresult,highlight,name);
    
    my_print(cmdresult);
    my_print("\f7\a0");
}



void my_filechooser(char *pathext,char *name)
{
        unsigned int highlight = 0;
        char list_dir[256];

      Uint8 *key;
      while(1)
        {       
                my_show_directory(pathext,highlight,name);
                end_frame();
		while(1)
                {
  			SDL_PumpEvents();
			key = SDL_GetKeyState(NULL);
                      
			if (key[SDLK_F12])
                        {
                                finish_emu=1;
                        	return;
                        }
                        if (key[SDLK_ESCAPE])
                                return;
                        if (key[SDLK_UP])
                        {
                                waitkeyup(SDLK_UP);
                                if(highlight>0)highlight--;
                        	break;
                        }
                        if (key[SDLK_DOWN])
                        {
                                waitkeyup(SDLK_DOWN);
                                highlight++;
                                break;          
                        }
                        if (key[SDLK_RETURN])
                                return;
                }       
        }
}



void my_show_speed()
{
    char lbl[20];
    int s = emuspeeds[emu_speed];
    my_print("\f0");
    my_paint(0,10,0,1);
    my_print("\f4\a0");
    my_move(0,0);
    sprintf(lbl,"%d\%",s);
    my_print(lbl);
    my_print("\f7\a0");
}

extern int bank_latch,video_latch;


void get_statusregs(char* lbl)
{
        sprintf(lbl,
      //          "PC:%04X IR: %02X AF:%04X HL:%04X\nDE:%04X BC:%04X SP:%04X IX:%04X\nIY:%04X",
                "PC:%04X AF:%04X HL:%04X SP:%04X\nDE:%04X BC:%04X\nIX:%04X IY:%04X  IR:%02X\n\f6Video:%02X Bank:%02X DBank: %d\f7",
            z80_get_reg(Z80_REG_PC),
                z80_get_reg(Z80_REG_IR),
                z80_get_reg(Z80_REG_AF),
                z80_get_reg(Z80_REG_HL),
                z80_get_reg(Z80_REG_DE),
                z80_get_reg(Z80_REG_BC),
                z80_get_reg(Z80_REG_SP),
                z80_get_reg(Z80_REG_IX),
                z80_get_reg(Z80_REG_IY),
                                (unsigned char)video_latch,
                                (unsigned char)bank_latch,disas_bank);
}

void get_statusstack(char *lbl)
{
        unsigned int i,sp;
        char lbl1[300],lbl2[300],lbl3[300],lbl4[300];
        //STACK
        sp=z80_get_reg(Z80_REG_SP);
        
        sprintf(lbl2," ");
        for (i=0;i<5;++i)
        {
                sprintf(lbl3,"%02X%02X ",bank1[sp+i*2],bank1[sp+i*2]);
                strcat(lbl2,lbl3);
        }
        strcpy(lbl,lbl2);
}



int disassemble (char *destin,unsigned int jogval,int bank)
{
        int g,retr,f,start,mid,end,linecount;
        unsigned int curp,peecee;
        char lbl[200];
        char dis_buf[4000];
        char return_char[4];


        //Poss might have to increase this
        // to avoid routine below scanning backwards into other vars space
        peecee=z80_get_reg(Z80_REG_PC);
        curp=peecee-8+jogval;
        
        //skip forward to the relevant byte
        sprintf(return_char,"\n");
        sprintf(dis_buf,"\0");
        mid=0;
        for (f=0;f<12;f++)
        {
                if(curp==peecee)
                {
                        mid=strlen(dis_buf);
                        sprintf(lbl,"->");
                        strcat(dis_buf,lbl);
                }
                sprintf(lbl,"%4X - ",curp);
                strcat(dis_buf,lbl);
//              retr=Z80_Dasm((unsigned char *)bank0+curp,lbl,curp);
                if(bank==0)
                                retr=Z80_Dasm((unsigned char *)bank0+curp,lbl,curp);
                else if(bank==1)
                                retr=Z80_Dasm((unsigned char *)bank1+curp,lbl,curp);
                else if(bank==2)
                                retr=Z80_Dasm((unsigned char *)bank2+curp,lbl,curp);
                else if(bank==3)
                                retr=Z80_Dasm((unsigned char *)bank3+curp,lbl,curp);
                else if(bank==4)
                                retr=Z80_Dasm((unsigned char *)bank4+curp,lbl,curp);
                //              retr=Z80_Dasm((unsigned char *)bank0+curp,lbl,curp);
//              retr=Z80_Dasm((unsigned char *)bank0+curp,lbl,curp);
                if(retr !=0)
                {
                    strcat(dis_buf,lbl);
                    strcat(dis_buf,return_char);
                    curp+=retr;//advance to the next opcode
                }
                else
                    curp++; //advance to the next opcode
        }
        end=strlen(dis_buf);

        if (mid!=0)
        {
                //okay, we've got a listing, now search thru
                //to fnd the PC line, and then take the 14 lines either side of it
                //and copy them to the output buffer
                start=0;
                f=0;    
                //Skip to PC line
//              while(dis_buf[f++]!='_')
//              {
//                      if(f>end)return(0);
//              };
                //Now go back 6 lines
                f=mid;
                for(g=0;g<4;g++)
                {
                        while(dis_buf[f--]!='\n')
                        {
                                if(f<=0)return(0);
                        }
                }
                start=f+2;
                for(f=start;f<end;f++)
                        destin[f-start]=dis_buf[f];
        }
                else return(0);

//if(end > 150)
  //  end = 150;
                for(f=0;f<end;f++)
                        destin[f]=dis_buf[f];
                destin[f]='\0';
        return(1);
}


void draw_overlay()
{
    char txt1[1000],txt2[1000];

   // my_move(0,0);
   // query_6845(&txt1,&txt2);
   // my_print(txt1);
    my_print((char *)"\f0");
    my_paint(0,32,12,26);
    my_print((char *)"\f4\a0");
    my_move(0,12);

    disassemble (txt2,0,disas_bank);
    
    my_print(txt2);

    my_print((char *)"\f0");
    my_paint(0,32,6,12);
    my_print((char *)"\f5");
    my_move(0,7);
    get_statusregs(txt2);
    my_print((char *)txt2);
//	get_statusstack(txt2);
//      my_print((char *)txt2);
    my_print((char *)"\f4");
    sprintf(txt1,"\n\f2Machine Type: \f5%d\f2",mc_type);
    my_print((char *)txt1);
    sprintf(txt1,"  Emu Speed: \f5");
//    sprintf(txt1,"%d",emu_speed);
    my_print((char *)txt1);
    switch(emu_speed)
	{
		case 0:	    sprintf(txt1,"25%%"); break;
		case 1:	    sprintf(txt1,"50%%"); break;
		case 2:	    sprintf(txt1,"100%%"); break;
		case 3:	    sprintf(txt1,"200%%"); break;
		case 4:	    sprintf(txt1,"400%%"); break;
		case 5:	    sprintf(txt1,"800%%"); break;
	}	
    my_print((char *)txt1);
    my_move(0,25);
    my_print((char *)"F1 - s.step F5 - fps\n");
    my_print((char *)"F9 - bank  F10 - memmap");
    if(show_memmap == 1)
	draw_memmap();
 
}

void draw_memmap()
{

	unsigned char*   vmem = ((Uint8 *)screen->pixels);
	int y = 0;
	for(int x = 0; x < 65536; x++)
	{
		switch( disas_bank)
		{
			case 0:	*(vmem + y++) = bank0[x];
				if(useDoubleSize)
					*(vmem + y++) = bank0[x];
				break;
			case 1:	*(vmem + y++) = bank1[x];
				if(useDoubleSize)
					*(vmem + y++) = bank1[x];
				break;
			case 2:	*(vmem + y++) = bank2[x];
				if(useDoubleSize)
					*(vmem + y++) = bank2[x];
				break;
			case 3:	*(vmem + y++) = bank3[x];
				if(useDoubleSize)
					*(vmem + y++) = bank3[x];
				break;
			case 4:	*(vmem + y++) = bank4[x];
				if(useDoubleSize)
					*(vmem + y++) = bank4[x];
				break;
		}
	}
	if(useDoubleSize)
		SDL_UpdateRect(screen, 0,0,512,512);
	else
		SDL_UpdateRect(screen, 0,0,256,256);

}



extern unsigned int fps_limiter_delay;



void draw_overlay2()
{
    char txt1[1000];
    my_print("\f0");
    my_paint(0,10,31,32);
    my_print("\f7");
    my_move(0,31);
    sprintf(txt1,"FPS: %u Del: %u ", current_fps,fps_limiter_delay);
    my_print(txt1);
}


void draw_overlay3(unsigned int x)
{
    char txt1[1000];
    my_move(0,30);
    sprintf(txt1,"Value: %u", x);
     my_print(txt1);
}


