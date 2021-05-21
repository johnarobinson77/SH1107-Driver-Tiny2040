/**
 * Copyright (c) 2021 John Robinson.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


/* draw_char.h
 * The the purpose of this functions and structures contained in draw_char.h
 * and draw_char.c is to set up regions of the screen into which text can be written.
 * Each region operates as an independant terminal like function.
 * The supplied font is a standard 8x8 asci font up to 128.
 */ 

#ifndef DRAW_CHAR_H
#define DRAW_CHAR_H

#include "pixel_ops.h"

// This structure holds the bounds in 8x8 characters.  When Characters are written to 
// screen the horizontal wrap and verticle scroll are kept within the bound of the
// left, right, top and bottom character positions.
// It should always be initialized with the init_char_screen_region() fuction to insure 
// consistancy.
typedef struct char_screen_region {
  // Character region bounds (inclusive)
  // must be between 0 and 15
  int ccol_lft;
  int ccol_rgt;
  int crow_top;
  int crow_bot;
  int crow;
  int ccol;
  screen_region_t sr;
} char_screen_region_t;
  
// This function is used to init the char_screen_region struct.  left, and top must 
// less than right and bottom respectively or the function will return false.  The
// screen region includes the right and bottom character positon.
bool init_char_screen_region(char_screen_region_t *this, int lft_col, int top_row,
			     int rgt_col, int bot_row);

// This fuction clears the region of the screen defined in the char_screen_region and
// sets the current character position to the top, left corner. 
void clear_text(char_screen_region_t *this);

// sets the next character postion.  The row and column are relative to the
// top left corner of the char screen region.  The character postion should
// not be greater than the char_screen_region bounds.
bool start_char_at(char_screen_region_t *this, int row, int col);

// this fuction does a verticle scroll of the test in full characters.
// positive numbers scroll up.  negative number scroll down.
void scroll_text(char_screen_region_t *this, int n);

// This function writes a character at the current posision and advences the
// character postion to the right.  If it is at the right edge of the char
// screen region the character position advances to the next line.  If at the
// bottom of the screen region, the characters in the char_screen_region scroll up.
// '\n' sets the chacter postion to the next line.
bool write_char_next(char_screen_region_t *this, uint8_t chr);

// combines the fuction of start_char_at and write_char_next()
bool write_char_at(char_screen_region_t *this, uint8_t chr, int row, int col);

// convience function that calls write_char_next() for each char in the string.
void srn_print(char_screen_region_t *this, char pstr[]);

#endif
