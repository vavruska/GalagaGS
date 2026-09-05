/*
 * sound.c - Platform Independant Sound Support - Apr. 1995
 *
 * Copyright 1994-1995 Sujal M. Patel (smpatel@wam.umd.edu)
 * Conditions in "copyright.h"          
 */
#include <types.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <sound.h>
#include <misctool.h>
#include <resources.h>
#include <loader.h>
#include <gsos.h>
#include <orca.h>
#include "sound.h"
#include "data.h"
#include "defs.h"

#define TOOLFAIL(string) if (toolerror()) SysFailMgr(toolerror(), "\p" string "\n\r    Error Code -> $");

static SoundParamBlock soundBlock[NUM_SOUNDS][2];
static Handle dp;
static Handle sh[MAX_SOUNDS];
#ifndef USE_RESOURCES
static char *soundFiles[] = {
    "explode.wav",
    "firetorp.wav",
    "shield.wav",
    "torphit.wav",
    "explodeBig.wav",
    "ddloo.wav",
    "warp.wav",
    "smart.wav",
};
#endif

#ifdef USE_RESOURCES
static Handle loadSound(long id) {
    Handle sh;
    
    sh = LoadResource(rSoundSample, id);
    TOOLFAIL("Loading SoundResource")
    DetachResource(rSoundSample, id);
    return sh;
}
#else
static Handle loadSound(char *filename, word userId) {
    GSString255 src;
    OpenRecGS openRec;
    IORecGS readRec;
    RefNumRecGS closeRec;
    Handle sh;
    ResultBuf255 optionList;

    src.length = sprintf(src.text, "9:sounds:%s", filename);
    openRec.pCount = 12;
    openRec.pathname = &src;
    openRec.requestAccess = readEnable;
    openRec.resourceNumber = 0;
    openRec.optionList = &optionList;

    OpenGS(&openRec);
    if (toolerror()) {
        if (toolerror() == fileNotFound) {
            TOOLFAIL("Sound File Not Found");
        }
        TOOLFAIL("Sound file open error");
    }

    sh = NewHandle(openRec.eof, userId, attrLocked, 0);
    readRec.pCount = 4;
    readRec.refNum = openRec.refNum;
    readRec.dataBuffer = *sh;
    readRec.requestCount = openRec.eof;
    ReadGS(&readRec);
    if (toolerror()) {
        TOOLFAIL("sound file read error");
    }

    closeRec.pCount = 1;
    closeRec.refNum = openRec.refNum;
    CloseGS(&closeRec);

    return sh;
}
#endif

void init_sound (word userId) {
    int i;
    long size;
    word blockSize, blockCount;
    pointer sb;
    extern SoundParamBlock soundBlock[NUM_SOUNDS][2];

    dp = NewHandle(0x100L, userId, attrBank + attrPage + attrFixed + attrLocked + attrNoCross, 0);
    SoundStartUp((Word) (*dp));

    for (i = 0; i < NUM_SOUNDS; i++) {
#ifdef USE_RESOURCES
        sh[i] = loadSound(i+1);
#else
        sh[i] = loadSound(soundFiles[i], userId);
#endif
        if (sh[i] != NULL) {
            sb = *sh[i];
            sb += 40;
            size = ((long)sb[3]) <<24L;
            size |=((long)sb[2]) <<16L;
            size |=((long)sb[1]) <<8L;
            size |=(long)sb[0];
            sb += 4;
            blockCount = 0;

            while (size) {
                blockSize = size > 32768 ? 32768 : size;
                soundBlock[i][blockCount].waveStart = sb;
                soundBlock[i][blockCount].waveSize = (blockSize + 255) / 256;
                soundBlock[i][blockCount].freqOffset = 32.0 * 22050.0 / 1645.0;
                soundBlock[i][blockCount].docBuffer = 0;
                soundBlock[i][blockCount].bufferSize = 0;
                soundBlock[i][blockCount].nextWavePtr = size > 32768L ? &soundBlock[i][blockCount + 1] : NULL;
                soundBlock[i][blockCount].volSetting = 250;
                size -= blockSize;
                sb += blockSize;
                blockCount++;
            }
        }
    }
} 

void play_sound (int k) {
    int count = 0;
    word gen;

    if (playSounds) {
        do {
            gen = 0x0001 | (k << 8);
            if (FFSoundDoneStatus(k)) {
                FFStopSound(1 << k);
            }
            FFStartSound(gen, (Pointer) &soundBlock[k][0]);
            count++;
            
        } while (toolerror() == genBusyErr && count < 14);
    }
}

void kill_sound (void) { 
    int i;

    FFStopSound(0x7fff);
    SoundShutDown();
    for (i = 0; i < NUM_SOUNDS; i++) {
        DisposeHandle(sh[i]);
    }
    DisposeHandle(dp);
}

