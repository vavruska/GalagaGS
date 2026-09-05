/* $Id: prize.c,v 1.2 1998/04/30 05:11:58 mrogre Exp $ */
/* Copyright (c) 1998 Joe Rumsey (mrogre@mediaone.net) */
#include "copyright.h"

#include <stdlib.h>
#include "images.h"
#include "struct.h"
#include "data.h"
#include "defs.h"
#include "proto.h"
#include "sound.h"
#include "gte.h"

#define PR_SING 0
#define PR_DOUB 1
#define PR_TRIP 2
#define PR_SPEED 3
#define PR_SHIELD 4
#define PR_SMART 5
#define PR_LEMON 6
#define PR_EXTRABULLET 7

#ifdef ENABLE_SPREAD_SHOT
    #define PR_SPREAD 8
#endif

#ifdef ENABLE_MACHINE_GUN
    #ifdef ENABLE_SPREAD_SHOT
        #define PR_MACHINE 9
        #define NUMPRIZES 10
    #else
        #define PR_MACHINE 8
        #define NUMPRIZES 9
    #endif /* ENABLE_SPREAD_SHOT */
#else
    #ifdef ENABLE_SPREAD_SHOT
        #define PR_SPREAD 8
        #define NUMPRIZES 9
    #else
        #define NUMPRIZES 8
    #endif /* ENABLE_SPREAD_SHOT */
#endif /* ENABLE_MACHINE_GUN */

#define PRIZESPEED 3

W_Image *prizeImages[NUMPRIZES];

struct prize {
    struct prize *next, *prev;
    int x, y, type, dying;
    W_Image shape;
} *first_prize; /* 8-) */

void init_prizes(void) {
    prizeImages[0] = getImage(I_PR_SING);
    prizeImages[1] = getImage(I_PR_DOUB);
    prizeImages[2] = getImage(I_PR_TRIP);
    prizeImages[3] = getImage(I_PR_SPEED);
    prizeImages[4] = getImage(I_PR_SHIELD);
    prizeImages[5] = getImage(I_PR_BRAIN);
    prizeImages[6] = getImage(I_PR_LEMON);
    prizeImages[7] = getImage(I_PR_EXTRABULLET);
#ifdef ENABLE_SPREAD_SHOT
    prizeImages[8] = getImage(I_PR_SPREAD);
#endif

#ifdef ENABLE_MACHINE_GUN
    prizeImages[NUMPRIZES - 1] = getImage(I_PR_MACHINE);
#endif
}


bool any_prizes(void) {
    return (first_prize != NULL);
}

void new_prize(int x, int y) {
    struct prize *p;

    p = malloc(sizeof(struct prize));
    p->next = first_prize;
    p->prev = 0;
    if (first_prize) {
        first_prize->prev = p;
    }
    first_prize = p;
    p->x = x;
    p->y = y;
    p->type = random() % NUMPRIZES;
    p->dying = 0;
    memcpy(&p->shape, prizeImages[p->type], sizeof(W_Image));
    addSpriteCentered(&p->shape, x, y);
}

void undo_prizes(void) {
    struct prize *p = first_prize, *nextp;
    W_Image *image;

    while (p) {
        nextp = p->next;
        image = prizeImages[p->type];

        removeSprite(&p->shape);

        if (p->dying) {
            if (p->next) {
                p->next->prev = p->prev;
            }
            if (p->prev) {
                p->prev->next = p->next;
            }
            if (p == first_prize) {
                first_prize = p->next;
            }
            free(p);
        }
        p = nextp;
    }
}

void do_prizes(void) {
    struct prize *p = first_prize, *nextp;
    int i, k, ne;
    int oldPlaySounds;

    while (p) {
        nextp = p->next;

        /* Collected (or off-screen) last frame: clean up this entry. */
        if (p->dying) {
            if (p->next) {
                p->next->prev = p->prev;
            }
            if (p->prev) {
                p->prev->next = p->next;
            }
            if (p == first_prize) {
                first_prize = p->next;
            }
            removeSprite(&p->shape);
            free(p);
            p = nextp;
            continue;
        }

        if (!paused) {
            p->y += PRIZESPEED;
        }
        moveSpriteCentered(&p->shape, p->x, p->y);

        if (!p->dying && p->y > (WINHEIGHT - 16) && (ABS(p->x - plx) < 8)
#ifdef NO_PRIZE_WHILE_DEAD
            && !pldead
#endif
           ) {
            p->dying = 1;
            hideSprite(&p->shape);
            play_sound(SND_DDLOO);
            switch (p->type) {
            case PR_SING:
                if (weapon == SINGLESHOT) {
                    maxtorps++;
                } else {
                    weapon = SINGLESHOT;
                }
                break;
            case PR_DOUB:
                if (weapon == DOUBLESHOT) {
                    maxtorps++;
                } else {
                    weapon = DOUBLESHOT;
                    if (maxtorps < 4) {
                        maxtorps = 4;
                    }
                }
                break;
            case PR_TRIP:
                if (weapon == TRIPLESHOT) {
                    maxtorps++;
                } else {
                    weapon = TRIPLESHOT;
                    if (maxtorps < 6) {
                        maxtorps = 6;
                    }
                }
                break;
            case PR_SPEED:
                if (movespeed < MAXSPEED) {
                    movespeed++;
                }
                break;
#ifdef ENABLE_SPREAD_SHOT
            case PR_SPREAD:
                if (weapon == SPREADSHOT) {
                    maxtorps++;
                } else {
                    weapon = SPREADSHOT;
                    if (maxtorps < 5) {
                        maxtorps = 5;
                    }
                }
                break;
#endif /* ENABLE_SPREAD_SHOT */
#ifdef ENABLE_MACHINE_GUN
            case PR_MACHINE:
                if (weapon == MACHINEGUN) {
                    maxtorps++;
                } else {
                    weapon = MACHINEGUN;
                    if (maxtorps < 3) {
                        maxtorps = 3;
                    }
                }
                break;
#endif /* ENABLE_MACHINE_GUN */
            case PR_SHIELD:
#ifdef ACTIVATED_SHIELD
                if (shieldon) {
                    plshield += SHIELDTIME;
                } else {
                    shieldsleft += SHIELDTIME;
                }
                if (shieldsleft + plshield > MAXSHIELDS) {
                    if (shieldon) {
                        shieldsleft = 0;
                        plshield = MAXSHIELDS;
                    } else {
                        shieldsleft = MAXSHIELDS;
                        plshield = 0;
                    }
                }
#else
                plshield = SHIELDTIME;
                play_sound(SND_SHIELD);
#endif /* ACTIVATED_SHIELD */
                break;
            case PR_SMART:
                play_sound(SND_SMART);
                oldPlaySounds = playSounds;
                playSounds = 0;
                for (i = 0; i < MAXALIENS; i++) {
                    //smart bomb only affects aliens on screen
                    if (aliens[i].alive && !aliens[i].dying && aliens[i].y >=0) {
                        aliens[i].dying = 1;
                        aliens[i].alive = 0;
                        if (i >= 10) {
                            if (aliens[i].dir < 0) {
                                score += 50;
                            } else {
                                score += (6 - (i / 10)) * 100;
                                if (!(random() % (gotlemon ? 3 : PRIZECHANCE))) {
                                    new_prize(aliens[i].x, aliens[i].y);
                                }
                            }
                            new_explosion(&aliens[i].shape,
                                          aliens[i].x,
                                          aliens[i].y, 0);
                        } else {
                            if (aliens[i].dir < 0) {
                                score += 200;
                            } else {
                                ne = 0; /* count how many escorts */
                                for (k = i + 9; k < i + 12; k++) {
                                    if (aliens[k].escorting == i) {
                                        ne++;
                                    }
                                }
                                score_flagship(aliens[i].x, aliens[i].y, ne);
                            }
                            new_explosion(&aliens[i].shape,
                                          aliens[i].x,
                                          aliens[i].y, 1);
                        }
                    }
                }
                playSounds = oldPlaySounds;
                break;
            case PR_LEMON:
                gotlemon = 1;
                maxtorps = MINTORPS;
                weapon = 0;
                movespeed = MINSPEED;
                break;
            case PR_EXTRABULLET:
                if (maxtorps < MAXTORPS) {
                    maxtorps++;
                }
                break;
            }
            if (maxtorps > MAXTORPS) {
                maxtorps = MAXTORPS;
            }
        } else if (p->y > WINHEIGHT) {
            p->dying = 1;
            hideSprite(&p->shape);
        }
        p = nextp;
    }
}


