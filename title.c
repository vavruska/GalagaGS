/* $Id: title.c,v 1.4 1998/05/11 06:52:59 mrogre Exp $ */
/* Copyright (c) 1998 Joe Rumsey (mrogre@mediaone.net) */
#include "copyright.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <math.h>
#include "defs.h"
#include "images.h"
#include "struct.h"
#include "data.h"
#include "proto.h"
#include "pathfile.h"

#include "gte.h"

extern Byte tiles[];

W_Image *pauseImage, *titleImage;
int pausex, pausey, pauseyspeed = 1;
static int line = -1;

void undo_pause(void) {
    int i,j;
    W_Image *pause = getImage(I_PAUSE);

    for (i = 0; i < pause->height; i++) {
        for (j = 0; j < pause->width; j++) {
            GTESetTile(j+8, i+10, 0);
        }
    }
}

void do_pause(void) {
    int i,j;
    int t;
    W_Image *pause = getImage(I_PAUSE);

    for (i = 0; i < pause->height; i++) {
        for (j = 0; j < pause->width; j++) {
            t = pause->tileStart + i * 32 + j;
            GTESetTile(j+8, i+10, t);
        }
    }
}

void center_text(char *text, int y) {
    int x;
    int p = 0;
    int c;
    
    x = 20 - strlen(text) / 2;
    for (p = 0; p < x;p++) {
        GTESetTile(p, y, 65 | TILE_PRIORITY_BIT);
    }
    for (p = 0; p < strlen(text); p++) {
        if (text[p]>31 && text[p]<127) {
            c = text[p] + 225;
            GTESetTile(x++, y, c | TILE_PRIORITY_BIT);
        }
    }
    for (p = x; p < 40;p++) {
        GTESetTile(p, y, 65 | TILE_PRIORITY_BIT);
    }
}

void draw_text(char *text, int x, int y) {
    int p = 0;
    int c;
    
    for (p = 0; p < strlen(text); p++) {
        if (text[p]>31 && text[p]<127) {
            c = text[p] + 225;
            GTESetTile(x++, y, c | TILE_PRIORITY_BIT);
        }
    }
}

void clearScreen(int start, int end) {
    int x,y;
    for (y = start; y <= end ; y++) {
        for (x = 0; x < 40;x++) {
            GTESetTile(x, y, 65 | TILE_PRIORITY_BIT);
        }
    }
}

static void show_points(void) {
    int i;
    static int a_images[6] = {
        367,
        369,
        371,
        373,
        375,
        377
    };
    int y = line + 7;
    int t;

    char buf[40];

    switch (y) {
    case 7:
    case 9:
    case 11:
    case 13:
    case 15:
    case 17:
        i = line / 2;
        t = a_images[5-i];
        clearScreen(y, y+1);
        GTESetTile(13, y, t| TILE_PRIORITY_BIT);
        GTESetTile(14, y, (t + 1)| TILE_PRIORITY_BIT);
        GTESetTile(13, y + 1, (t + 32)| TILE_PRIORITY_BIT);
        GTESetTile(14, y + 1, (t + 33)| TILE_PRIORITY_BIT);
        if (i < 5) {
            sprintf(buf, "- %d", (i + 1) * 100);
        } else {
            sprintf(buf, "- ???");
        }
        draw_text(buf, 25, y);
        line += 2;
        break;
    case 19:
        center_text("Ships in convoy are worth 50 points", y + 1);
        line++;
        break;
    case 20:
        clearScreen(y + 1, y + 2);
        line++;
        break;
    case 21:
        center_text("Bonus ships at 20,000, 50,000,", y + 1);
        line++;
        break;
    case 22:
        center_text("then every 50,000.", y + 1);
        line++;
        break;
    case 23:
        line = -1;
        break;
    }
}

static void show_help(void) {
    int y = line + 7;

    switch (y) {
    case 7:
        center_text("         Keyboard controls           ", y);
        break;
    case 8:
        center_text("", y);
        break;
    case 9:
        center_text("  left/right arrow   move            ", y);
        break;
    case 10:
        center_text("  space bar          fire            ", y);
        break;
    case 11:
        center_text("  p                  pause           ", y);
        break;
    case 12:
        center_text("  q                  end game        ", y);
        break;
    case 13:
        center_text("  k                  keyboard control", y);
        break;
    case 14:
        center_text("  m                  mouse control   ", y);
        break;
    case 15:
        center_text("  s                  toggle sound    ", y);
        break;
    case 16:
        center_text("  Q                  exit game       ", y);
        break;
    case 17:
        clearScreen(17, 20);
        break;
    case 18:
        center_text("Press space to start, or Q to quit", 21);
        break;
    case 19:
        line = -1;
    }

    if (line != -1) {
        line++;;
    }
}

void show_logo(void) {
    int tx,ty;
    int x = 13, y = 2;
    W_Image *title = getImage(I_TITLE);
    for (ty = 0; ty < title->height; ty++) {
        for (tx = 0; tx < title->width; tx++) {
            GTESetTile(x++, y, title->tileStart + (ty * 32) + tx | TILE_PRIORITY_BIT);
        }
        x = 13; 
        y++;
    }
}

void do_title(void) {
    static int lastPage = -1;
    extern int line, title_page, pagetimer;
    if (lastPage != title_page) { 
        lastPage = title_page;
        line = 0;
        clearScreen(7, 23);
    } 

    if (getting_name) {
        title_page = 4;
    }
    if (line != -1) {
        switch (title_page) {
        case 0:
            //set up some of the tiles for demo mode.
            GTELoadTileSet(352, 512, (tiles + (512L * 128L)));
            GTELoadTileSet(256, 353, (tiles + (672L * 128L)));
            show_logo();
            title_page = 1;
            break;
        case 1:
            show_scores(&line);
            center_text("Bonus ships at 20,000, 50,000,", 20);
            center_text("then every 50,000.", 21);
            break;
#if 0
        case 2:
            show_points();
            break;
#endif
        case 4:
            GTELoadTileSet(352, 512, (tiles + (512L * 128L)));
            GTELoadTileSet(256, 353, (tiles + (672L * 128L)));
            show_logo();
            title_page = 3;
            pagetimer = 299;
            break;
        case 3:
        default:
            show_help();
            break;
        }
        center_text("Copyright (c) 2026 Chris Vavruska", 24);
    }


    if (line == -1) {
        pagetimer--;
        if (!pagetimer) {
            title_page++;
            if (title_page > 3) {
                title_page = 1;
            }
            pagetimer = 299;
        }
    }
}



