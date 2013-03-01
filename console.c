/**********************************************************************
  Module: console.c
  Author: Jim Young
  Date:   2013 Jan 3

  Purpose: see console.h

  Changes: 
    2013 Jan 3 [ Jim Young ]
      - initial version

  NOTES: none
**********************************************************************/

#include <curses.h>
#include <string.h>
#include <time.h>        /*for nano sleep */

#include "console.h"
int WIDTH, HEIGHT;

static char *screen[] = {
"                                   Lives:",
"/------\\          /------\\          /------\\          /------\\          /------\\",
"|      |          |      |          |      |          |      |          |      |",
"+      +----------+      +----------+      +----------+      +----------+      +",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"",
"",
"", 
"" };

static int SCREEN_ROWS = 24; /* must match the template above for proper background drawing */


/* Local functions */

static int check_screen_size(int reqHeight, int reqWidth) {

  // COLS and LINES are provided by curses
  if ( (reqHeight < SCR_HEIGHT) || (reqWidth < SCR_WIDTH)) {
    fprintf(stderr, "\n\n\rSorry, FROGGER requires a screen resolution of at least %ix%i. \n\rYou requested only %ix%i. Sorry.\n\r",SCR_WIDTH, SCR_HEIGHT, reqWidth, reqHeight);
	 return (FALSE);
  }

  if ( (reqWidth > COLS) || (reqHeight > LINES) ) {
    fprintf(stderr, "\n\n\rSorry, your window is only %ix%i. \n\rYou requested %ix%i. Sorry.\n\r", COLS, LINES, reqWidth, reqHeight);
    return (FALSE);
  }

  return(TRUE);
}

int screen_init(int height, int width) {
    int status;

    initscr();
    crmode();
    noecho();
    clear();

    HEIGHT = height;  WIDTH = width;
    status = check_screen_size(HEIGHT, WIDTH);
        
    if (status) {
		screen_draw_image(0, 0, screen, SCREEN_ROWS);
    		screen_refresh();
	  }
   
    return(status);
}

void screen_draw_image(int row, int col, char *image[], int height) {
    int i, length;
    int new_left, new_right, new_offset, new_length;

    for (i = 0; i < height; i++) {
		if (row+i < 0 || row+i >= SCR_HEIGHT)
			continue;
		length = strlen(image[i]);
		new_left  = col < 0 ? 0 : col;
		new_offset = col < 0 ? -col : 0;
		new_right = col+length > SCR_RIGHT ? SCR_RIGHT : col+length;
		new_length = new_right - new_left + 1;
		if (new_offset > length || new_left >= new_right) 
		  continue;

		if (mvaddnstr(row+i, new_left, image[i]+new_offset, new_length) == ERR)
			fprintf(stderr, "ERROR drawing to screen"); /* smarter handling is needed */
    }
}

void screen_clear_image(int row, int col, int width, int height) {
    int i, j;
      
    if (col+width > SCR_RIGHT)
	width = SCR_RIGHT-col+1;
    if (col < 0) {
	width += col; /* -= -col */
	col = 0;
    }

    if (width < 1 || col > SCR_RIGHT) /* nothing to clear */
	return;

    for (i = 0; i < height; i++) {
	if (row+height < 0 || row+height > SCR_BOTTOM)
		continue;
	move(row+i, col);
	for (j = 0; j < width; j++)
	    addch(' ');
    }
}

void screen_refresh(void) {
    move(LINES-1, COLS-1);
    refresh();
}

void screen_fini(void) {
    endwin();
}

void put_banner(const char *str) {
  int len;

  len = strlen(str);
  
  move (HEIGHT/2, (WIDTH-len)/2);
  addnstr(str, len);

  screen_refresh();
}

void put_string(char *str, int row, int col, int maxlen) {

  move(row, col);
  addnstr(str, maxlen);

}

/* setup to work in USECS, reduces risk of overflow */
#define TIME_USECS_SIZE 1000000
#define TIMESLICE_USEC 20000
#define USEC_TO_NSEC 1000  
void sleep_ticks(int ticks) {
  struct timespec rqtp;

  if (ticks <= 0)
    return;

  /* work in usecs at first */
  rqtp.tv_nsec = TIMESLICE_USEC * ticks;

  /* handle usec overflow */
  rqtp.tv_sec = rqtp.tv_nsec / TIME_USECS_SIZE;
  rqtp.tv_nsec %= TIME_USECS_SIZE;

  rqtp.tv_nsec *= USEC_TO_NSEC;  /*convert to nsecs */

  nanosleep(&rqtp, NULL);
}

