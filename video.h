#ifndef __VIDEO_H__
#define __VIDEO_H__

void initialise_display(void);
Uint8 getpixel(Uint16 offset, Uint8 bit);
int draw_scanline(int scanline);
void query_6845(char *emu_query3,char *emu_query4);
extern unsigned int crtc_reg;
extern unsigned char CRTC_reg[18];
void video_resize(int w,int h,int ressizable);
extern int StretchEnable; 
void set_screenres();
void video_fullscreen(int);

int get_display_w();
int get_display_h();
void end_frame();

extern int isFullscreen;
extern int trippy_mode;


void start_fonts();
void doit();

#define DISPLAY_CUSTOM 0
#define DISPLAY_1X 1
#define DISPLAY_2X 2
#define DISPLAY_FULLSCREEN 3

extern SDL_Surface *screen;
           
#endif 
