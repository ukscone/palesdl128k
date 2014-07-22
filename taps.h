#ifndef __TAPS_H__
#define __TAPS_H__

int load_lynx_tap(const char* fn,const char* fn2,int tape_type);
void set_t_mode(int ff);
int save_lynx_tap();
int load_level9_tap(char fn[],char fn2[]);
int save_LSF(char *fn);
int load_LSF(char *fn);
extern unsigned int tape_mode;
void init_tapestuff();

#define TAP_TAPES 0
#define LSF_TAPES 1
#define LEV9_TAPES 2
#define RAW_TAPES 3
#define WAV_TAPES 4

#endif
