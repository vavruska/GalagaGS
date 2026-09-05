#define VERSION "1.0.0"

#define USE_RESOURCES

#define ORIGINAL_XGALAGA
/*#define IM_A_BIG_FAT_CHEATER/**/

#ifndef ORIGINAL_XGALAGA
# define ENABLE_SHIP_WRAP
# define ENABLE_SPREAD_SHOT 1 
# define ENABLE_MACHINE_GUN 1
# define DISABLE_RESET_ON_DEATH
# define ACTIVATED_SHIELD
# define SHOW_SHIELD_BAR
# define NO_PRIZE_WHILE_DEAD
#endif

/* Only 6 alien sprites are currently in the 512 tile tileset.  The level files
 * reference up to 17 shapes; when a new level is loaded the 6 sprites tiles referenced 
 * on that level are loaded into the 6 slots used by the aliens. */
#define USE_ROW_ALIEN_FALLBACK 1

#define MAX_REAL_ALIENS 6

#define MAXTORPS 10
#define MAXETORP 10
#define MINTORPS 3
//#define TORPSPEED 12
#define TORPSPEED 6

//#define ETORPSPEED 8
#define ETORPSPEED 4

#define MINSPEED 2
#define MAXSPEED 8

#define TORPDELAY 5

#define WINWIDTH  160
#define WINHEIGHT 200
#define W_Textheight 8

#define PLAYER_Y 186

#define MAXALIENS 60
//#define MAXALIENS 20

/* Post-respawn invincibility: while this many frames remain after a ship
 * spawns, it is drawn as the shield ship and nothing can kill it. */
#define SPAWNSHIELDTIME 60

//#define TURNSPEED 10
#define TURNSPEED 6

#define BONUSSHIPSCORE 50000

/* Attack movement is scaled down from the original ~400x500 field speeds
 * (moves[][] are 0-4 px/frame) to fit the 160x200 field.  Divisor 2 gives
 * ~0.5x; set to 1 to restore the original attack speed. */
#define ATTACK_MOVE_SCALE 2
#define HORT 8

#define ABS(a)  abs(a) /* (((a) < 0) ? -(a) : (a)) */

#define LEFTKEY 1
#define RIGHTKEY 2
#define FIREKEY 4

#define random() rand()

#ifdef ORIGINAL_XGALAGA
  #define NUMWEAPONS 3 
#else
 #define NUMWEAPONS 3 + ENABLE_SPREAD_SHOT + ENABLE_MACHINE_GUN
#endif
#define SINGLESHOT 0
#define DOUBLESHOT 1
#define TRIPLESHOT 2
#define SPREADSHOT 3
#define MACHINEGUN 4

#ifdef ORIGINAL_XGALAGA
# define PRIZECHANCE 30
#else
# define PRIZECHANCE 25
#endif

#define TORPCHANCE 120

#ifdef ORIGINAL_XGALAGA
# define SHIELDTIME 300
#else
# define STARTSHIELDS 0
# define MAXSHIELDS 800
# define SHIELDTIME 200
#endif

#define ALIENSHAPES 17
