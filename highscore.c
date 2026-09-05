/* $Id: highscore.c,v 1.2 1998/04/30 05:11:55 mrogre Exp $ */
/* Copyright (c) 1998 Joe Rumsey (mrogre@mediaone.net) */
#include "copyright.h"


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>

#include "defs.h"
#include "struct.h"
#include "proto.h"
#include "data.h"

#define NUM_MY_SCORES 10
#define HISCORE_FILE "9:galscores"

static char new_name[20];
static int nnpos = 0;
static int thisplace = -1, my_thisplace = -1;

static struct high_score {
    char name[20];
    long score;
    long level;
} my_scores[NUM_MY_SCORES];

void do_name(void) {
    char buf[21];
    center_text("Great score! Enter your name:", 12);
    snprintf(buf, sizeof(buf) - 1, "%s_", new_name);
    draw_text(buf, 14, 14);
}

static void save_scores(void) {
    int i;
    FILE *hsf;
    long x;
    char my_file_name[256];
    size_t s;

    snprintf(my_file_name, sizeof(my_file_name) - 1, HISCORE_FILE);
    hsf = fopen(my_file_name, "w");
    if (hsf == NULL) {
        return;
    }
    for (i = 0; i < NUM_MY_SCORES; i++) {
        if ((s = fwrite(my_scores[i].name, sizeof(char), 20, hsf)) < 20) {
            break;
        }
        x = my_scores[i].score;
        if ((s = fwrite(&x, sizeof(long), 1, hsf)) < 1) {
            break;
        }
        x = my_scores[i].level;
        if ((s = fwrite(&x, sizeof(long), 1, hsf)) < 1) {
            break;
        }
    }
    fclose(hsf);
    return;
}

void add_score(char *name, int score) {
    int i, j; /* ,k; */

    thisplace = my_thisplace = -1;

    load_scores();

    for (i = 0; i < NUM_MY_SCORES; i++) {
        if (score > my_scores[i].score) {
            for (j = NUM_MY_SCORES - 1; j > i; j--) {
                strcpy(my_scores[j].name, my_scores[j - 1].name);
                my_scores[j].score = my_scores[j - 1].score;
                my_scores[j].level = my_scores[j - 1].level;
            }
            strcpy(my_scores[i].name, name);
            my_scores[i].score = score;
            my_scores[i].level = level;
            my_thisplace = i;
            break;
        }
    }
    save_scores();
}

int score_key(unsigned int event) {
    if (getting_name) {
        if (event & 1024) {
            switch (event & 0x7f) {
            case 13:
            case 10:
                getting_name = 0;
                add_score(new_name, score);
                title_page = 1;
                pagetimer = 300;
                break;
            case 8:
            case 127:
                if (nnpos > 0) {
                    nnpos--;
                    new_name[nnpos] = 0;
                }
                break;
            case 'u' + 128:
                nnpos = 0;
                new_name[nnpos] = 0;
                break;
            default:
                if (nnpos < 15) {
                    new_name[nnpos++] = event & 0x7f;
                    new_name[nnpos] = 0;
                }
                break;
            }
        }

        return 1;
    }
    return 0;
}

int check_score(int score) {
    int i;


    load_scores(); /* in case someone else has gotten a high score */
    for (i = 0; i < NUM_MY_SCORES; i++) {
        if (score > my_scores[i].score) return 1;
    }

    my_thisplace = -1;
    thisplace = -1;
    return 0;
}


void show_scores(int *line) {
    int i;
    char buf[60];
    int y = *line + 7;

    switch (y) {
    case 7:
        sprintf(buf, "Rank Name               Score   Level");
        center_text(buf, y);
        (*line)++;
        break;
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
        i = (*line) - 1;
        snprintf(buf, 60, "%2d.  %-15s  %7ld   %3ld", i + 1,
                my_scores[i].name, my_scores[i].score, my_scores[i].level);
        draw_text(buf, 2, y);
        (*line)++;
        break;
    case 18:
        clearScreen(18, 23);
        (*line) = -1;
        break;
    }
}

void load_scores(void) {
    int i;
    int hsf;
    char my_file_name[256];

    snprintf(my_file_name, sizeof(my_file_name) - 1, HISCORE_FILE);
    hsf = open(my_file_name, O_RDONLY);
    if (hsf < 0) {
        for (i = 0; i < NUM_MY_SCORES; i++) {
            my_scores[i].name[0] = 0;
            my_scores[i].score = 0;
            my_scores[i].level = 0;
        }
        return;
    } else {
        for (i = 0; i < NUM_MY_SCORES; i++) {
            if (read(hsf, my_scores[i].name, 20) < 20) {
                break;
            }
            my_scores[i].name[15] = 0;
            if (read(hsf, &my_scores[i].score, sizeof(long)) < sizeof(long)) {
                break;
            }
            if (read(hsf, &my_scores[i].level, sizeof(long)) < sizeof(long)) {
                break;
            }
        }
    }
    close(hsf);
    return;
}


