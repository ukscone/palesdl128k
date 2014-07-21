#if !defined(__APPLE__)
#include <malloc.h>
#endif
#include <string.h>

#include "pale.h"
#include "io.h"
#include "rawtape.h"
#include "sound.h"

#define LYNX_SNDMEM 512
#define LYNX_MYSNDMEM LYNX_SNDMEM

unsigned char sound_linbuf[LYNX_MYSNDMEM];
unsigned int  sound_timings[LYNX_MYSNDMEM];
unsigned char sound_linbuf2[LYNX_MYSNDMEM];
unsigned int sound_div_in=1;
unsigned int sound_ptr_a=0;
unsigned int sound_ptr_b=0;
unsigned int sound_ptr_a2=0;
unsigned int sound_ptr_b2=0;




void get_sound_status(char *txt,char *txt2)
{
        char lbl1[300],lbl2[300],lbl3[300],lbl4[300];
        int i;


        sprintf(lbl2," ");
        for (i=0;i<10;++i)
        {
                sprintf(lbl3,"%02X ",sound_linbuf[(sound_ptr_b+i)%LYNX_MYSNDMEM]);
                strcat(lbl2,lbl3);
        }
        strcpy(txt,lbl2);

        sprintf(lbl2," ");
        for (i=0;i<10;++i)
        {
                sprintf(lbl3,"%04X ",sound_timings[(sound_ptr_b+i)%LYNX_MYSNDMEM]);
                strcat(lbl2,lbl3);
        }
        strcpy(txt2,lbl2);


}


void update_sound()
{
        static char sound_port_last=0;
        static int time_last=0;
        int time_this;
        int time_diff;
        static int togg=0;

        //CAllback buffer is expected to be 22kHz sampled
        //so create a 22kHz wave from the IO port samples and timings we have got
//      time_this=z80_get_cycles_elapsed();     //returns noof cycles elaps since last call to z80_emulate
//      time_this+=sound_cycles;
        //time_this=sound_cycles;
        //sound_cycles=0;

        //time_diff=time_this;                            


//time_diff= time_diff & 0xfffff;

//      SDL_LockAudio();

for(int y=0;y<4;y++)
{
	if (speaker_enable==1)
	        sound_linbuf[sound_ptr_a]=(sound_port*4);  //+(tape_level);
	else
	        sound_linbuf[sound_ptr_a]=0;  //+(tape_level);
                
        sound_ptr_a=(sound_ptr_a+1)%LYNX_MYSNDMEM;
        if(sound_ptr_a==sound_ptr_b)                                    //'STOP THE CIRC BUFFER IF SOUND OUT TOO SLOW
        {
                sound_ptr_a=(sound_ptr_a-1)%LYNX_MYSNDMEM;      //'CHECK THIS
        }
 

}
       
//      SDL_UnlockAudio();

}


// AUDIO CALLBACK FUNCTION
// AUDIO CALLBACK FUNCTION
// AUDIO CALLBACK FUNCTION
// AUDIO CALLBACK FUNCTION
// AUDIO CALLBACK FUNCTION
// AUDIO CALLBACK FUNCTION
// AUDIO CALLBACK FUNCTION

#define SOUND_DIVY 8    // 182

//              sampleng=sound_timings[sound_ptr_b];


void my_audio_callback(void *userdata, Uint8 *stream, int len)
{
        unsigned int i,sampleng,stopit;
        unsigned char sample,z;
        unsigned int f;

//    SDL_LockAudio();


        i=0;
        while(i<len)
        {
                sample=sound_linbuf[sound_ptr_b];
                //sampleng=sound_timings[sound_ptr_b];
                stream[i++]=sample;

/*
                if(sampleng>SOUND_DIVY)
                {
                        sound_timings[sound_ptr_b]-=SOUND_DIVY;
                }
                else
                {
 */                       sound_linbuf[sound_ptr_b]=0;
                        sound_timings[sound_ptr_b]=0;
                        sound_ptr_b=(sound_ptr_b+1)%LYNX_MYSNDMEM;
                        if(sound_ptr_b==sound_ptr_a)                                    //'STOP THE CIRC BUFFER IF SOUND OUT TOO SLOW
                        {
                                sound_ptr_b=(sound_ptr_b-1)%LYNX_MYSNDMEM;      //'CHECK THIS
                        }
 /*                       sound_timings[sound_ptr_b]+=sampleng;
                }
*/        }


  //   SDL_UnlockAudio();

/*

// TEST TONE
        i=0;
        f=0;
        z=0;
        while(i<len)
        {
                stream[i++]=z;  //*4
                f++;
                if(f>31)                        // Note that this is a divisor of LYNXSNDMEM so we dont get glitches
                {
                        f=0;
                        if(z!=0)
                                z=0;
                        else
                                z=0x3f;
                }       
        }

*/


}



/* Open the audio device */
SDL_AudioSpec *desired, *obtained;
SDL_AudioSpec *hardware_spec;

void init_sound()
{
       /* Allocate a desired SDL_AudioSpec */
        desired = (SDL_AudioSpec*)malloc(sizeof(SDL_AudioSpec));

        /* Allocate space for the obtained SDL_AudioSpec */
        obtained = (SDL_AudioSpec*)malloc(sizeof(SDL_AudioSpec));

        /* 22050Hz - FM Radio quality */
        desired->freq=22050;

        /* 16-bit signed audio */
        desired->format=AUDIO_U8;

        /* Mono */
        desired->channels=1;

        /* Large audio buffer reduces risk of dropouts but increases response time */
        desired->samples=LYNX_SNDMEM;

        /* Our callback function */
        desired->callback=my_audio_callback;

        desired->userdata=NULL;

        /* Open the audio device */
        if ( SDL_OpenAudio(desired, obtained) < 0 ){
                printf("Cant Open the Audio\n");
        }
        /* desired spec is no longer needed */
        free(desired);
        hardware_spec=obtained;


        /* Prepare callback for playing */
        for(int f=0;f<LYNX_SNDMEM;f++)
        {
                sound_linbuf[f]=0;
                sound_timings[f]=0;
                sound_linbuf2[f]=0;
        }


        /* Start playing */
        SDL_PauseAudio(0);
}


/*
//SDL_TimerID sound_timer;


if(SDL_PollEvent(&event)==1)
{
    if(event.user.code==UPDATE_SOUND && SoundEnable)
                flush_sound();  
}


const int UPDATE_SOUND = 1;
Uint32 SoundTimer(Uint32 interval, void* param)
{
    SDL_Event sound_event;
 // gui_error("HEllo");

    sound_event.type = SDL_USEREVENT;
    sound_event.user.code = UPDATE_SOUND;
    sound_event.user.data1 = 0;
    sound_event.user.data2 = 0;
    SDL_PushEvent(&sound_event);
    return interval;
}

//      SDL_RemoveTimer(sound_timer);

*/

/*
typedef struct{
  int freq;
  Uint16 format;
  Uint8 channels;
  Uint8 silence;
  Uint16 samples;
  Uint32 size;
  void (*callback)(void *userdata, Uint8 *stream, int len);
  void *userdata;
} SDL_AudioSpec;

AudioSpec *aud;

//      SDL_Event event;
//  START SOUND TIMER
//      sound_timer = SDL_AddTimer(40, SoundTimer, 0);


void init_sound()
{
        aud.freq=22050;
        aud.format=;
        aud.channels=1;
        aud.silence=;
        aud.samples=4000;
        aud.size=1;
        v
v


}

*/
