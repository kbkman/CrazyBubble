#ifndef MIDI
#define MIDI

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN  

#include <windows.h>
#include <windowsx.h> 
#include <mmsystem.h>
#include <objbase.h>
#include <iostream>
#include <conio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <math.h>
#include <io.h>
#include <fcntl.h>
#include <direct.h>
#include <wchar.h>

#include <dmksctrl.h>
//#include <dmusici.h>
#include <dmusicc.h>
#include <dmusics.h>
#include <dsound.h>
//#include <dmusicf.h>

#define DM_NUM_SEGMENTS 64

#define MIDI_NULL     0   // this midi object is not loaded
#define MIDI_LOADED   1   // this midi object is loaded
#define MIDI_PLAYING  2   // this midi object is loaded and playing
#define MIDI_STOPPED  3   // this midi object is loaded, but stopped



#define DSVOLUME_TO_DB(volume) ((DWORD)(-30*(100 - volume)))

#define MULTI_TO_WIDE( x,y )  MultiByteToWideChar( CP_ACP,MB_PRECOMPOSED, y,-1,x,_MAX_PATH);

#define DD_INIT_STRUCT(ddstruct) { memset(&ddstruct,0,sizeof(ddstruct)); ddstruct.dwSize=sizeof(ddstruct); }



typedef struct DMUSIC_MIDI_TYP
{
IDirectMusicSegment        *dm_segment;  // the directmusic segment
IDirectMusicSegmentState   *dm_segstate; // the state of the segment
int                        id;           // the id of this segment               
int                        state;        // state of midi song

} DMUSIC_MIDI, *DMUSIC_MIDI_PTR;


int DMusic_Load_MIDI(const char *filename);
int DMusic_Play(int id);
int DMusic_Stop(int id);
int DMusic_Shutdown(void);
int DMusic_Delete_MIDI(int id);
int DMusic_Delete_All_MIDI(void);
int DMusic_Status_MIDI(int id);
int DMusic_Init(HWND hWnd);

#endif


