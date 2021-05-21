//Character Interface

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "font8x8_basic.h"
#include "draw_char.h"

void clear_text(char_screen_region_t *this) {
  clear_screen_region (&this->sr);
  this->crow = this->crow_top;
  this->ccol = this->ccol_lft;
}

bool init_char_screen_region(char_screen_region_t *this, int lft_col, int top_row,
			     int rgt_col, int bot_row) {
  if (lft_col < 0 || rgt_col > 15 ||
      top_row < 0 || bot_row > 15 ||
      rgt_col < lft_col || bot_row < top_row) return false;
  // set up the character bounds
  this->ccol_lft = lft_col;
  this->ccol_rgt = rgt_col;
  this->crow_top = top_row;
  this->crow_bot = bot_row;
  // set up the screen region.
  set_screen_region(&this->sr, lft_col*8, top_row*8, rgt_col*8+7, bot_row*8+7);
  // set the character position at the top left corner
  this->crow = top_row;
  this->ccol = lft_col;
}

bool start_char_at(char_screen_region_t *this, int row, int col) {
  if ((row + this->crow_top) > this->crow_bot ||
      (col + this->ccol_lft) > this->ccol_rgt) return false;
  this->crow = row + this->crow_top;
  this->ccol = col + this->ccol_lft;
  return true;
}

void scroll_text(char_screen_region_t *this, int n) {
  if (n > this->crow_bot || n < -this->crow_bot) { // if scroll >  region
    clear_text(this); // just clear the region and return
    return;
  }
  int r;
  if (n > 0) { // scroll up
    for (r = this->crow_top + n; r <= this->crow_bot; r++) {
      for (int c = this->sr.xMin;  c <= this->sr.xMax; c++) {
      	srn_display_pixels[r-n][c] = srn_display_pixels[r][c];
      }
    }
    for (r = r - n; r <= this->crow_bot; r++){
      for (int c = this->sr.xMin;  c <= this->sr.xMax; c++) {
	      srn_display_pixels[r][c] = 0;
      }
    }
    srn_refresh();
  } else if (n < 0) { //scroll down
    for (r = this->crow_bot + n; r >= 0; r--) {
      for (int c = this->sr.xMin;  c <= this->sr.xMax; c++) {
	      srn_display_pixels[r-n][c] = srn_display_pixels[r][c];
      }
    }
    for (r = this->crow_top; r > this->crow_top - n; r--) {
      for (int c = this->sr.xMin;  c <= this->sr.xMax; c++) {
	      srn_display_pixels[r][c] = 0;
      }
    }
    srn_refresh();
  }
}

bool write_char_next(char_screen_region_t *this, uint8_t chr) {
  if (this->crow > this->crow_bot) {
    scroll_text(this, 1);
    this->crow = this->crow_bot;
    this->ccol = this->ccol_lft;
  }
  if (this->ccol > this->ccol_rgt || chr == '\n') {
    this->crow += 1;
    this->ccol = this->ccol_lft;
    if (this->crow > this->crow_bot) {
      scroll_text(this, 1);
      this->crow = this->crow_bot;
      this->ccol = this->ccol_lft;
    }
  } else if (chr >= 0x20)  { // skip non-printable characters
    for (int i = 0;  i < 8; i++) {
      srn_display_pixels[this->crow][(this->ccol<<3)+i] = font8x8_basic[chr][i];
    }
    this->ccol += 1;
  }
  return true;
}

bool write_char_at(char_screen_region_t *this, uint8_t chr, int row, int col) {
  if (!start_char_at(this, row, col)) return false;
  write_char_next(this, chr);
}

void srn_print(char_screen_region_t *this, char pstr[]){
  for (int i = 0; i < 256; i++) {
    if (pstr[i] == 0) break;
    write_char_next(this, pstr[i]);
  }
  srn_refresh();
}
		    
