#include <string.h>
#include <stdbool.h>

/* main.c */
void xgal_exit(int v);
int main(int argc, char **argv);
/* images.c */
W_Image *getImage(int offset);
int cmpfilenames(const void *left, const void *right);
void hideSprite(W_Image *image);
void initVbufs(void);
void addSprite(W_Image *image, int x, int y);
void addSpriteCentered(W_Image *image, int cx, int cy);
void replaceSprite(W_Image *image, int dir, int x, int y);
void moveSpriteCentered(W_Image *image, int cx, int cy);
/* data.c */
/* paths.c */
void path_dir(int path, int pos, int *dir, int *steer);
void enter_path_dir(int path, int pos, int *dir, int *steer);
void start_path(int path, struct alien *al);
void new_alien(int level, int i, struct alien *al);
/* title.c */
void undo_pause(void);
void do_pause(void);
void center_text(char *text, int y);
void draw_text(char *text, int x, int y);
void clearScreen(int start, int end);
void do_title(void);
/* explosions.c */
bool any_bubbles(void);
bool any_explosions(void);
void undo_explosions(void);
void do_explosions(void);
void new_explosion(W_Image *image, int x, int y, int type);
void score_flagship(int x, int y, int ne);
void init_explosions(void);
/* score.c */
void do_score(void);
void draw_score(void);
void init_score(void);
/* highscore.c */
void do_name(void);
void add_score(char *name, int score);
int score_key(unsigned int ev);
int check_score(int score);
void show_scores(int *line);
void load_scores(void);
/* prize.c */
void init_prizes(void);
bool any_prizes(void);
void new_prize(int x, int y);
void undo_prizes(void);
void do_prizes(void);
/* sound.c */
void play_sound(int k);
void maybe_play_sound(int k);
void sound_completed(int k);
void kill_sound(void);
/* pathfile.c */
int get_path(int);
void get_xy(int, int*, int*);
int get_delay(int);
int get_dir(int, int);
int get_duration(int, int);
int get_shape(int);
int read_level(int);
/*warp.c*/
void init_warp(void);
int do_warp(void);
