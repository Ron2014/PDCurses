#include "UnitTest++/src/UnitTest++.h"
#include <curses.h>

TEST(two_win) {
	initscr();			/* Start curses mode 		*/
	cbreak();			/* Line buffering disabled, Pass on
					 * everty thing to me 		*/
    noecho();

	if(has_colors() == FALSE)
	{	endwin();
		printf("Your terminal does not support color\n");
		exit(1);
	}
	start_color();			/* Start color 			*/
	init_pair(1, COLOR_RED, COLOR_BLACK);
    
    refresh();

    WINDOW *w0 = newwin(10, 10, 0, 0);
    box(w0, 0, 0);

    keypad(w0, TRUE);
    wtimeout(w0, 300);

    wattron(w0, COLOR_PAIR(1));
    mvwprintw(w0, 1, 1, "hhh");
	wattroff(w0, COLOR_PAIR(1));

    WINDOW *w1 = newwin(10, 10, 0, 30);
    box(w1, 0, 0);

    int ch;
    while((ch = wgetch(w0)) != KEY_F(1)) {
        wrefresh(w0);
        wrefresh(w1);
    }

	endwin();			/* End curses mode		  */
}