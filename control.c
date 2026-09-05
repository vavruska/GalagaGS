/* Apple IIGS mouse - relative ship control via the Misc Tools mouse API.
 * The OS services the ADB bus (the Finder works), so we read the mouse the
 * same way the OS does: InitMouse + ReadMouse, differentiated for relative
 * deltas. Button via the Event Manager.
 */
#include <stdio.h>
#include <types.h>
#include <stdbool.h>

#include "control.h"

static int mouseReady = 0;
static bool mouseDebug = 0;

extern int deltaX, deltaY, mouseBut;

extern void mousePoll(void);

void mouseInit(void) {
    FILE *f;

    mouseReady = 1;
    if (mouseDebug) {
        f = fopen("9:adbmouse.log", "w");
        if (f) {
            fprintf(f, "InitMouse done\n");
            fclose(f);
        }
    }
}

void mouseGet(int *dx, int *dy, int *but) {
    extern int deltaX, deltaY;

    *dx = 0;
    *dy = 0;
    *but = 0;

    if (!mouseReady) {
        return;
    }

    mousePoll();
    *dx = deltaX & 0x7f;
    *dy = deltaY & 0x7f;

    *but =  mouseBut;
    if (mouseDebug) {
        if (deltaX || deltaY || 1) {
            FILE *f = fopen("9:adbmouse.log", "a");
            if (f) {
                fprintf(f, "x=%x y=%x b=%d\n", deltaX, deltaY, *but);
                fclose(f);
            }
        }
    }
}
