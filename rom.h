#ifndef __ROM_H__
#define __ROM_H__

int build_rom(const char* r0,const char* r1,const char* r2,const char* r3,const char* r4,const char* r5,const char* r6,const char* r7,const char* rpath);

int load_lynx_rom();
void patch_lynx_rom();
void create_dummy_rom();
void saveROMconfigfile();
int loadROMconfigfile();
void init_ROMsets();
void load_romset(int f);
extern char romset_files[8*8][40];
extern unsigned char romset_hwtypes[8];
extern unsigned char rompath[200];

#endif
