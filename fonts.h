#ifndef __FONTS_H__
#define __FONTS_H__

// Function Prototypes
void init_fonts(unsigned int,unsigned int);
int  load_font(const char* fname);
void showchar(int , int , int );
void my_print(const char* text);
void my_paint(unsigned char ,unsigned char ,unsigned char ,unsigned char );
void my_input(char *,char *);
void show_help(int);
void my_show_directory(char *);
void my_show_speed();

void my_move(unsigned char ,unsigned char );

void my_filechooser(char *,char *);
void draw_overlay();

void draw_overlay2();
void draw_overlay3(unsigned int);

#endif
