/* $Id: explosions.c,v 1.2 1998/04/30 05:11:54 mrogre Exp $ */
/* Copyright (c) 1998 Joe Rumsey (mrogre@mediaone.net) */
#include "copyright.h"

#include <types.h>
#include <stdlib.h>
#include "images.h"
#include "struct.h"
#include "data.h"
#include "proto.h"
#include "sound.h"
#include "gte.h"

static struct explosion *first_exp=0;
static struct score_bubble *first_bub=0;

W_Image *expImage, *bubbleImages[4];

bool any_bubbles(void) {
    return (first_bub != NULL);
}

static void undo_bubbles(void) {
    struct score_bubble *bub=first_bub, *nextbub;

    while(bub) {
    	nextbub = bub->next;
    	if(bub->count <= 0) {
            GTESetTile(bub->x, bub->y, 0);
            GTESetTile(bub->x + 1, bub->y, 0);
            if (bub->prev) {
                bub->prev->next = bub->next;
            }
            if (bub->next) {
                bub->next->prev = bub->prev;
            }
            if (first_bub == bub) {
                first_bub = NULL;
            }
    	    free(bub);
    	}
    	bub=nextbub;
    }
}

static void do_bubbles(void) {
    struct score_bubble *bub=first_bub;
    bool needToUndo = false;

    while(bub) {
    	if(!paused && bub->count) {
    	    bub->count--;
            if (!bub->count) {
                needToUndo = true;
            }
    	}
        bub = bub->next;
    }
    if (needToUndo) {
        undo_bubbles();
    }
}

bool any_explosions(void) {
    return (first_exp != NULL);
}

void undo_explosions(void) {
    struct explosion *exp=first_exp, *nextexp;
    W_Image *image = getImage(I_EXPLOSION);

    while(exp) {
    	nextexp = exp->next;
    	if(exp->frame >= image->frames) {
            GTEUpdateSprite(exp->spriteId, image->tileStart | image->spriteSize | GTE_SPRITE_HIDE,
                            getVbuff(image->vbuffIdx));
            if (exp->prev) {
                exp->prev->next = exp->next;
            }
            if (exp->next) {
                exp->next->prev = exp->prev;
            }
            if (first_exp == exp) {
                first_exp = 0;
            }
    	    free(exp);
    	}
    	exp=nextexp;
    }
    undo_bubbles();
}

void do_explosions(void) {
    struct explosion *exp = first_exp;
    struct explosion *next;
    W_Image *image = getImage(I_EXPLOSION);

    while (exp) {
        next = exp->next;
        if (exp->frame >= image->frames) {
            GTEUpdateSprite(exp->spriteId,
                            image->tileStart | image->spriteSize | GTE_SPRITE_HIDE,
                            getVbuff(image->vbuffIdx));
            if (exp->prev) {
                exp->prev->next = exp->next;
            } else {
                first_exp = exp->next;
            }
            if (exp->next) {
                exp->next->prev = exp->prev;
            }
            free(exp);
        } else {
            GTEUpdateSprite(exp->spriteId,
                            image->tileStart + exp->frame | image->spriteSize,
                            getVbuff(image->vbuffIdx + exp->frame));
            if (!paused) {
                exp->frame++;
            }
        }
        exp = next;
    }
    if (first_bub) {
        do_bubbles();
    }
}

void new_explosion(W_Image *image, int x, int y, int type) {
    struct explosion *exp;

    exp = malloc(sizeof(struct explosion));
    exp->next = first_exp;
    exp->prev = 0;
    if (exp->next) {
        exp->next->prev = exp;
    }
    first_exp = exp;

    exp->x = x;
    exp->y = y;
    exp->frame = 0;
    exp->spriteId = image->spriteId;
    exp->image = image;

    switch (type) {
    case 1:
        play_sound(SND_EXPLOSION);
        break;
    case 2:
        play_sound(SND_EXP_SB);
        break;
    case 0:
    default:
        play_sound(SND_TORPHIT);
        break;
    }
}

void score_flagship(int x, int y, int ne) {
    struct score_bubble *bub;

    bub = malloc(sizeof(struct score_bubble));
    bub->next = first_bub;
    bub->prev = 0;
    if (bub->next) {
        bub->next->prev = bub;
    }
    first_bub = bub;

    //assuming the width of an alien is 12
    bub->x = (x-3)/4;
    bub->y = (y-1)/8;
    bub->count = 10;
    bub->shape = bubbleImages[ne];
    if (bub->x >= 0 && bub->x < 40 && bub->y > 0 && bub->y < 24) {
        GTESetTile(bub->x, bub->y, bub->shape->tileStart | TILE_PRIORITY_BIT);
        GTESetTile(bub->x + 1, bub->y, bub->shape->tileStart + 1 | TILE_PRIORITY_BIT);
    }
    switch (ne) {
    case 0:
        score += 500;
        break;
    case 1:
        score += 1000;
        break;
    case 2:
        score += 2000;
        break;
    case 3:
        score += 4000;
        break;
    }
}

void init_explosions(void) {
    expImage = getImage(I_EXPLOSION);

    bubbleImages[0] = getImage(I_S500);
    bubbleImages[1] = getImage(I_S1000);
    bubbleImages[2] = getImage(I_S2000);
    bubbleImages[3] = getImage(I_S4000);
}
