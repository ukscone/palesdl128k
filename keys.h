#ifndef __KEYS_H__
#define __KEYS_H__
void CheckKeys(void);
void update_keyrate();

void pump_string(const char* s);

void init_keys();
void close_keys();
extern char key[];
void waitkeyup(Uint16);
Uint16 my_getkey();

#endif
