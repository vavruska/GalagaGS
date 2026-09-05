#ifndef PATHS_H
#define PATHS_H

/* enter_paths are for new ships.  consists of starting co-ordinates, number of frames
   to wait at the start (presumably offscreen) and a path to follow.
   path -1 means this ship is not used on this level
   at end of path, ship flies into convoy position on it's own from wherever it winds up
*/

struct enter_path {
    int shape, x, y, delay, path;
};

#define P_PEELLEFT 0
#define P_PEELRIGHT 1
#define P_LOOP 2
#define P_SWOOP1 3
#define P_SWOOP2 4
#define P_ZIGZAG 5
#define P_LOOP2  6
#define P_SPIN   7
#define P_LEFTDIAG 8
#define P_RIGHTDIAG 9

#define NUMPATHS 30

#endif
