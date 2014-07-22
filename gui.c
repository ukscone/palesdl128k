#include "fonts.h"
#include "pale.h"

int emu_display = 0;
int show_sysvars = 0;
int show_memmap = 0;
int show_status = 0;
int show_status2 = 0;
int show_keycodes = 0;
int memmap_bankno = 0;

int show_tapemon = 0;

int show_memscan = 0;

void update_tape_gui(){};
void update_tape_gui_speed(int speed){};

void gui_error(const char* lbl)
{
  printf("\nPete Meditation ERROR:%s\n",lbl);
};

void update_gui_mtype(){};
void update_sysvars(){};
void update_memmap(){};

void update_disksize_gui(){};
void update_disk_gui(){};


void update_gui_speed(){};
void update_sysvar(){};
void update_tape_monitor(){};
void update_memscan(){};


void set_memmap_bankno( int j){};
void set_show_memmap(unsigned int j){
	show_memmap = j;
};
void set_show_status(unsigned int j){};
void set_show_sysvars(unsigned int j){};
void set_display(){};
void set_SoundEnable(){};
