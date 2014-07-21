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


/*
 
typedef enum {
        // The keyboard syms have been cleverly chosen to map to ASCII 
        SDLK_UNKNOWN            = 0,
        SDLK_FIRST              = 0,
        SDLK_BACKSPACE          = 14,
        SDLK_TAB                = 15,
        SDLK_CLEAR              = 0,
        SDLK_RETURN             = 28,
        SDLK_PAUSE              = 0,
        SDLK_ESCAPE             = 1,
        SDLK_SPACE              = 57,
        SDLK_EXCLAIM            = 0,
        SDLK_QUOTEDBL           = 0,
        SDLK_HASH               = 0,
        SDLK_DOLLAR             = 0,
        SDLK_AMPERSAND          = 0,
        SDLK_QUOTE              = 40,
        SDLK_LEFTPAREN          = 0,
        SDLK_RIGHTPAREN         = 0,
        SDLK_ASTERISK           = 0,
        SDLK_PLUS               = 0,
        SDLK_COMMA              = 51,
        SDLK_MINUS              = 12,
        SDLK_PERIOD             = 52,
        SDLK_SLASH              = 0,
        SDLK_0                  = 11,
        SDLK_1                  = 2,
        SDLK_2                  = 3,
        SDLK_3                  = 4,
        SDLK_4                  = 5,
        SDLK_5                  = 6,
        SDLK_6                  = 7,
        SDLK_7                  = 8,
        SDLK_8                  = 9,
        SDLK_9                  = 10,
        SDLK_COLON              = 0,
        SDLK_SEMICOLON          = 39,
        SDLK_LESS               = 0,
        SDLK_EQUALS             = 13,
        SDLK_GREATER            = 0,
        SDLK_QUESTION           = 0,
        SDLK_AT                 = 0,
         
         //  Skip uppercase letters
         
        SDLK_LEFTBRACKET        = 26,
        SDLK_BACKSLASH          = 0,
        SDLK_RIGHTBRACKET       = 27,
        SDLK_a                  = 30,   //was 97
        SDLK_b                  = 48,
        SDLK_c                  = 46,
        SDLK_d                  = 32,
        SDLK_e                  = 18,
        SDLK_f                  = 33,
        SDLK_g                  = 34,
        SDLK_h                  = 35,
        SDLK_i                  = 23,
        SDLK_j                  = 36,
        SDLK_k                  = 37,
        SDLK_l                  = 38,
        SDLK_m                  = 50,
        SDLK_n                  = 49,
        SDLK_o                  = 24,
        SDLK_p                  = 25,
        SDLK_q                  = 16,
        SDLK_r                  = 19,
        SDLK_s                  = 31,
        SDLK_t                  = 20,
        SDLK_u                  = 22,
        SDLK_v                  = 47,
        SDLK_w                  = 17,
        SDLK_x                  = 45,
        SDLK_y                  = 21,
        SDLK_z                  = 44,
        SDLK_DELETE             = 14,
        // End of ASCII mapped keysyms 


        // Arrows + Home/End pad /
        SDLK_UP                 = 72,
        SDLK_DOWN               = 80,
        SDLK_RIGHT              = 77,
        SDLK_LEFT               = 75,

        // Function keys /
        SDLK_F1                 = 59,
        SDLK_F2                 = 60,
        SDLK_F3                 = 61,
        SDLK_F4                 = 62,
        SDLK_F5                 = 63,
        SDLK_F6                 = 64,
        SDLK_F7                 = 65,
        SDLK_F8                 = 66,
        SDLK_F9                 = 67,
        SDLK_F10                = 68,
        SDLK_F11                = 87,
        SDLK_F12                = 88,

        // Key state modifier keys //
        SDLK_CAPSLOCK           = 58,
        SDLK_RSHIFT             = 54,
        SDLK_LSHIFT             = 42,
        SDLK_RCTRL              = 29,
        SDLK_LCTRL              = 29,
        SDLK_LAST

} SDLKey;

*/

#endif
