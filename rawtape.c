#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#include "pale.h"
#include "io.h"
#include "taps.h"
#include "rawtape.h"
#include "gui.h"

#define  BASE_SAMPLE_RATE 22050

#define  TAPE_CYCS_RECORD_128 158
#define  TAPE_CYCS_PLAY_128 114

#define         BASE_RAW_BUFLEN 1  // 8Mb

unsigned int tape_spd=TAPE_CYCS_PLAY_128;
int tape_spd_adjust=0;

int tape_override=FORCE_OFF;

unsigned int raw_buflen;//set by init
unsigned char *raw_tape;

unsigned int raw_position=0;
unsigned int raw_play=0;
unsigned int raw_rec=0;
unsigned int raw_samples=0;
unsigned int raw_motor=0;
unsigned char tape_level=0;
unsigned int tapecyc_last=0;
unsigned int tapewid;
unsigned int tapewid_last=0;
unsigned int tot_tape_cycles=0;
unsigned int raw_sample_rate=BASE_SAMPLE_RATE;

unsigned char raw_threshold=0x70;//70 is good for Colossal Advent 80 for Zen (tape 3)

unsigned char tape_inverted=0;

unsigned char tape_operation=TAPE_STOPPED;


int load_raw(char fnam[])
{
        FILE *handle;
        int ret,errorr,g,h;
        int  size_read,size_written;
        int cdd,f;
        char lbl[200];

        handle=fopen( fnam, "rb" );
        if( handle != NULL )
        {
          raw_samples = fread(raw_tape,1,raw_buflen,handle);
          fclose( handle );
          if( raw_samples == -1 )
          {
                                gui_error(":( Couldn't Read the Input file");
                                return(1);
          }
        }
        else
        {
                                gui_error(":( Couldn't Read the Input file");
                return(1);
        }
        raw_position=0;

        if(raw_samples==raw_buflen)
                                gui_error("RAW file too big");
        
        
        return(raw_samples);                    
}



int save_raw(char fnam[])
{
        FILE *handle;
        FILE *handle2;
        int size_written,size_read,ret;
        
        /* open a file for output                 */
        handle2 = fopen( fnam,"wb");
        if( handle2 != NULL )
        {
                size_written = fwrite(raw_tape,1,raw_position,handle2 );
                fclose( handle2 );
        }
        else
        {
                                gui_error(":( Couldn't Open output file");
                return(1);
        }
        raw_position=0;
        return(0);
}
/*
'-----------------------------------------------------------------------------------
' Wave File Format
'-----------------------------------------------------------------------------------
' RIFF Chunk   ( 12 bytes)
' 00 00 - 03  "RIFF"
' 04 04 - 07  Total Length to Follow  (Length of File - 8)
' 08 08 - 11  "WAVE"
'
' FORMAT Chunk ( 24 bytes )
' 0C 12 - 15  "fmt_"
' 10 16 - 19  Length of FORMAT Chunk  Always 0x10
' 14 20 - 21  Audio Format            Always 0x01
' 16 22 - 23  Channels                1 = Mono, 2 = Stereo
' 18 24 - 27  Sample Rate             In Hertz
' 1C 28 - 31  Bytes per Second        Sample Rate * Channels * Bits per Sample / 8
' 20 32 - 33  Bytes per Sample        Channels * Bits per Sample / 8
'                                       1 = 8 bit Mono
'                                       2 = 8 bit Stereo or 16 bit Mono
'                                       4 = 16 bit Stereo
' 22 34 - 35  Bits per Sample
'
' DATA Chunk
' 24 36 - 39  "data"
' 28 40 - 43  Length of Data          Samples * Channels * Bits per Sample / 8
' 2C 44 - End Data Samples
'              8 Bit = 0 to 255             unsigned bytes
'             16 Bit = -32,768 to 32,767    2's-complement signed integers
'-----------------------------------------------------------------------------------
*/

#define WAV_HEADER_LENGTH 44


int save_wav(char fnam[])
{
        FILE *handle;
        FILE *handle2;
        unsigned int leng,size_written,size_read,ret;
        char t_head[WAV_HEADER_LENGTH];
        
        //Build Header Info
        t_head[0]='R';t_head[1]='I';t_head[2]='F';t_head[3]='F';
        leng=WAV_HEADER_LENGTH+raw_position-8;
//      t_head[4]=leng>>(3*8);
//      t_head[5]=leng>>(2*8);
//      t_head[6]=leng>>8;
//      t_head[7]=leng;
        t_head[4]=leng;
        t_head[5]=leng>>8;
        t_head[6]=leng>>(2*8);
        t_head[7]=leng>>(3*8);

        t_head[8]='W';t_head[9]='A';t_head[10]='V';t_head[11]='E';
        t_head[12]='f';t_head[13]='m';t_head[14]='t';t_head[15]=' ';

        t_head[16]=0x10;//leng of format chunk always 0x10
        t_head[17]=0;
        t_head[18]=0;
        t_head[19]=0;
        
        t_head[20]=0x01;//channels
        t_head[21]=0;
        t_head[22]=0x01;//audio format always 0x01
        t_head[23]=0;


        t_head[24]=raw_sample_rate; //0x22;//22050 hertz
        t_head[25]=raw_sample_rate>>8; //0x56;
        t_head[26]=raw_sample_rate>>(2*8);
        t_head[27]=0;

        t_head[28]=raw_sample_rate; // 0x22;//bytes per sec
        t_head[29]=raw_sample_rate>>8; // 0x56;
        t_head[30]=raw_sample_rate>>(2*8);
        t_head[31]=0;

        t_head[32]=0x01;//bytes per sample
        t_head[33]=0;

        t_head[34]=0x08;//bits per sample
        t_head[35]=0;

        t_head[36]='d';t_head[37]='a';t_head[38]='t';t_head[39]='a';

        t_head[40]=raw_position;
        t_head[41]=raw_position>>8;
        t_head[42]=raw_position>>(2*8);
        t_head[43]=raw_position>>(3*8);

        /* open a file for output                 */
        handle2 = fopen( fnam,"wb");
        if( handle2 != NULL )
        {
                size_written = fwrite(t_head,1,WAV_HEADER_LENGTH,handle2 );
                size_written = fwrite(raw_tape,1,raw_position, handle2 );
                fclose( handle2 );
        }
        else
        {
                                gui_error(":( Couldn't Open output file");
                return(1);
        }
        return(0);
}


int load_wav(char fnam[])
{
        FILE * handle,handle2;
        unsigned int wav_length,f;
        unsigned int leng,size_written,size_read,ret;
        char lbl[200];

        handle=fopen( fnam, "rb" );
        if( handle != NULL )
        {
          size_read = fread(raw_tape,1,raw_buflen,handle);
          fclose( handle );
          if( size_read == -1 )
          {
                                gui_error(":( Couldn't Read the Input file");
                                return(1);
          }
        }
        else
        {
                                gui_error(":( Couldn't Read the Input file");
                return(1);
        }

        raw_position=0;

//sprintf(lbl,"Size Read was %d",size_read);
//gui_error(lbl);


        if(size_read==raw_buflen)
                                gui_error("WAV file too big");

        //raw_tape[5]=0;
        //sprintf(lbl,"Header %s",raw_tape);
        //MessageBox(NULL,lbl,"PALE ",MB_YESNOCANCEL | MB_DEFBUTTON1);
        
        if(raw_tape[0]!='R')
        {
                                gui_error(":( NOT A standard RIFF WAV file");
                return(1);
        }
        //Build Header Info
        raw_samples=((int)raw_tape[43]<<(3*8))+(int)raw_tape[42]<<(2*8)+((int)raw_tape[41]<<8)+((int)raw_tape[40]);

//sprintf(lbl,"T43 = %x 42=%x  41=%x 40=%x Samples was %d",raw_tape[43],raw_tape[42],raw_tape[41],raw_tape[40],raw_samples);
//gui_error(lbl);


//sprintf(lbl,"T43 = %x 42=%x  41=%x  ",raw_tape[43],raw_tape[42],raw_tape[41]);
//ret=MessageBox(NULL,lbl,"PALE Load WAV",MB_ICONERROR | MB_OK);

        if(raw_tape[20]!=0x01)
        {
                                gui_error("This looks like stereo, I expected MONO");
                                return(1);
        }

        raw_sample_rate=(raw_tape[26]<<(2*8))+(raw_tape[25]<<8)+raw_tape[24];


//sprintf(lbl,"T26 = %x 25=%x  24=%x  ",raw_tape[26],raw_tape[25],raw_tape[24]);
//ret=MessageBox(NULL,lbl,"PALE Load WAV",MB_ICONERROR | MB_OK);

//sprintf(lbl,"IN DLL Sample Rate = %x",raw_sample_rate);
//ret=MessageBox(NULL,lbl,"PALE Load WAV",MB_ICONERROR | MB_OK);

        if(raw_tape[36]!='d')
        {
                                gui_error(":( Couldn't find the WAV data segment");
                return(1);
        }

        //now copy the data down into the first bit
        for(f=0;f<raw_samples;f++)
                raw_tape[f]=raw_tape[f+44];


        return(raw_samples);
}




void stop_tape()
{
        tape_operation=TAPE_STOPPED;
        raw_motor=0;
        tape_level=0;
        update_tape_gui();
}


void start_tape_play()
{
   tape_operation=TAPE_PLAY;
   tape_spd=TAPE_CYCS_PLAY_128/(raw_sample_rate/BASE_SAMPLE_RATE);
   raw_motor=1;
   update_tape_gui();
}

void start_tape_record()
{
char lbl[200];

        tape_operation=TAPE_RECORD;
        tape_spd=TAPE_CYCS_RECORD_128/(raw_sample_rate/BASE_SAMPLE_RATE);
//sprintf(lbl,"tape_spd is %u rawsamprate is %u",tape_spd,raw_sample_rate);
//MessageBox(NULL,lbl,"PALE ",MB_YESNOCANCEL | MB_DEFBUTTON1);
        raw_motor=1;
        update_tape_gui();
}

void force_raw_play()
{
        tape_override=FORCE_PLAY;//force start
        tape_spd=TAPE_CYCS_PLAY_128/(raw_sample_rate/BASE_SAMPLE_RATE);
        update_tape_gui();
}

void force_raw_auto()
{
        tape_override=FORCE_OFF;
        update_tape_gui();
}

void  force_raw_rec()
{
        tape_spd=TAPE_CYCS_RECORD_128/(raw_sample_rate/BASE_SAMPLE_RATE);
        tape_override=FORCE_RECORD;
        tape_operation=TAPE_RECORD;
        update_tape_gui();
}

void force_raw_stop()
{
        tape_override=FORCE_STOP;//force stop
        tape_level=0;
        update_tape_gui();
}

void rewind_raw()
{
        raw_position=0;
        update_tape_gui();
}

unsigned int  get_raw_position()
{
        return(raw_position);
}

unsigned int   get_raw_samples()
{
        return(raw_samples);
}

int   get_raw_motor()
{
        return(raw_motor);
}

 int  get_tape_level()
{
        return(tape_level);
}


void raw_clear()
{
        int f;
        for(f=0;f<raw_buflen;f++)
                raw_tape[f]=128;
        raw_position=0;
}

void tape_clear()
{
   raw_clear();
   update_tape_gui();
   return;
}

unsigned int get_tapecyc()
{
   return (tapecyc);
}

void set_tapecyc(unsigned int ii)
{
  tapecyc=ii;
}

int get_tape_icount()
{
  return(tapecyc);
}

int set_tape_spd(int gt)
{
  //Note this is the speed threshold dev stuff
  tape_spd=gt;
  return(0);
}

void set_tape_spd_adjust(int gt)
{
  //Note this is the tweakable speed adjustment
  tape_spd_adjust=gt-50;
  return;
}

int set_tape_inverted(int gt)
{
   tape_inverted=gt;
   return(0);
}

int get_raw_threshold(int gt)
{
   return(raw_threshold);
}

void  set_raw_thresh(int gt)
{
   raw_threshold=gt;
   return;
}

int set_raw_position(unsigned int gt)
{
   raw_position=gt;
   return(0);
}

int set_raw_samprate(unsigned int gt)
{
   raw_sample_rate=gt;
   if ((raw_sample_rate<BASE_SAMPLE_RATE) || (raw_sample_rate>100000))     
   {       
      gui_error("Invalid Sample Rate");
      raw_sample_rate=BASE_SAMPLE_RATE;
   }
   return(0);
}

int  get_raw_samprate(int gt)
{
   return(raw_sample_rate);
}

int get_raw_samprate_index(int gt)
{
   return((raw_sample_rate/BASE_SAMPLE_RATE)-1);
}


int set_rawbuflen(unsigned int gt)
{

        char lbl[200];

        raw_tape=(unsigned char *)malloc(gt*1024000);

        if(raw_tape==NULL)
        {
                gui_error("Couldn't Allocate enough Memory for the TAPE buffer");
                raw_tape=(unsigned char *)malloc(1000000);
                if(raw_tape==NULL)
                {
                        gui_error("Couldn't Allocate enough Memory for ANY tape buffer :(");
                }
                else
                        raw_buflen=1000000;
        }
        raw_buflen=gt*1024000;

//      sprintf(lbl,"Allocated tape buffer - %x",raw_buflen);
//      MessageBox(NULL,lbl,"PALE ",MB_OK );

        return(0);
}

int set_raw_buflen(int gt)
{
   if(tape_operation!=TAPE_STOPPED || tape_override==FORCE_PLAY ||  tape_override==FORCE_RECORD )
   {
      gui_error("STOP the tape before changing Buffer Length");
      return(0);
   }
   free(raw_tape);
   set_rawbuflen(gt);
   return(0);
}

int get_tape_monitor(unsigned char tapemon[],unsigned int gt)
{
  int f;
  if(raw_position>gt)
  {
    for(f=0;f<gt;f++)
    {
       //this little calc makes it return the last N points
       //hence this monitor works on SAVEing proggies too
       tapemon[f]=raw_tape[raw_position+f-gt];
    }
  }
  return(1);
}



void init_tapestuff()
{
  set_raw_buflen(BASE_RAW_BUFLEN);
}


void update_tape()
{
        int i;
        static long time_last=0;
        static long time_this=0;
        static long time_diff;
        static long time_last2=0;
        static long time_this2=0;
        static long time_diff2;


        if (tape_mode>=RAW_TAPES && tape_override!=FORCE_STOP)
        {       
                //TAPE RECORD
                if(tape_operation==TAPE_RECORD  || tape_override==FORCE_RECORD)
                {
                        time_this2=z80_get_cycles_elapsed();    //returns noof cycles elaps since last call to z80_emulate
                        if(time_this2<time_last2)//z80_emuate intervened so its not a diff
                                time_diff2=time_this2;  //BUT THIS PROBABLY THROWS AWAY CYCLES HERE
                        else
                                time_diff2=(time_this2-time_last2);
                        time_last2=time_this2;                          
//                      if(time_diff2>1) //tape_spd)   //SAVE AS FAST AS POSSIBLE
                        {
                                //synthesise the output waveform from measuring the number of elapsed z80 cycles
                                //since the last time this routine was called
                                //Get the no of cycles that have passed
                                for(i=0;i<2;i++)                                            // OVERRSTUFF THE SAMPLES
                                {
 if (hw_type==LYNX_HARDWARE_128 || LYNX_HARDWARE_192 || hw_type==LYNX_HARDWARE_256)
                                        {
                                                //128 uses values 19h and 27h 31d & 37d output to port 84 as straight two levels
                                                raw_tape[raw_position]=128+((signed int)(sound_port-32)*8);
                                        }
                                        //if(tape_inverted==1)
                                        //      raw_tape[raw_position]=255-raw_tape[raw_position];//Inverts Output Signal As well
                                        raw_position++;
                                        if (raw_position>=raw_buflen)
                                                raw_position=raw_buflen-1;
                                }
                        }
                }
                //TAPE INPUT
                if(tape_operation==TAPE_PLAY || tape_override==FORCE_PLAY)
                {
                                time_this=z80_get_cycles_elapsed();     //returns noof cycles elaps since last call to z80_emulate
                                if(time_this<time_last)//z80_emuate intervened so its not a diff
                                        time_diff=time_this;    //BUT THIS PROBABLY THROWS AWAY CYCLES HERE
                                else
                                        time_diff=(time_this-time_last);
                                time_last=time_this;                            

                                if(time_diff>(tape_spd+tape_spd_adjust))
                                {       
                                        if(tape_inverted==1)
                                                tape_level=(255-raw_tape[raw_position]);        //inverts input signal
                                        else
                                                tape_level=raw_tape[raw_position];
                                        raw_position++;
                                        if (raw_position>=raw_buflen)
                                                raw_position=raw_buflen-1;
                                }
                }
                update_tape_gui();
        }
}
