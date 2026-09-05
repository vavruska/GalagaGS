/* Copyright (c) 1995-1998 Joe Rumsey (mrogre@mediaone.net) */
#include "copyright.h"
/*
 * main.c for XGalaga
 */
#include <types.h>
#include <types.h>
#include <loader.h>
#include <locator.h>
#include <gsos.h>
#include <memory.h>
#include <misctool.h>
#include <orca.h>
#include <resources.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "struct.h"
#include "data.h"
#include "images.h"
#include "defs.h"
#include "paths.h"
#include "pathfile.h"
#include "proto.h"
#include "sound.h"
#include "defs.h"

#include "gte.h"
#include "gteHelp.h"

#include "control.h"


struct torp torps[MAXTORPS];
struct torp *first_etorp = NULL;

static int convoyx = 0, convoymove = 1;
static int convoy_dx = 0;
static int livecount = 0;
static int warpStep = 1;
static int attacking = 0, maxattacking, entering = 0;
static int maxetorps = MAXETORP, numetorps = 0;
static int plflash = 50;
static int shieldspawn = 0;
#ifndef ORIGINAL_XGALAGA
static int shots = 0;
static int hits = 0;
#endif
volatile word frameTick = 0;

#ifdef BENCHMARK
static unsigned int bm_rendered = 0;
static unsigned long bm_renderTotal = 0;
static unsigned int bm_frames = 0, bm_minRender = 0xffff, bm_maxRender = 0;
static FILE *bm_file = NULL;
#endif

#ifdef USE_RESOURCES
static word curApp;
static word fID;
#endif

word userId;

extern Word tilesPalette[16];
extern Byte tiles[];

#define convoy_x_pos(i) (convoyx + (HORT * (i - 10 * (i / 10))))
#define convoy_y_pos(i) (14 + (16*(i/10)))

static int moves[16][2] = {
    { 0, -4 },
    { 1, -4 },
    { 3, -3 },
    { 4, -1 },
    { 4, 0 },
    { 4, 1 },
    { 3, 3 },
    { 1, 4 },
    { 0, 4 },
    { -1, 4 },
    { -3, 3 },
    { -4, 1 },
    { -4, 0 },
    { -4, -1 },
    { -3, -3 },
    { -1, -4 }
};

void
xgal_exit(int v) {
    /*    W_AutoRepeatOn();*/
    /* Destroy our main window so the fullscreen mode gets unset if we're
       running fullscreen. (We should really clean up much more here!) */
    kill_sound();
#ifdef BENCHMARK
    if (bm_file) {
        fclose(bm_file);
    }
#endif
#ifdef USE_RESOURCES
    SetCurResourceApp(curApp);
    CloseResourceFile(fID);
    ResourceShutDown();
#endif
    GTEShutDown();
    MTShutDown();
    MMShutDown(userId);
    TLShutDown();
    /*    sleep(1);*/ /* Without this, the auto-repeat request fails on my machine... */
    /* Note if we ever need this autorepeat thingie again, the sleep can and should be replaced by a call to XSync() */
    exit(v);
}

/*-------------------aliens---------------*/

static void delete_etorps(void) {
    struct torp *tmp;

    while(first_etorp) {
        tmp = first_etorp->next;
        if (first_etorp->alive) {
            removeSprite(&first_etorp->shape);
        }
        free(first_etorp);
        first_etorp = tmp;
    }
    numetorps = 0;
}

static void init_aliens(int level) {
    int i;
    extern int livecount;

    convoyx = 0;
    convoymove = 1;
    undo_prizes();

    maxattacking = 1 + (level * 2);
    if (maxattacking > 30) {
        maxattacking = 30;
    }
    attacking = 0;
    maxetorps = 10 +(level * 5);
    numetorps = 0;

    delete_etorps();
    metaLevel = 1;
    if (read_level(level) <= 0) {
        SysFailMgr(toolerror(), "\pError Reading level file\n\r    Error Code -> $");
        exit(0);
    }
    level_alien_rows(levelAlienRows);
    loadLevelSprites();

    numActiveAlien = 0;
    for (i = 0; i < MAXALIENS; i++) {
        livecount++;
        new_alien(level, i, &aliens[i]);
    }

    for (i = 0; i < MAXTORPS; i++) {
        torps[i].alive = 0;
    }
}

static void undo_aliens(void) {
    int i;

    for (i = 0; i < MAXALIENS;i++) {
        if (aliens[i].valid) {
            hideSprite(&aliens[i].shape);
        }
    }
}

static void do_escort(int i) {
    int fs = aliens[i].escorting;

    if (!aliens[fs].alive) {
        aliens[i].escorting = -1;
    } else if (aliens[fs].dir >= 0) {
        aliens[i].dir = aliens[fs].dir;
    } else {
        int col = i - 10 * (i / 10);
        aliens[i].x = HORT * col + convoyx + convoymove;
        aliens[i].y = -10;
        aliens[i].dir = -2;
        aliens[i].path = -1;
        aliens[i].steer = 2;
        aliens[i].escorting = -1;
    }
}

static void do_convoy(int i) {
    aliens[i].x += convoy_dx;
    if ((entering == 0) &&
        (attacking < maxattacking) &&
        ((livecount < maxattacking) ||
         ((random() % 10000) < (level + 2 * (48 - (livecount)))))) {
        switch (random() % 2) {
        case 0:
            path_dir(P_PEELLEFT, 0, &aliens[i].dir, &aliens[i].steer);
            aliens[i].path = P_PEELLEFT;
            break;
        case 1:
            path_dir(P_PEELRIGHT, 0, &aliens[i].dir, &aliens[i].steer);
            aliens[i].path = P_PEELRIGHT;
            break;
        }
        aliens[i].path_pos = 0;
        attacking++;
        if (i < 10) { /* Flagship, grab escorts */
            int e;
            for (e = i + 9; e < i + 12; e++) {
                if (aliens[e].alive && aliens[e].dir == -1) {
                    aliens[e].escorting = i;
                }
            }
        }
    }
}

static void new_etorp(int x, int y, int xs, int ys) {
    struct torp *t;

    t = malloc(sizeof(struct torp));
    t->next = first_etorp;
    if(t->next)
        t->next->prev = t;
    t->prev = 0;
    first_etorp = t;

    t->x = x;
    t->y = y;
    t->xspeed = xs;
    t->yspeed = ys;
    t->alive = 1;
    t->frame = 0;
    memcpy(&t->shape, getImage(I_ETORP), sizeof(W_Image));
    addSprite(&t->shape, -10, -10);
    numetorps++;
}

static void do_enter(int i) {
    int diffx, diffy;
    int row, col, cx, cy;
    int tc;

    if (aliens[i].enterdelay) {
        aliens[i].enterdelay--;
        if (!aliens[i].enterdelay) {
            replaceSprite(&aliens[i].shape, aliens[i].dir,
                                  aliens[i].x,
                                  aliens[i].y);
        }
        return;
    }

    if (aliens[i].path >= 0) {
        aliens[i].x += moves[aliens[i].dir][0] + (metaLevel - 1) * moves[aliens[i].dir][0] / 2;
        aliens[i].y += moves[aliens[i].dir][1] + (metaLevel - 1) * moves[aliens[i].dir][1] / 2;

        aliens[i].steer--;
        if (aliens[i].steer <= 0) {
            aliens[i].path_pos++;
            enter_path_dir(aliens[i].path, aliens[i].path_pos, &aliens[i].dir, &aliens[i].steer);
            if (metaLevel > 1) aliens[i].steer = aliens[i].steer / (1 + ((metaLevel - 1) * .5));

            if (aliens[i].dir < 0) {
                aliens[i].path = -1;
            }
        }
        tc = TORPCHANCE - level / 2 - weapon * 5;
        if (tc < 35) {
            tc = 35;
        }
        if (numetorps < maxetorps && (!(random() % tc))) {
            int xs, ys;

            /* could aim better, not sure it should! */

            if (aliens[i].x > plx + 120) { 
                xs = -3;
            } else if (aliens[i].x > plx + 60) { 
                xs = -2;
            } else if (aliens[i].x < plx - 120) { 
                xs = 3;
            } else if (aliens[i].x < plx - 6) { 
                xs = 2;
            } else {
                xs = 0;
            }
            ys = (ETORPSPEED + level / 5) - ABS(xs);
            new_etorp(aliens[i].x, aliens[i].y + aliens[i].shape.height / 2 + 2, xs, ys);
        }
    } else {
        //this used to use convoy_x_pos/convoy_y_pos but was trimmed down
        //to not have to do the same math multiple times. Less cycles=faster
        row = i / 10;
        col = i - 10 * row;
        cx = convoyx + HORT * col;
        cy = 14 + 16 * row;
        diffx = ABS(cx - aliens[i].x);
        diffy = ABS(cy - aliens[i].y);
        if (diffy < 4 + (metaLevel * 2)) {
            aliens[i].y = cy;
            if (diffx < 4 + (metaLevel * 2)) {
                aliens[i].x = cx;
                aliens[i].dir = -1;
                aliens[i].entering = 0;
                return;
            }
            aliens[i].dir = 0;
            if (cx > aliens[i].x) {
                aliens[i].dir = 4;
            } else {
                aliens[i].dir = 12;
            }
        } else {
            if (cy < aliens[i].y) {
                if (diffx < 4 + (metaLevel * 2)) {
                    aliens[i].x = cx;
                    aliens[i].dir = 0;
                } else {
                    if (cx > aliens[i].x) {
                        aliens[i].dir = 2;
                    } else {
                        aliens[i].dir = 14;
                    }
                }
            } else {
                if (diffx < 4 + (metaLevel * 2)) {
                    aliens[i].x = cx;
                    aliens[i].dir = 8;
                } else {
                    if (cx > aliens[i].x) {
                        aliens[i].dir = 6;
                    } else {
                        aliens[i].dir = 10;
                    }
                }
            }
        }
        aliens[i].x += moves[aliens[i].dir][0] + (metaLevel - 1) * moves[aliens[i].dir][0] / 2;
        aliens[i].y += moves[aliens[i].dir][1] + (metaLevel - 1) * moves[aliens[i].dir][1] / 2;
    }
}

static void do_aliens(void) {
    int i, j, k;
    int tc;
    int prevDir;
    extern struct alien aliens[];


    if (!paused) {
        if (convoyx <= 5 && convoymove < 0) {
            convoymove = -convoymove;
        } else if (convoyx >= WINWIDTH - 100 && convoymove > 0) {
            convoymove = -convoymove;
        }
        convoy_dx = (counter & 1) ? 0 : 2 * convoymove;
        convoyx += convoy_dx;
    }

    livecount = 0; attacking = 0;
    numAliveAlien = 0;
    for (k = 0, livecount = 0, entering = 0; k < numActiveAlien; k++) {
        int ai = activeAlien[k];
        if (aliens[ai].alive) {
            aliveAlien[numAliveAlien++] = ai;
            livecount++;
            if (aliens[ai].dir >= 0 && aliens[ai].escorting < 0 && !(aliens[ai].entering)) {
                attacking++;
            }
            if (aliens[ai].entering) {
                entering++;
            }
        }
    }

    for (k = 0; k < numActiveAlien; k++) {
        i = activeAlien[k];
        if (aliens[i].alive) {
            int row = i / 10;
            int col = i - 10 * row;
            prevDir = aliens[i].dir;
            if (!paused) {
                if (aliens[i].escorting >= 0) {
                    do_escort(i);
                }
                if (aliens[i].entering) {
                    do_enter(i);
                } else if (aliens[i].dir == -1) {
                    do_convoy(i);
                } else if (aliens[i].dir == -2) {
                    //this used to use convoy_x_pos/convoy_y_pos but was trimmed down
                    //to not have to do the same math multiple times. Less cycles=faster
                    int cy = 14 + 16 * row;
                    aliens[i].x += convoy_dx;
                    aliens[i].y += 2;
                    if (aliens[i].y >= cy) {
                        aliens[i].y = cy;
                        aliens[i].x = convoyx + HORT * col;
                        aliens[i].dir = -1;
                    }
                } else {
                    aliens[i].x += moves[aliens[i].dir][0] / ATTACK_MOVE_SCALE;
                    aliens[i].y += moves[aliens[i].dir][1] / ATTACK_MOVE_SCALE;
                    if (aliens[i].x > WINWIDTH + HORT) {
                        aliens[i].x = -HORT;
                    } else if (aliens[i].x < -HORT) {
                        aliens[i].x = WINWIDTH + HORT;
                    }
                    if (aliens[i].y > WINHEIGHT + (aliens[i].shape.height * 2)) {
                        aliens[i].x = HORT * col + convoyx + convoymove;
                        aliens[i].y = -30;
                        aliens[i].dir = -2;
                        aliens[i].path = -1;
                        aliens[i].steer = 2;
                        aliens[i].escorting = -1;
                        attacking--;
                        if (i < 10) {
                            for (j = i + 9; j < i + 12; j++) {
                                aliens[j].escorting = -1;
                            }
                        }
                    } else if (aliens[i].y < 0) {
                        aliens[i].dir = 8;
                    }

                    if (aliens[i].escorting < 0) {
                        aliens[i].steer--;
                        if (aliens[i].steer <= 0) {
                            if (aliens[i].path >= 0) {
                                int lastdir = aliens[i].dir;

                                aliens[i].path_pos++;
                                path_dir(aliens[i].path, aliens[i].path_pos, &aliens[i].dir, &aliens[i].steer);
                                if (aliens[i].dir < 0) {
                                    aliens[i].dir = lastdir;
                                    do {
                                        switch (random() % 8) {
                                        case 0:
                                            start_path(P_LOOP, &aliens[i]);
                                            break;
                                        case 1:
                                            start_path(P_SWOOP1, &aliens[i]);
                                            break;
                                        case 2:
                                            start_path(P_SWOOP2, &aliens[i]);
                                            break;
                                        case 3:
                                            start_path(P_ZIGZAG, &aliens[i]);
                                            break;
                                        case 4:
                                            start_path(P_LOOP2, &aliens[i]);
                                            break;
                                        case 5:
                                            start_path(P_SPIN, &aliens[i]);
                                            break;
                                        case 6:
                                            start_path(P_LEFTDIAG, &aliens[i]);
                                            break;
                                        case 7:
                                            start_path(P_RIGHTDIAG, &aliens[i]);
                                            break;
                                        default:
                                            aliens[i].steer = TURNSPEED;
                                            aliens[i].path = -1;
                                        }
                                    } while ((aliens[i].path < 0) || (aliens[i].steer < 0));
                                }
                            } else {
                                if (random() % 2) {
                                    aliens[i].dir++;
                                    if (aliens[i].dir > 15) {
                                        aliens[i].dir = 0;
                                    }
                                } else {
                                    aliens[i].dir--;
                                    if (aliens[i].dir < 0) {
                                        aliens[i].dir = 15;
                                    }
                                }
                                aliens[i].steer = TURNSPEED;
                            }
                        }
                    }
                    tc = TORPCHANCE - level / 2 - weapon * 5;
                    if (tc < 35) {
                        tc = 35;
                    }

                    if (numetorps < maxetorps && (!(random() % tc))) {
                        int xs, ys;

                        /* could aim better, not sure it should! */

                        if (aliens[i].x > plx + 120) { 
                            xs = -3;
                        } else if (aliens[i].x > plx + 60) { 
                            xs = -2;
                        } else if (aliens[i].x < plx - 120) { 
                            xs = 3;
                        } else if (aliens[i].x < plx - 60) { 
                            xs = 2;
                        } else {
                            xs = 0;
                        }
                        ys = (ETORPSPEED + level / 5) - ABS(xs);
                        new_etorp(aliens[i].x, aliens[i].y + aliens[i].shape.height / 2 + 2, xs, ys);
                    }
                }
                if (aliens[i].dir != prevDir) {
                    int d = aliens[i].dir;
                    if (d < 0) {
                        d = 0;  /* convoy/rejoin: use the resting frame */
                    }
                    replaceSprite(&aliens[i].shape, d,
                                  aliens[i].x,
                                  aliens[i].y);
                } else if (!(aliens[i].dir == -1 && aliens[i].escorting < 0 &&
                             !aliens[i].entering && (counter & 1))) {
                    moveSpriteCentered(&aliens[i].shape,
                                  aliens[i].x,
                                  aliens[i].y);
                }
            } else {  /* paused */
                moveSpriteCentered(&aliens[i].shape,
                              aliens[i].x,
                              aliens[i].y);
            }

            if (!paused && !pldead && !plflash && !plshield && !shieldspawn &&
                (ABS(aliens[i].x - plx) < (aliens[i].shape.width / 2 + playerShip->width / 2)) &&
                (ABS(aliens[i].y - (WINHEIGHT - (int) playerShip->height / 2)) < (aliens[i].shape.height / 2 + playerShip->height / 2))) {
                pldead = 1;
                new_explosion(playerShip, plx, WINHEIGHT - (playerShip->height / 2), 2);
                aliens[i].alive = 0;
                aliens[i].dying = 1;
                if (aliens[i].dir >= 0) {
                    attacking--;
                }
                new_explosion(&aliens[i].shape, aliens[i].x, aliens[i].y, 0);
            }
        }
    }
    if (livecount == 0 && !paused) {
        if (warpStep == 1) {
            init_warp();
            delete_etorps();
            warpStep++;
        }
        if (!numtorps && !numetorps && !any_prizes() && 
            !any_explosions() && !any_bubbles()) {
            hideSprite(playerShip);
            warpStep++;
        }
        if (warpStep == 20) {
            play_sound(SND_WARP);
        }
        do {
            if (warpStep == 1) {
                clearScreen(1, 20);
                init_aliens(++level);
                gotlemon = 0;
                warpStep = 1;
                numtorps = 0;
                plx = WINWIDTH / 2;
            } else {
                if (warpStep > 2) {
                    //run until the warp sequence is done
                    if (do_warp()) {
                        warpStep = 1;
                        continue;
                    }
                }
            }
            break;
        } while (1);
    }
}

/*------------------player----------------*/
static void init_player(void) {
    int i;
    for (i = 0; i < MAXTORPS; i++) {
        if (torps[i].alive) {
            torps[i].alive = 0;
            hideSprite(&torps[i].shape);
        }
    }
}

static void init_torps(void) {
    int i;
    W_Image *im;
    for (i = 0; i < MAXTORPS; i++) {
        im = getImage(I_MTORP);
        torps[i].alive = 0;
        memcpy(&torps[i].shape, getImage(I_MTORP), sizeof(W_Image));
        addSprite(&torps[i].shape, -10, -10);
    }
}

static void new_torp(int x, int y, int xs, int ys) {
    int i;

    for (i = 0; i < maxtorps; i++) {
        if (!torps[i].alive) {
            torps[i].x = x;
            torps[i].y = y;
            torps[i].alive = 1;
            torps[i].xspeed = xs;
            torps[i].yspeed = ys;
            numtorps++;
            replaceSprite(&torps[i].shape, 0, x, y);
            play_sound(SND_FIRETORP);
            return;
        }
    }
}

static void do_torps(void) {
    int i, j, k, ne;

    for (i = 0; i < MAXTORPS; i++) {
        if (torps[i].alive) {
            if (!paused) {
                int ptw, ptwL, pth_1;

                torps[i].y += torps[i].yspeed;
                torps[i].x += torps[i].xspeed;
                torps[i].frame++;
                ptw = playerTorp->width / 2;
                ptwL = ptw - 1;
                pth_1 = playerTorp->height / 2 - 1;
                for (k = 0; k < numAliveAlien; k++) {
                    j = aliveAlien[k];
                    if (aliens[j].alive && !aliens[j].dying &&
                        (ABS(torps[i].y - aliens[j].y) < (aliens[j].hitH2 + pth_1)) &&
                        ((torps[i].x - aliens[j].x) < (aliens[j].hitW2 + ptw)) &&
                        ((aliens[j].x - torps[i].x) < (aliens[j].hitW2 + ptwL))) {
                        aliens[j].dying = 1;
                        aliens[j].alive = 0;
                        if (aliens[j].dir >= 0) {
                            attacking--;
                        }

                        torps[i].alive = 0;
                        numtorps--;
                        hideSprite(&torps[i].shape);
                        if (j >= 10) {
                            if (aliens[j].dir < 0) {
                                score += 50;
                            } else {
                                score += (6 - (j / 10)) * 100;
                                if (!(random() % (gotlemon ? 3 : PRIZECHANCE))) {
                                    new_prize(aliens[j].x, aliens[j].y);
                                }
                            }
                            new_explosion(&aliens[j].shape, aliens[j].x,
                                          aliens[j].y, 0);
                        } else {
                            if (aliens[j].dir < 0) {
                                score += 200;
                            } else {
                                ne = 0; /* count how many escorts */
                                for (k = j + 9; k < j + 12; k++) {
                                    if (aliens[k].escorting == j) {
                                        ne++;
                                    }
                                }
                                score_flagship(aliens[j].x, aliens[j].y, ne);
                            }
                            new_explosion(&aliens[j].shape, aliens[j].x,
                                          aliens[j].y, 1);
                        }
                        goto skip;
                    }
                }
                if (torps[i].y < -torps[i].yspeed ||
                    torps[i].x < ABS(torps[i].xspeed) ||
                    torps[i].x > WINWIDTH - ABS(torps[i].xspeed)) {
                    torps[i].alive = 0;
                    hideSprite(&torps[i].shape);
                    numtorps--;
                } else {
                    moveSpriteCentered(&torps[i].shape,
                                  torps[i].x,
                                  torps[i].y);
                }
            skip:;
            } else { /* paused */
                moveSpriteCentered(&torps[i].shape,
                              torps[i].x,
                              torps[i].y);
            }
        }
    }
}

static void do_etorps(void) {
    struct torp *t = first_etorp, *nextt;
    
    while(t) {
        nextt=t->next;
        if(t->alive) {
            if(!paused) { 
                t->y+=t->yspeed;
                t->x+=t->xspeed;
                if(t->y > WINHEIGHT || t->x < 0 || t->x > WINWIDTH) {
                    if (t->next) {
                        t->next->prev = t->prev;
                    }
                    if (t->prev) {
                        t->prev->next = t->next;
                    }
                    if (t == first_etorp) {
                        first_etorp = t->next;
                    }
                    removeSprite(&t->shape);
                    free(t);
                    numetorps--;
                } else if (!pldead && !plflash && !plshield && !shieldspawn &&
                           ((t->x - plx) < 5) && ((plx - t->x) < 6) &&
                           (ABS(t->y - (WINHEIGHT - (int) playerShip->height / 2)) < 6)) { /* DEAD! */
                    pldead = 1;
                    new_explosion(playerShip, plx, WINHEIGHT - playerShip->height / 2, 2);
                } else {
                    if (!(counter & 1)) {
                        t->frame = (t->frame + 1) % t->shape.frames;
                        GTEUpdateSprite(t->shape.spriteId,
                                        t->shape.tileStart + t->frame | t->shape.spriteSize,
                                        getVbuff(t->shape.vbuffIdx + t->frame));
                    }
                    moveSpriteCentered(&t->shape, t->x, t->y);
                }
            } else {
                GTEUpdateSprite(t->shape.spriteId,
                                t->shape.tileStart + t->frame | t->shape.spriteSize,
                                getVbuff(t->shape.vbuffIdx + t->frame));
                moveSpriteCentered(&t->shape, t->x, t->y);
            }
        }
        t = nextt;
    }
}

static void undo_etorps(void) {
    struct torp *t = first_etorp, *nextt;

    while (t) {
        nextt = t->next;
        removeSprite(&t->shape);
        free(t);
        t = nextt;
    }
}

static void undo_player(void) {
    hideSprite(playerShip);
}

static void do_player(int mx, int my, int but) {
    static int torpok;
#ifdef ENABLE_MACHINE_GUN
    static int shotside = 0;
#endif
    int wev;
    static int keys = 0;
    int mdx, mdy, mbut;
    extern int plflash;
    extern W_Image *playerShip;
    extern int plx;

    if (gameOver) {
        wev = GTEReadControl();
        if (!score_key(wev)) switch (wev & 0x7f) {
            case 'q':
            case 'Q':
                xgal_exit(0);
                break;
            case '\r':
                return;
                break;
            case 's':       /* toggle sound on the title screen */
                playSounds = !playSounds;
                return;     /* this key must not start the game */
                break;
            case 'm':
                if (wev & 1024) {
                    mouseControl = 1;
                }
                break;
            case 'k':
                if (wev & 1024) {
                    mouseControl = 0;
                }
                break;
            case ' ':
                gameOver = 0;
                maxtorps = MINTORPS;
                weapon = SINGLESHOT;
                movespeed = MINSPEED;
                ships = 3;
                level = 1;
                undo_aliens();
                undo_prizes();
                init_aliens(level);
                pldead = 0;
                shieldspawn = 0;
                plshield = 0;
                plflash = 0;
                GTEUpdateSprite(playerShip->spriteId,
                                playerShip->tileStart | playerShip->spriteSize, getVbuff(playerShip->vbuffIdx));
                score = 0;
                nextBonus = 20000;
                plx = WINWIDTH/2;
                GTEFillTileStore(0);
                GTERender(0);
                GTELoadTileSet(0, 512, tiles);
                draw_score();
                break;
            default:
                /*printf ("1keyevent %d\n", wev.key); */
                return;     /* unhandled key must not cause any action */
                break;
            }
        return;
    } else {
        int key = GTEReadControl();
        wev = key & 0x7f;
        keys = 0;

        switch (wev) {
        case 'f':
        case 21:
            keys |= RIGHTKEY;
            break;
        case 'b':
        case 8:
            keys |= LEFTKEY;
            break;
        case ' ':
            if (key & 1024) {
                keys |= FIREKEY;
            }
            break;
#ifdef ACTIVATED_SHIELD
        case 'x':
            if (key & 1024) {
                plshield += shieldsleft;
                shieldsleft = 0;
                shieldon = 1;
            }
            break;
#endif
        case 'q':
            if (key & 1024) {
                if (!pldead  && !paused) {
                    new_explosion(playerShip, plx, WINHEIGHT - ((playerShip->height) / 2), 2);
                    ships = 0;
                    pldead = 1;
                }
            }
            break;
        case 'Q':
            xgal_exit(0);
            break;
        case 'p':
            if (key & 1024) {
                paused = !paused;
                if (!paused) {
                    undo_pause();
                }
            }
            break;
        case 's':
            if (key & 1024) {
                playSounds = !playSounds;
            }
            break;
        case 'm':
            if (key & 1024) {
                mouseControl = 1;
            }
            break;
        case 'k':
            if (key & 1024) {
                mouseControl = 0;
            }
            break;
#ifdef IM_A_BIG_FAT_CHEATER
        case 'i': //invinsible
            if (key & 1024) {
                if (plflash == 0) {
                    plflash = -2;
                } else {
                    plflash = 0;
                }
            }
            break;
        case 'l': //kill em all
            if (key & 1024) {
                int i;
                for (i = 0; i < MAXALIENS; i++) {
                    aliens[i].alive = 0;
                }
            }
            break;
        case 'c': //next bonus please
            if (key & 1024) {
                score += BONUSSHIPSCORE;
            }
            break;
        case 'h': //shield on
            if (key & 1024) {
                plshield = SHIELDTIME;
                play_sound(SND_SHIELD);
            }
            break;
        case 'w': //next weapon
            if (key & 1024) {
                weapon++;
                if (weapon == NUMWEAPONS) {
                    weapon = 0;
                }
            }
            break;
        case 't': // increase the number of torpedos
            if (key & 1024) {
                maxtorps++;
                if (maxtorps > MAXTORPS) {
                    maxtorps = MINTORPS;
                }
            }
            break;
#endif /* IM_A_BIG_FAT_CHEATER */
        default:
            break;
        }

        if (!paused) {
            torpok--;
            if (warpStep < 3) {
                if (mouseControl) {
                    mouseGet(&mdx, &mdy, &mbut);
                    keys &=  ~(LEFTKEY | RIGHTKEY);
                    if (mdx) {
                        if (mdx & 0x40) {
                            mdx = -2;
                            keys |= LEFTKEY;
                        } else {
                            mdx = 2;
                            keys |= RIGHTKEY;
                        }
                    }
                }
                if (keys & LEFTKEY) {
                    mx = 0;
                } else if (keys & RIGHTKEY) {
                    mx = WINWIDTH;
                } else {
                    mx = plx;
                }

                but = 0;
                if (keys & FIREKEY) {
                    but = 1;
                } else {
                    but = 0;
                }
                if (mouseControl && mbut) {
                    but = 1;
                }
            }
            if (pldead) {
                if (pldead == 1) {
                    ships--;
                    if (ships <= 0) {
                        W_Image *go = getImage(I_GAMEOVER);
                        GTELoadTileSet(352, 512, (tiles + (512L * 128L)));
                        for (int j = 0; j < go->height; j++) {
                            for (int i = 0; i < go->width; i++) {
                                GTESetTile(13 + i, 8 + j, go->tileStart + (j * 32) + i);
                            }
                        }
                    }
                }
                pldead++;

                if (pldead >= 100) {
                    if (ships <= 0) {
                        title_page = 0;
                        gameOver = 1;
                        undo_aliens();
                        undo_etorps();
                        if (check_score(score)) {
                            getting_name = 1;
                        }
                    } else {
#ifdef DISABLE_RESET_ON_DEATH
                        //ships--;
                        maxtorps--;
                        if (maxtorps < MINTORPS) {
                            maxtorps = MINTORPS;
                        }
                        switch (weapon) {
                        case SINGLESHOT:
                            if (maxtorps < 3) {
                                maxtorps = 3;
                                weapon = SINGLESHOT;
                            }
                            break;
                        case DOUBLESHOT:
                            if (maxtorps < 4) {
                                maxtorps = 4;
                            }
                            break;
                        case SPREADSHOT:
                            if (maxtorps < 5) {
                                maxtorps = 5;
                            }
                            break;
                        case TRIPLESHOT:
                            if (maxtorps < 6) {
                                maxtorps = 6;
                            }
                            break;
                        case MACHINEGUN:
                            if (maxtorps < 3) {
                                maxtorps = 3;
                            }
                            break;
                        }
#else
                        //ships--;
                        maxtorps = MINTORPS;
                        weapon = SINGLESHOT;
                        movespeed = MINSPEED;
#endif /* DISABLE_RESET_ON_DEATH */
                        pldead = 0;
                        plflash = 0;
                        shieldspawn = SPAWNSHIELDTIME;
                        plx = WINWIDTH/2;
                        GTEUpdateSprite(playerShip->spriteId,
                                        shieldShip->tileStart | shieldShip->spriteSize, getVbuff(shieldShip->vbuffIdx));
                        moveSpriteCentered(playerShip, plx, PLAYER_Y);
                    }
                }
                return;
            }

            if (but && torpok <= 0 && (warpStep == 1)) {
                int noseY = PLAYER_Y + playerTorp->height / 2;
                switch (weapon) {
                case SINGLESHOT:
                    if (numtorps < maxtorps) {
                        new_torp(plx, noseY, 0, -TORPSPEED);
                    }
                    torpok = TORPDELAY;
                    break;
                case DOUBLESHOT:
                    if (numtorps < maxtorps - 1) {
                        new_torp(plx - 2, noseY, 0, -TORPSPEED);
                        new_torp(plx + 2, noseY, 0, -TORPSPEED);
                        torpok = TORPDELAY;
                    }
                    break;
                case TRIPLESHOT:
                    if (numtorps < maxtorps - 2) {
                        new_torp(plx - 3, noseY, -2, 1 - TORPSPEED);
                        new_torp(plx,   noseY, 0,   -TORPSPEED);
                        new_torp(plx + 3, noseY, 2, 1 - TORPSPEED);
                        torpok = TORPDELAY;
                    }
                    break;
#ifdef ENABLE_SPREAD_SHOT
                case SPREADSHOT:
                    if (numtorps == 0) {
                        if ((maxtorps % 2) == 1) new_torp(plx, noseY, 0, -TORPSPEED * 1.15);
                        else {
                            new_torp(plx - 5, noseY, 0, -TORPSPEED * 1.15);
                            new_torp(plx + 5, noseY, 0, -TORPSPEED * 1.15);
                        }
                        if (maxtorps > 2) {
                            new_torp(plx, noseY - 15, -2, -TORPSPEED * 1.15);
                            new_torp(plx, noseY - 15, 2, -TORPSPEED * 1.15);
                        }
                        if (maxtorps > 4) {
                            new_torp(plx, noseY - 25, -4, -TORPSPEED * 1.15);
                            new_torp(plx, noseY - 25, 4, -TORPSPEED * 1.15);
                        }
                        if (maxtorps > 6) {
                            new_torp(plx, noseY - 35, -6, -TORPSPEED * 1.15);
                            new_torp(plx, noseY - 35, 6, -TORPSPEED * 1.15);
                        }
                        if (maxtorps > 8) {
                            new_torp(plx, noseY - 50, -8, -TORPSPEED * 1.15);
                            new_torp(plx, noseY - 50, 8, -TORPSPEED * 1.15);
                        }
                        if (maxtorps > 10) {
                            new_torp(plx, noseY - 60, -10, -TORPSPEED * 1.15);
                            new_torp(plx, noseY - 60, 10, -TORPSPEED * 1.15);
                        }
                        torpok = TORPDELAY;
                    }
                    break;
#endif /* ENABLE_SPREAD_SHOT */
#ifdef ENABLE_MACHINE_GUN
                case MACHINEGUN:
                    if (numtorps < maxtorps) {
                        shotside = (shotside == -15) ? 15 : -15;
                        new_torp(plx + shotside, noseY, 0, -TORPSPEED * 1.3);
                        torpok = TORPDELAY - 2;
                    }
                    break;
#endif /* ENABLE_MACHINE_GUN */
                }
            }

            if (warpStep < 3) {
                if (!but) {
                    //torpok = 0;
                }
                if ((mx / movespeed) > (plx / movespeed)) {
                    plx += movespeed;
                } else if ((mx / movespeed) < (plx / movespeed)) {
                    plx -= movespeed;
                }
#ifdef ENABLE_SHIP_WRAP

                if (plx < 10) {
                    plx = WINWIDTH - 10;
                }
                if (plx > WINWIDTH - 10) {
                    plx = 10;
                }
#else
                if (plx < playerShip->width / 2) {
                    plx = playerShip->width / 2;
                }
                if (plx > WINWIDTH - playerShip->width / 2) {
                    plx = WINWIDTH - playerShip->width / 2;
                }
#endif

                if (plflash > 0) {
                    plflash--;
                }
                moveSpriteCentered(playerShip, plx, PLAYER_Y);
                if (plshield > 0) {
                    plshield--;
                }
                if (shieldspawn > 0) {
                    shieldspawn--;
                }
                if (shieldspawn > 0) {
                    if ((shieldspawn <= 20) && (shieldspawn % 2)) { /* last 20 frames, toggle every 1: off */
                        GTEUpdateSprite(playerShip->spriteId,
                                        shieldShip->tileStart | shieldShip->spriteSize | GTE_SPRITE_HIDE,
                                        getVbuff(shieldShip->vbuffIdx));
                    } else {
                        GTEUpdateSprite(playerShip->spriteId,
                                        shieldShip->tileStart | shieldShip->spriteSize,
                                        getVbuff(shieldShip->vbuffIdx));
                    }
                } else if (plshield && ((plshield > SHIELDTIME / 4) || plshield % 2)) {
                    GTEUpdateSprite(playerShip->spriteId,
                                    shieldShip->tileStart | shieldShip->spriteSize,
                                    getVbuff(shieldShip->vbuffIdx));
                } else {
                    static int ignitionTime = 0;
                    int offset = keys & RIGHTKEY ? 1 : keys & LEFTKEY ? 3 : 0;
                    if (offset == 0) {
                        ignitionTime = ignitionTime < 10 ? ignitionTime +1 : 0;
                        offset = ignitionTime > 4 ? 2 : 0;
                    }
                    GTEUpdateSprite(playerShip->spriteId,
                                    playerShip->tileStart+ offset | playerShip->spriteSize, getVbuff(playerShip->vbuffIdx+offset));
                }
            } else {
                hideSprite(playerShip);
            }
        } else if (!pldead) { /* paused */
            moveSpriteCentered(playerShip, plx, PLAYER_Y);
        }
    }
}

#pragma databank 1 
void timerTick(void)
{
    extern word frameTick;
    frameTick = 1;
}
#pragma databank 0

void initFramerate(void) {
    GTEAddTimer(4, (pointer) &timerTick, 0x0000);
}

int main(int argc, char **argv) {
    //int ac;
    //char *dpyname = 0;
    int mx = 0, my = 0, but = 0;
    //int start_fullscreen = 1;

    TLStartUp();
    TOOLFAIL("Unable to start tool locator");

    userId = MMStartUp();
    TOOLFAIL("Unable to start memory manager");

    MTStartUp();
    TOOLFAIL("Unable to start misc tools");

#ifdef USE_RESOURCES
    ResourceStartUp(userId);
    curApp = GetCurResourceApp();
    fID = OpenResourceFile(readEnable, NULL, LGetPathname2(userId, 1));
#endif 


    init_sound(userId);

    startupGTE(userId);

    GTESetPalette(0, (Pointer) tilesPalette);
    GTELoadTileSet(0, 512, tiles);   /* Load in the tiles */
    GTEFillTileStore(0);
    GTERender(0);
    initFramerate();
    initVbufs();

    //WINHEIGHT -= (W_Textheight + 1);
    playerShip = getImage(I_PLAYER1);
    playerTorp = getImage(I_MTORP);
    enemyTorp = getImage(I_ETORP);
    shieldShip = getImage(I_SHIELD);

    level = startLevel;   /* change made here */

    load_scores();
    init_torps();
    init_aliens(level);
    init_explosions();
    init_score();
    init_prizes();
    init_player();
    mouseInit();
    ships = 3;
    nextBonus = 20000;
    gameOver = 1;
    frameTick = 1;

    #ifdef BENCHMARK
    bm_file = fopen("9:xgspeed.log", "w");
    fprintf(bm_file, "Started\n");
    fflush(bm_file);
#endif

    while (1) {
        //while the warp is being drawn disable the Rendering
        if (!frameTick && warpStep < 4) {
#ifdef BENCHMARK
            bm_rendered++;
#endif
            GTERender(0);
            releaseSpriteIds();
            continue;
        }
        frameTick = 0;
        counter++;
#ifdef BENCHMARK
        bm_frames++;
        bm_renderTotal += bm_rendered;
        if (bm_rendered < bm_minRender) {
            bm_minRender = bm_rendered;
        }
        if (bm_rendered > bm_maxRender) {
            bm_maxRender = bm_rendered;
        }
        if (bm_frames >= 300) {
            fprintf(bm_file, "renders/frame: avg=%u.%u min=%u max=%u frames=%u\n",
                    (unsigned int) (bm_renderTotal / bm_frames),
                    (unsigned int) ((bm_renderTotal * 10 / bm_frames) % 10),
                    bm_minRender, bm_maxRender, bm_frames);
            fflush(bm_file);
            bm_frames = 0;
            bm_renderTotal = 0;
            bm_minRender = 0xffff;
            bm_maxRender = 0;
        }
        bm_rendered = 0;
#endif
        do_player(mx, my, but);
        if (!getting_name) {
            do_etorps();
            do_aliens();
            do_torps();
            do_prizes();
            do_explosions();
        }
       do_score();
        if(gameOver){
            do_title(); 
            if(getting_name) {
                do_name();
                title_page = 3;
             }
        }
        if(paused) {
            do_pause();
        }
    }
    return (0);
}


