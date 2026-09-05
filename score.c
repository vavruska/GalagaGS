/* $Id: score.c,v 1.2 1998/04/30 05:11:58 mrogre Exp $ */
/* Copyright (c) 1998 Joe Rumsey (mrogre@mediaone.net) */
#include "copyright.h"

#include <stdio.h>
#include <string.h>
#include "struct.h"
#include "images.h"
#include "data.h"
#include "defs.h"
#include "proto.h"

#include "gte.h"

char scorestr[40] = "Score: 0000000", shipstr[4] = "";
W_Image *miniShip, *extraImage;
int drawExtra = 0, extrax, extray;

void do_score(void) {
    static int lastscore, lastlevel, lastships;
    static int lastsounds;
    static int lastMouseControl;

    if (lastscore != score) {
        if ((score > 0) && (score >= nextBonus)) {
            ships++;
            extrax = 2;
            extray = WINHEIGHT / 16;
            drawExtra = 1;
            if (nextBonus < BONUSSHIPSCORE) {
                nextBonus = BONUSSHIPSCORE;
            } else {
                nextBonus += BONUSSHIPSCORE;
            }
        }
    }

    if (lastscore != score || lastlevel != level || lastships != ships || lastsounds != playSounds || mouseControl != lastMouseControl) {
        draw_score();
        lastscore = score;
        lastlevel = level;
        lastships = ships;
        lastsounds = playSounds;
        lastMouseControl = mouseControl;
    }

    if (drawExtra) {
        extrax += 1;
        if (extrax > 40 - extraImage->width) {
            drawExtra = 0;
            for (int i = -1; i < extraImage->width; i++) {
                GTESetTile(extrax + i, extray, 0);
                GTESetTile(extrax + i, extray+1, 0);
            }
        } else {
            GTESetTile(extrax - 1, extray, 0);
            GTESetTile(extrax - 1, extray+1, 0);
            for (int i = 0; i < extraImage->width; i++) {
                GTESetTile(extrax + i, extray, extraImage->tileStart + i);
                GTESetTile(extrax + i, extray+1, extraImage->tileStart + i + 32);
            }
        }
    }
}

void draw_score(void) {
    int basex;
    int i, c;

    sprintf(scorestr, "Score: %07ld   Level: %02d", score, level);
    GTESetTile(39, 0, 21 + (playSounds != 0)  | TILE_PRIORITY_BIT);

    if (mouseControl) {
        GTESetTile(38, 0, 53);
    } else {
        GTESetTile(38, 0, 54);
    }

    basex = (WINWIDTH / 7) - strlen(scorestr) / 2;
    for (i = 0; i < strlen(scorestr); i++) {
        c = scorestr[i];
        if (c >= 32) {
            c = c + 33;
            GTESetTile(basex + i, 0, c | TILE_PRIORITY_BIT);
        }
    }

    for (i = 0; i < 6; i++) {
        if (i < ships - 1) {
            GTESetTile(i, 0, miniShip->tileStart | TILE_PRIORITY_BIT);
        } else {
            GTESetTile(i, 0, 0);
        }
    }

}

void init_score(void) {
    miniShip = getImage(I_MINISHIP);
    extraImage = getImage(I_EXTRA);
}


