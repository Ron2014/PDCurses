/************************************************************************ 
 * This file is part of PDCurses. PDCurses is public domain software;	*
 * you may use it for any purpose. This software is provided AS IS with	*
 * NO WARRANTY whatsoever.						*
 *									*
 * If you use PDCurses in an application, an acknowledgement would be	*
 * appreciated, but is not mandatory. If you make corrections or	*
 * enhancements to PDCurses, please forward them to the current		*
 * maintainer for the benefit of other users.				*
 *									*
 * No distribution of modified PDCurses code may be made under the name	*
 * "PDCurses", except by the current maintainer. (Although PDCurses is	*
 * public domain, the name is a trademark.)				*
 *									*
 * See the file maintain.er for details of the current maintainer.	*
 ************************************************************************/

#define CURSES_LIBRARY 1
#define INCLUDE_WINDOWS_H
#include <curses.h>
#include <stdlib.h>
#include <string.h>

extern HANDLE hConOut;
extern unsigned char atrtab[MAX_ATRTAB];

#ifdef PDCDEBUG
const char *rcsid_PDCdisp =
	"$Id: pdcdisp.c,v 1.20 2006/03/27 14:33:00 wmcbrine Exp $";
#endif

/*man-start**************************************************************

  PDC_cursor_on()    - Turns on the hardware cursor.

  PDCurses Description:
	Turns on the hardware cursor; does nothing if it is already on.

  PDCurses Return Value:
	Returns OK upon success, ERR upon failure.

  Portability:
	PDCurses  int PDC_cursor_on(void);

**man-end****************************************************************/

int PDC_cursor_on(void)
{
	CONSOLE_CURSOR_INFO cci;

	PDC_LOG(("PDC_cursor_on() - called\n"));

	if (!SP->visible_cursor)
	{
		SP->visible_cursor = TRUE;
		GetConsoleCursorInfo(hConOut, &cci);
		cci.bVisible = TRUE;
		SetConsoleCursorInfo(hConOut, &cci);
	}

	return OK;
}

/*man-start**************************************************************

  PDC_cursor_off()   - Turns off the hardware cursor.

  PDCurses Description:
	Turns off the hardware cursor; does nothing if it is already off.

  PDCurses Return Value:
	Returns OK upon success, ERR upon failure.

  PDCurses Errors:
	ERR will be returned if the hardware cursor can not be disabled.

  Portability:
	PDCurses  int PDC_cursor_off(void);

**man-end****************************************************************/

int PDC_cursor_off(void)
{
	CONSOLE_CURSOR_INFO cci;

	PDC_LOG(("PDC_cursor_off() - called\n"));

	if (SP->visible_cursor)
	{
		SP->visible_cursor = FALSE;
		GetConsoleCursorInfo(hConOut, &cci);
		cci.bVisible = FALSE;
		SetConsoleCursorInfo(hConOut, &cci);
	}

	return OK;
}

/*man-start**************************************************************

  PDC_fix_cursor()   - Fix the cursor start and stop scan lines
		       (if necessary)

  PDCurses Description:
	This is a private PDCurses routine.

	This routine will fix the cursor shape for certain video 
	adapters. Normally, the values used are correct, but some 
	adapters choke. The most noticable choke is on a monochrome 
	adapter.  The "correct" scan lines will result in the cursor 
	being set in the middle of the character cell, rather than at 
	the bottom.

	The passed flag indicates whether the cursor is visible or not.

	This only applies to the DOS platform.

  PDCurses Return Value:
	This function returns OK on success and ERR on error.

  PDCurses Errors:
	No errors are defined for this function.

  Portability:
	PDCurses  int PDC_fix_cursor(int flag);

**man-end****************************************************************/

int PDC_fix_cursor(int flag)
{
	return OK;
}

/*man-start**************************************************************

  PDC_gotoxy() - position hardware cursor at (x, y)

  PDCurses Description:
	This is a private PDCurses routine.

	Moves the physical cursor to the desired address on the screen. 
	We don't optimize here -- on a PC, it takes more time to 
	optimize than to do things directly.

  PDCurses Return Value:
	This function returns OK on success and ERR on error.

  PDCurses Errors:
	No errors are defined for this function.

  Portability:
	PDCurses  int PDC_gotoxy(int row, int col);

**man-end****************************************************************/

int PDC_gotoxy(int row, int col)
{
	COORD coord;

	PDC_LOG(("PDC_gotoxy() - called: row %d col %d from row %d col %d\n",
		row, col, SP->cursrow, SP->curscol));

	coord.X = col;
	coord.Y = row;

	SetConsoleCursorPosition(hConOut, coord);

	return OK;
}

/*man-start**************************************************************

  PDC_putc()   - Output a character in the current attribute.

  PDCurses Description:
	This is a private PDCurses routine.

	Outputs character 'chr' to screen in tty fashion. If a colour
	mode is active, the character is written with colour 'colour'.

  PDCurses Return Value:
	This function returns OK on success and ERR on error.

  PDCurses Errors:
	No errors are defined for this function.

  Portability:
	PDCurses  int PDC_putc(chtype character, chtype color);

**man-end****************************************************************/

int PDC_putc(chtype character, chtype color)
{
	int curRow, curCol;
	WORD buffer[2];
	COORD coord;
	DWORD written;

	PDC_LOG(("PDC_putc() - called:char=%c attrib=0x%x color=0x%x\n",
		character & A_CHARTEXT, character & A_ATTRIBUTES, color));

	PDC_get_cursor_pos(&curRow, &curCol);

	coord.X = curCol;
	coord.Y = curRow;
#if 0
	buffer[0] = color;
	WriteConsoleOutputAttribute(hConOut, &buffer, 1, coord, &written);
#endif
	buffer[0] = character;
	WriteConsoleOutputCharacter(hConOut, (char*)&buffer[0], 1, 
		coord, &written);

	return OK;
}

/*man-start**************************************************************

  PDC_putctty()   - Output a character and attribute in TTY fashion.

  PDCurses Description:
	This is a private PDCurses routine.

	Outputs character 'chr' to screen in tty fashion. If a colour
	mode is active, the character is written with colour 'colour'.

	This function moves the physical cursor after writing so the
	screen will scroll if necessary.

  PDCurses Return Value:
	This function returns OK on success and ERR on error.

  PDCurses Errors:
	No errors are defined for this function.

  Portability:
	PDCurses  int PDC_putctty(chtype character, chtype color);

**man-end****************************************************************/

int PDC_putctty(chtype character, chtype color)
{
	int curRow, curCol;
	WORD buffer;
	COORD coord;
	DWORD written;

	PDC_LOG(("PDC_putctty() - called\n"));

	PDC_get_cursor_pos(&curRow, &curCol);

	coord.X = curCol;
	coord.Y = curRow;
#if 0
	buffer = color;
	WriteConsoleOutputAttribute(hConOut, &buffer, 1, coord, &written);
#endif
	buffer = character;
	WriteConsoleOutputCharacter(hConOut, (char*)&buffer, 1,
		coord, &written);

	return OK;
}

/*man-start**************************************************************

  PDC_scroll() - low level screen scroll

  PDCurses Description:
	Scrolls a window in the current page up or down. Urow, lcol,
	lrow, rcol are the window coordinates. Lines is the number of
	lines to scroll. If 0, clears the window, if < 0 scrolls down,
	if > 0 scrolls up.  Blanks areas that are left, and sets
	character attributes to attr. If in a colour graphics mode,
	fills them with the colour 'attr' instead.

  PDCurses Return Value:
	The PDC_scroll() function returns OK on success otherwise ERR is 
	returned.

  Portability:
	PDCurses  int PDC_scroll(int urow, int lcol, int rcol,
				 int nlines, chtype attr);

**man-end****************************************************************/

int PDC_scroll(int urow, int lcol, int lrow, int rcol, int nlines, chtype attr)
{
	PDC_LOG(("PDC_scroll() - called: urow %d lcol %d lrow %d "
		"rcol %d nlines %d\n", urow, lcol, lrow, rcol, nlines));

	return OK;
}

/*man-start**************************************************************

  PDC_transform_line()  - display a physical line of the screen

  PDCurses Description:
	This is a private PDCurses function.

	Updates the given physical line to look like the corresponding
	line in _curscr.

  PDCurses Errors:
	No errors are defined for this routine.

  Portability:
	PDCurses  void PDC_transform_line(int lineno);

**man-end****************************************************************/

void PDC_transform_line(int lineno)
{
	CHAR_INFO ci[512];
	int j, x, endx, len;
	chtype *srcp;
	COORD bufSize, bufPos;
	SMALL_RECT sr;

	PDC_LOG(("PDC_transform_line() - called: lineno=%d\n", lineno));

	if (curscr == (WINDOW *)NULL)
		return;

	x = curscr->_firstch[lineno];
	endx = curscr->_lastch[lineno];
	srcp = curscr->_y[lineno] + x;
	len = endx - x + 1;

	bufPos.X = bufPos.Y = 0;

	bufSize.X = len;
	bufSize.Y = 1;

	sr.Top = lineno;
	sr.Bottom = lineno;
	sr.Left = x;
	sr.Right = endx;

	for (j = 0; j < len; j++)
	{
		ci[j].Char.AsciiChar = srcp[j] & A_CHARTEXT;
		ci[j].Attributes = (chtype_attr(srcp[j]) & 0xFF00) >> 8;
	}

	WriteConsoleOutput(hConOut, ci, bufSize, bufPos, &sr);

	curscr->_firstch[lineno] = _NO_CHANGE;
	curscr->_lastch[lineno] = _NO_CHANGE;
}
