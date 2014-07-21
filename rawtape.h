#ifndef __RAWTAPE_H__
#define __RAWTAPE_H__

extern unsigned char tape_level;
void set_t_mode(int ff);
void update_tape();

extern unsigned int tape_spd;
extern int tape_spd_adjust;
extern unsigned char tape_operation;
 
extern unsigned char raw_threshold;
extern int tape_override;

#define FORCE_OFF 0
#define FORCE_STOP 1
#define FORCE_PLAY 2
#define FORCE_RECORD 3

#define LYNX_MAX_RAW 64000000

//extern unsigned char raw_tape[LYNX_MAX_RAW];
extern unsigned char *raw_tape;
extern unsigned int raw_position;
extern unsigned int raw_play;
extern unsigned int raw_rec;
extern unsigned int raw_samples;
extern unsigned int raw_motor;
extern void start_tape_record();
extern void start_tape_play();
extern void stop_tape();
extern void init_tapestuff();

void set_tape_spd_adjust(int gt);
void  set_raw_thresh(int gt);
void tape_clear();


void force_raw_play();
void force_raw_auto();
void force_raw_rec();
void force_raw_stop();
void rewind_raw();

int load_raw(char fnam[]);
int load_wav(char fnam[]);
int save_raw(char fnam[]);
int save_wav(char fnam[]);


#define TAPE_STOPPED 0
#define TAPE_PLAY 1
#define TAPE_RECORD 2


#endif
