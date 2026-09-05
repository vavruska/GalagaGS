#ifndef STRUCT_H
#define STRUCT_H


typedef struct W_Image {
    int width; 
    int height; 
    int frames; 
    int spriteId; 
    char filename[20];
    int tileStart; 
    int sequences; 
    int mksprite; 
    int vbuffIdx; 
    int spriteSize; 
} W_Image;

struct torp {
    struct torp *next, *prev;
    int alive, frame;
    int x, y;
    int xspeed, yspeed;
    W_Image shape;
};

struct star {
    int x, y;
    int tileId;
    int frames;
    int speed;
};

struct alien {
    int x, y;
    int dir;
    int steer;
    int alive;
    int dying;
    int path, path_pos;
    int escorting;
    int entering, enterdelay;
    W_Image shape;
    int valid;
    int hitW2, hitH2;   /* cached shape.width/2, shape.height/2 for collision */
};

struct explosion {
    struct explosion *next, *prev;
    int x, y;
    int frame;
    int spriteId;
    W_Image *image;
};

struct score_bubble {
    struct score_bubble *next, *prev;
    int x, y;
    int count;
    W_Image *shape;
};

struct js_state {
    signed short but;
    signed long dir;
};

#endif
