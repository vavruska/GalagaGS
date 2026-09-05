/* images.c.  Contains the image control data. [BDyess] */
/* automatically created by scripts/mkimgsrc */

#include <types.h>
#include <stdlib.h>
#include <misctool.h>
#include <stdbool.h>

#include "defs.h"
#include "images.h"
#include "struct.h"
#include "proto.h"
#include "data.h"
#include "gte.h"

int aliensIdx[] = {I_ALIEN1, I_ALIEN2, I_ALIEN3, I_ALIEN4, I_ALIEN5, I_ALIEN6, I_ALIEN7,
                I_ALIEN8, I_ALIEN9, I_ALIEN10, I_ALIEN11, I_ALIEN12, I_ALIEN13, I_ALIEN14,
                I_ALIEN15, I_ALIEN16, I_ALIEN17 };

/* keep sorted (for bsearch) [BDyess] */
static W_Image imagearray[] = {
/* {width,height,frames,spriteID,filename,startTile,sequences,mksprite,vbuffIdx,
   xpmdata,spriteSize} */
    // width needs to be half the real width for sprites only
    { 7, 12, 0, 0, "/al", 0, 0, 0, 0, GTE_SPRITE_16X16 },
    { 6, 12, 0, 0, "/alien1", 193, 5, 0, 0, GTE_SPRITE_16X16 },
    { 6, 12, 0, 0, "/alien10", 833, 0, 0, 0, GTE_SPRITE_16X16 },
    { 6, 12, 0, 0, "/alien11", 843, 0, 0, 0, GTE_SPRITE_16X16 },
    { 6, 12, 0, 0, "/alien12", 853, 0, 0, 0, GTE_SPRITE_16X16 },
    { 7, 11, 0, 0, "/alien13", 897, 0, 0, 0, GTE_SPRITE_16X16 },
    { 5, 11, 0, 0, "/alien14", 907, 0, 0, 0, GTE_SPRITE_16X16 },
    { 7, 12, 0, 0, "/alien15", 917, 0, 0, 0, GTE_SPRITE_16X16 },
    { 6, 11, 0, 0, "/alien16", 961, 0, 0, 0, GTE_SPRITE_16X16 },
    { 6, 12, 0, 0, "/alien17", 971, 0, 0, 0, GTE_SPRITE_16X16 },
    { 6, 12, 0, 0, "/alien2", 203, 5, 0, 0, GTE_SPRITE_16X16 },
    { 6, 12, 0, 0, "/alien3", 213, 5, 0, 0, GTE_SPRITE_16X16 },
    { 6, 12, 0, 0, "/alien4", 257, 5, 0, 0, GTE_SPRITE_16X16 },
    { 6, 12, 0, 0, "/alien5", 267, 5, 0, 0, GTE_SPRITE_16X16 },
    { 6, 12, 0, 0, "/alien6", 277, 5, 0, 0, GTE_SPRITE_16X16 },
    { 6, 12, 0, 0, "/alien7", 769, 0, 0, 0, GTE_SPRITE_16X16 },
    { 6, 13, 0, 0, "/alien8", 779, 0, 0, 0, GTE_SPRITE_16X16 },
    { 6, 12, 0, 0, "/alien9", 789, 0, 0, 0, GTE_SPRITE_16X16 },
    { 0, 0, 7, 0, "/etorp", 182, 7, 0, 0, GTE_SPRITE_8X8 },
    { 0, 0, 5, 0, "/explosion", 23, 5, 0, 0, GTE_SPRITE_16X16 },
    { 12, 2, 0, 0, "/extra", 9, 0, 0, 0, 0 },
    { 8, 8, 0, 0, "/miniship", 179, 0, 0, 0, 0 },
    { 4, 8, 0, 0, "/mtorp", 180, 1, 0, 0, GTE_SPRITE_8X8 },
    { 24, 6, 0, 0, "/pause", 321, 0, 0, 0, 0 },
    { 6, 14, 0, 0, "/player1", 1, 4, 1, 0, GTE_SPRITE_16X16 },
/* {width,height,frames,xpm,filename,loaded,alternate,bad,compiled_in,xbmdata,xpmdata,pixmap,clipmask} */
    { 0, 0, 0, 0, "/player2", 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, "/player3", 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, "/pr_blank", 177, 1, 0, 0, GTE_SPRITE_16X8 },
    { 0, 0, 0, 0, "/pr_brain", 175, 1, 0, 0, GTE_SPRITE_16X8 },
    { 0, 0, 0, 0, "/pr_doub", 161, 1, 0, 0, GTE_SPRITE_16X8 },
    { 0, 0, 0, 0, "/pr_extrabullet", 163, 1, 0, 0, GTE_SPRITE_16X8 },
    { 0, 0, 0, 0, "/pr_lemon", 165, 1, 0, 0, GTE_SPRITE_16X8 },
#ifndef ORIGINAL_XGALAGA
    { 0, 0, 0, 0, "/pr_machine", 0, 0, 0, 0, 0 },
#endif
    { 0, 0, 0, 0, "/pr_shield", 167, 1, 0, 0, GTE_SPRITE_16X8 },
    { 0, 0, 0, 0, "/pr_sing", 169, 1, 0, 0, GTE_SPRITE_16X8 },
    { 0, 0, 0, 0, "/pr_speed", 171, 1, 0, 0, GTE_SPRITE_16X8 },
#ifndef ORIGINAL_XGALAGA
    { 0, 0, 0, 0, "/pr_spread", 0, 0, 0, 0, 0 },
#endif
    { 0, 0, 0, 0, "/pr_trip", 173, 1, 0, 0, GTE_SPRITE_16X8 },
    { 8, 8, 0, 0, "/s1000", 223, 0, 0, 0, GTE_SPRITE_16X8 },
    { 8, 16, 0, 0, "/s2000", 189, 0, 0, 0, GTE_SPRITE_16X8 },
    { 8, 16, 0, 0, "/s4000", 191, 0, 0, 0, GTE_SPRITE_16X8 },
    { 8, 16, 0, 0, "/s500", 159, 0, 0, 0, GTE_SPRITE_16X8 },
    { 6, 16, 0, 0, "/shield", 287, 1, 0, 0, GTE_SPRITE_16X16 },
    { 14, 5, 0, 0, "/title", 352, 0, 0, 0, 0 },
    { 13, 2, 0, 0, "/gameover", 431, 0, 0, 0, 0},
};

word dirs[16][2] = {
    { 0, 0 },
    { 1, 0 },
    { 2, 0 },
    { 3, 0 },
    { 4, 0 },
    { 3, GTE_SPRITE_VFLIP },
    { 2, GTE_SPRITE_VFLIP },
    { 1, GTE_SPRITE_VFLIP },
    { 0, GTE_SPRITE_VFLIP },
    { 1, GTE_SPRITE_VFLIP | GTE_SPRITE_HFLIP },
    { 2, GTE_SPRITE_VFLIP | GTE_SPRITE_HFLIP },
    { 3, GTE_SPRITE_VFLIP | GTE_SPRITE_HFLIP },
    { 4, GTE_SPRITE_HFLIP },
    { 3, GTE_SPRITE_HFLIP },
    { 2, GTE_SPRITE_HFLIP },
    { 1, GTE_SPRITE_HFLIP }
};

static word vbuffs[GTE_VBUFF_SLOT_COUNT] = { 0 };
//static word nSprites = 0;
static int sprites[GTE_MAX_SPRITE] = { 0 };

extern Byte tiles[];

static int getSpriteID(void) {
    int i;

    //I feel as though this could be optimized to start the loop after all the 
    //static sprites (aliens, torpedo, etc) have been added
    for (i = 0; i < GTE_MAX_SPRITE; i++) {
        if (sprites[i] == 0) {
            sprites[i] = 1;
            return i;
        }
    }
    asm {
        // debug in case we run out of sprite slots
        brk 0x79
    };
    return -1;
}

void releaseSpriteIds(void) {
    int i;
    // a sprite id can not be reused until at least 1 render has been executed. This allows the sprite
    // to be erased, then the id is reusable.
    for (i = 0; i < GTE_MAX_SPRITE; i++) {
        if (sprites[i] == -1) {
            sprites[i] = 0;
        }
    }
}

void initVbufs(void) {
    int order[] = { I_ETORP, I_MTORP, I_EXPLOSION, I_PLAYER1, I_SHIELD, I_PR_BLANK,
        I_PR_BRAIN, I_PR_DOUB, I_PR_EXTRABULLET, I_PR_LEMON, I_PR_SHIELD,
        I_PR_SING, I_PR_SPEED, I_PR_TRIP, I_ALIEN1, I_ALIEN2, I_ALIEN3, 
        I_ALIEN4, I_ALIEN5, I_ALIEN6,
    };
    int i, j;
    int vbuff = 0;
    W_Image *IM = NULL;
    int step;

    for (i = 0; i < (int)(sizeof(order) / sizeof(order[0])); i++) {
        if (vbuff > 64) {
            //this is for debug during development to make sure we dont overflow the vbuffs
            asm {
                brk 0x69
            };
        }
        IM = &imagearray[order[i]];
        for (j = 0; j < IM->sequences; j++) {
            vbuffs[vbuff] = (GTE_VBUFF_SPRITE_START + vbuff * GTE_VBUFF_SPRITE_STEP);
            step = (IM->spriteSize & GTE_SPRITE_16X8) ? 2 : 1;
            if (j == 0) {
                IM->vbuffIdx = vbuff;
            }
            GTECreateSpriteStamp(IM->spriteSize | (IM->tileStart + (j * step)), 
                                 vbuffs[vbuff]);
            vbuff++; 
        }
    }
}

void loadLevelSprites(void) {
    int r,s,a;

    W_Image *source, *dest;

    for (r = 0; r < 6; r++) {
        a = levelAlienRows[r];
        source = getImage(aliensIdx[a]);
        dest = getImage(aliensIdx[r]);
        GTELoadTileSet(dest->tileStart, dest->tileStart + 10, (tiles + (source->tileStart * 128L)));
        GTELoadTileSet(dest->tileStart+32, dest->tileStart + 42, (tiles + ((source->tileStart+32) * 128L)));
        for (s = 0; s < 5; s++) {
            int step = (dest->spriteSize & GTE_SPRITE_16X8) ? 2 : 1;
            GTECreateSpriteStamp(dest->spriteSize + dest->tileStart + (s * step), 
                                 vbuffs[dest->vbuffIdx+s]);
        }
    }
}

const word getVbuff(word vbufIdx) {
    return vbuffs[vbufIdx];
}

void hideSprite(W_Image *image) {
    GTEUpdateSprite(image->spriteId, image->tileStart | image->spriteSize | GTE_SPRITE_HIDE, vbuffs[image->vbuffIdx]);
}

static void spriteCenterOffset(word spriteSize, int *ox, int *oy) {
    switch (spriteSize & 0x1800) {
    case GTE_SPRITE_16X16: *ox = 4; *oy = 8; break;
    case GTE_SPRITE_16X8:  *ox = 4; *oy = 4; break;
    case GTE_SPRITE_8X16:  *ox = 2; *oy = 8; break;
    default:               *ox = 2; *oy = 4; break; /* GTE_SPRITE_8X8 */
    }
}

void moveSpriteCentered(W_Image *image, int cx, int cy) {
    int ox, oy;
    spriteCenterOffset(image->spriteSize, &ox, &oy);
    GTEMoveSprite(image->spriteId, cx - ox, cy - oy);
}

void replaceSprite(W_Image *image, int dir, int x, int y) {
    GTEUpdateSprite(image->spriteId,                              
                    image->tileStart + dirs[dir][0] + dirs[dir][1] + image->spriteSize,
                    vbuffs[image->vbuffIdx + dirs[dir][0]]);
    moveSpriteCentered(image, x, y);
}

void removeSprite(W_Image *image) {
    GTERemoveSprite(image->spriteId);
    sprites[image->spriteId] = -1;
}

void addSprite(W_Image *image, int x, int y) {
    image->spriteId = getSpriteID();
    GTEAddSprite(image->spriteId, 
                 image->spriteSize + image->tileStart, 
                 vbuffs[image->vbuffIdx], x, y);
}

void addSpriteCentered(W_Image *image, int cx, int cy) {
    int ox, oy;
    spriteCenterOffset(image->spriteSize, &ox, &oy);
    image->spriteId = getSpriteID();
    GTEAddSprite(image->spriteId,
                 image->spriteSize + image->tileStart,
                 vbuffs[image->vbuffIdx],
                 cx - ox,
                 cy - oy);
}
    
W_Image *getImage(int offset) {
    W_Image *image = NULL;
    if (offset >= 0 && offset < I_LAST) {
        image = &imagearray[offset];
        if (image->mksprite) {
            addSprite(image, -20, -20);
            //only create a image sprite once
            image->mksprite = 0;
        }
    }
    return image;
}

int cmpfilenames (const void *left, const char*right){
    return strcmp((char *) left, ((W_Image *) right)->filename);
}



