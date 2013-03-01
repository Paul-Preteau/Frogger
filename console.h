/**********************************************************************
  Module: console.h
  Author: Jim Young
  Date:   2013 Jan 3

  Purpose: Draw 2d images on the screen.  Also, draw main game image.

  Changes: 
    2013 Jan 3 [ Jim Young ]
      - initial version (modified from version from Jim Uhl)

  NOTES: none of these functions are re-entrant (they are not thread
	 		safe)
**********************************************************************/

#ifndef CONSOLE_H
#define CONSOLE_H

/**************** DRAWING **************************/

enum LOG_TYPE{
  TOP,
  MIDDLE_TOP,
  MIDDLE_BOT,
  BOT,
}log_types;

/*frog dimensions*/
#define FROG_HEIGHT 2
#define FROG_WIDTH 2


/*log dimensions*/
#define LOG_WIDTH 26
#define LOG_HEIGHT 4

/* directions in terms of deltas in x / y dimension */
#define LEFT -1
#define RIGHT 1
#define UP -1
#define DOWN 1

/* width required for our game*/
#define SCR_LEFT 0
#define SCR_WIDTH 80
#define SCR_RIGHT (SCR_LEFT+SCR_WIDTH-1)

/* height */
#define SCR_TOP 0
#define SCR_HEIGHT 24
#define SCR_BOTTOM (SCR_TOP+SCR_HEIGHT-1)

/* NUM_ROWS indicates how many logical rows there are in the game -
   this includes home row (goal) and start row.
   many aspects of the implementation depend on this.
*/
#define NUM_ROWS 6

/* Game row definitions for frogs and logs */
#define FROG_HOME_ROW  0
#define FROG_START_ROW (NUM_ROWS-1)

#define LOG_TOP_ROW 1
#define LOG_BOTTOM_ROW (FROG_START_ROW-1)
#define LOG_ROW_COUNT 4 

#define ROW_HEIGHT 4

/* column where numeric score lives - this should really be
   a computer variable
*/
#define SCR_COL_LIVES 42

/* Initialize curses, draw initial gamescreen. Refreshes screen to terminal. 
 Also stores the requested dimensinos of the screen and tests the terminal for the
 given dimensions.*/
extern int screen_init(int reqHeight, int reqWidth);

/* Draws 2d `image' of `height' rows, at curses coordinates `(row, col)'.
   Note: parts of the `image' falling on negative rows are not drawn; each
   row drawn is clipped on the left and right side of the game screen (note
   that `col' may be negative, indicating `image' starts to the left of the
   screen and will thus only be partially drawn. Useful for objects that are
   half off the screen  */
extern void screen_draw_image(int row, int col, char *image[], int height);

/* Clears a 2d `width'x`height' rectangle with spaces.  Upper left hand
   corner is curses coordinate `(row,col)'. */
extern void screen_clear_image(int row, int col, int width, int height);

/* Moves cursor to bottom right corner and refreshes. If this is not done,
   the curses internal buffer (that you have been drawing to) is not dumped
   to screen. */
extern void screen_refresh(void);

/* Terminates curses cleanly. */
extern void screen_fini(void);

/* Puts the given banner in the center of the screen */
void put_banner(const char *);

/* Draws the given string at the given location  */
void put_string(char *, int row, int col, int maxlen);

/* Sleeps the given number of 20ms ticks */
void sleep_ticks(int ticks);

#endif /* CONSOLE_H */
