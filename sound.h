/*
 * sound.h - Platform Independant Sound Support - Dec. 1994
 *
 * Copyright 1994 Sujal M. Patel (smpatel@wam.umd.edu)
 * Conditions in "copyright.h"          
 */


#define SND_EXPLOSION 0
#define SND_FIRETORP  1
#define SND_SHIELD    2
#define SND_TORPHIT   3
#define SND_EXP_SB    4
#define SND_DDLOO     5
#define SND_WARP      6
#define SND_SMART     7
#define NUM_SOUNDS    8

#define MAX_SOUNDS 16

void init_sound (word userId);             /* Init Sound System                          */
void play_sound (int k);        /* Play a Sound                               */
void maybe_play_sound (int k);  /* Play sound if the last 'k' sound_completed */
void sound_completed (int k);   /* Complete a sound 'k'                       */
void kill_sound (void);             /* Terminate a sound unpredictably :)         */

