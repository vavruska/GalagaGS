/* Copyright (c) 1995-1998 Joe Rumsey */
#include "copyright.h"
/*
 * warp.c - Hyperspace starfield effect.
 *
 * When the player beats a level the game will show a hyperspace burst by
 * writing white stars directly into the SHR screen memory as they radiate
 * outward from the middle of the playfield.
 *
 * The stars launch in batches so the burst ripples outward.  Every
 * WARP_BATCH_DELAY_loops another batch of WARP_BATCH stars is launched from
 * the middle.  Each star travels outward until it reaches the edge of its
 * band, lingers for a moment, and then erases itself.
 *
 * The stars never travel above line 8 (the top of the playfield) or below
 * the line where the player's ship starts (PLAYER_Y), so they never cover
 * the ship.
 *
 * NOTE: This module is complete but NOT wired into the game loop yet.
 *       Nothing here is called from main.c.
 */

#include <types.h>
#include <stdlib.h>

#include "defs.h"
#include "data.h"

#define WARP_SHR1 ((Byte *) 0xE12000L)
#define WARP_LINE_BYTES 160

#define WARP_STARS 60
#define WARP_BATCH 5          /* stars launched at once */
#define WARP_BATCH_DELAY 6    /* loops between batch launches */
#define WARP_EDGE_LINGER 4     /* loops a star holds at the edge */

#define WARP_CENTER_X (WINWIDTH / 2)
#define WARP_CENTER_Y (WINHEIGHT / 2)

#define WARP_Y_MIN 8
#define WARP_Y_MAX (PLAYER_Y)

#define WARP_STAR_VALUE 0xEE
#define WARP_BLANK_VALUE 0x00

static int warpBatchDelay = WARP_BATCH_DELAY;

static struct {
    int x, y;   /* current position (byte column, scan line) */
    int ox, oy; /* position last drawn on screen; -1 if nothing */
    int dx, dy; /* movement per frame */
    int active; /* launched and still on screen */
    int edge;   /* reached the edge of its band, holding */
    int linger; /* loops left to hold at the edge */
    int done;   /* star finished its whole life and erased itself */
} warpStars[WARP_STARS];

static int warpFrames;

/* 16 evenly spaced radiating directions, in bytes per frame per speed
 * unit. */
static int warpDirs[16][2] = {
    { 0, -2 },  { 1, -2 }, { 1, -1 }, { 2, -1 },
    { 2, 0 },   { 2, 1 },  { 1, 1 },  { 1, 2 },
    { 0, 2 },   { -1, 2 }, { -1, 1 }, { -2, 1 },
    { -2, 0 },  { -2, -1 }, { -1, -1 }, { -1, -2 }
};

static void warpPutPixel(int x, int y, Byte value) {
    WARP_SHR1[y * WARP_LINE_BYTES + x] = value;
}

void init_warp(void) {
    int i;

    for (i = 0; i < WARP_STARS; i++) {
        if (warpStars[i].ox >= 0 && warpStars[i].oy >= 0) {
            warpPutPixel(warpStars[i].ox, warpStars[i].oy, WARP_BLANK_VALUE);
        }
        warpStars[i].active = 0;
        warpStars[i].edge = 0;
        warpStars[i].linger = 0;
        warpStars[i].done = 0;
        warpStars[i].ox = -1;
        warpStars[i].oy = -1;
    }
    warpFrames = 0;
}

static void warpLaunch(int first, int count) {
    int i;
    int sp;
    int d;

    for (i = first; i < first + count; i++) {
        warpStars[i].x = WARP_CENTER_X + (random() % 9) - 4;
        warpStars[i].y = WARP_CENTER_Y + (random() % 13) - 6;
        warpStars[i].ox = warpStars[i].x;
        warpStars[i].oy = warpStars[i].y;

        /* Aim the star at one of 16 evenly spaced points around the
         * screen */
        d = random() % 16;
        sp = 1 + (random() % 3);
        warpStars[i].dx = warpDirs[d][0] * sp;
        warpStars[i].dy = warpDirs[d][1] * sp;

        warpStars[i].active = 1;
        warpStars[i].edge = 0;
        warpStars[i].linger = 0;
        warpStars[i].done = 0;

        warpPutPixel(warpStars[i].x, warpStars[i].y, WARP_STAR_VALUE);
    }
}

int do_warp(void) {
    int i, j;
    int skip;
    int doneCount = 0;

    if ((warpFrames % warpBatchDelay) == 0 && warpBatchDelay > 0) {
        int b = warpFrames / warpBatchDelay;
        if (b * WARP_BATCH < WARP_STARS) {
            warpLaunch(b * WARP_BATCH, WARP_BATCH);
        }
    }
    warpFrames++;

    /* Advance the stars.  A star that reaches the edge of its band holds
     * still for a few loops, then erases itself. */
    for (i = 0; i < WARP_STARS; i++) {
        int tx = warpStars[i].x + warpStars[i].dx;
        int ty = warpStars[i].y + warpStars[i].dy;
        if (!warpStars[i].active) {
            continue;
        }
        if (warpStars[i].edge) {
            if (warpStars[i].linger > 0) {
                warpStars[i].linger--;
            } else {
                /* The last thing a star does is erase itself. */
                warpStars[i].active = 0;
                warpStars[i].done = 1;
            }
            continue;
        }
        if (tx < 0 || tx >= WINWIDTH ||
            ty < WARP_Y_MIN || ty > WARP_Y_MAX) {
            warpStars[i].edge = 1;
            warpStars[i].linger = WARP_EDGE_LINGER;
        } else {
            warpStars[i].x = tx;
            warpStars[i].y = ty;
        }
    }

    /* Erase last frame's pixels, unless a star is drawn there this frame,
     * so we never blink a star out that was just drawn. */
    for (i = 0; i < WARP_STARS; i++) {
        if (warpStars[i].ox < 0 || warpStars[i].oy < 0) {
            continue;
        }
        skip = 0;
        for (j = 0; j < WARP_STARS; j++) {
            if (warpStars[j].active &&
                warpStars[j].x == warpStars[i].ox &&
                warpStars[j].y == warpStars[i].oy) {
                skip = 1;
                break;
            }
        }
        if (!skip) {
            warpPutPixel(warpStars[i].ox, warpStars[i].oy, WARP_BLANK_VALUE);
            warpStars[i].ox = -1;
            warpStars[i].oy = -1;
        }
    }

    /* Draw the surviving stars and remember what is on screen. */
    for (i = 0; i < WARP_STARS; i++) {
        if (!warpStars[i].active) {
            continue;
        }
        warpPutPixel(warpStars[i].x, warpStars[i].y, WARP_STAR_VALUE);
        warpStars[i].ox = warpStars[i].x;
        warpStars[i].oy = warpStars[i].y;
    }

    for (i = 0; i < WARP_STARS; i++) {
        if (warpStars[i].done) {
            doneCount++;
        }
    }

    return (doneCount == WARP_STARS);
}
