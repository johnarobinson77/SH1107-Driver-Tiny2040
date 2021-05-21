/**
 * Copyright (c) 2021 John Robinson.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"
#include "pixel_ops.h"

// PIXEL DATA
// The display pixel buffer is is a local copy of the display buffer
// in the SH1107.  All drawing commands make changes to display_pixel array
// and then call srn_refresh to copy the holw buffer out to the SH1107.

extern uint8_t srn_display_pixels[16][128];


// Clears a region of the display bounded by min X, min Y, max X, max Y inclusive.
// Returns false if bound are ouside the the bound are outside the display limits of
// 0 to 127.  in that case no operation is performed
static inline bool clear_display(int minX, int minY, int maxX, int maxY) {
  if (minX < 0    || minY <    0 ||
      maxX < minX || maxY < minY ||
      maxX > 127  || maxY > 127) return false;
  int row_part = minY & 0x7;
  int row = minY >> 3;
  int last_row = maxY >> 3;
  int last_row_part = maxY & 0x7;
  if (row == last_row || row_part != 0) {// top &  possible partial row
    uint8_t row_mask = 0xFF << row_part;
    if (row == last_row) {
      row_mask &= 0xFF >> (7 - last_row_part);
    }
    for (int i = minX; i <= maxX; i++) {
      srn_display_pixels[row][i] &= ~row_mask;
    }
    row += 1;
  }
  for (; row < last_row; row++) { // full rows
    for (int i = minX; i <= maxX; i++) {
      srn_display_pixels[row][i] = 0;
    }
  }
  if (row == last_row) {// bottom partial row
    uint8_t row_mask = 0xFF >> (7 - last_row_part);
    for (int i = minX; i <= maxX; i++) {
      srn_display_pixels[row][i] &= ~row_mask;
    }
  }
  return true;
}

				   
bool set_screen_region(screen_region_t *this, int xmin, int ymin, int xmax, int ymax) {
  if (xmin < 0 || xmin > 127 ||
      xmax < xmin || xmax > 127 ||
      ymin < 0 || ymin > 127 ||
      ymax < ymin || ymax > 127 ) return false;
  this->xMin = xmin;
  this->yMin = ymin;
  this->xMax = xmax;
  this->yMax = ymax;
  return true;
}

bool clear_screen_region(screen_region_t *this) {
  return clear_display(this->xMin, this->yMin, this->xMax, this->yMax);
}


void scroll_screen_region(screen_region_t *this, int xStep, int yStep){
  if (xStep > 0) { // shift pixels left
    int row_part = this->yMin & 0x7;
    int row = this->yMin >> 3;
    int last_row = this->yMax >> 3;
    int last_row_part = this->yMax & 0x7;
    if (row == last_row || row_part != 0) {// top &  possible partial row
      uint8_t row_mask = 0xFF << row_part;
      if (row == last_row) {
	row_mask &= 0xFF >> (7 - last_row_part);
      }
      for (int i = this->xMin; i <= this->xMax - xStep; i++) {
	      srn_display_pixels[row][i] &= ~row_mask;
	      srn_display_pixels[row][i] |= srn_display_pixels[row][i+xStep] & row_mask; 
      }
      for (int i = this->xMax - xStep + 1; i <= this->xMax; i++) {
	      srn_display_pixels[row][i] &= ~row_mask;
      }
      row += 1;
    }
    for (; row < last_row; row++) { // full rows
      for (int i = this->xMin; i <= this->xMax - xStep; i++) {
	      srn_display_pixels[row][i] = srn_display_pixels[row][i+xStep]; 
      }
      for (int i = this->xMax - xStep + 1; i <= this->xMax; i++) {
	      srn_display_pixels[row][i] = 0;
      }
    }
    if (row == last_row) {// bottom partial row
      uint8_t row_mask = 0xFF >> (7 - last_row_part);
      for (int i = this->xMin; i <= this->xMax - xStep; i++) {
	      srn_display_pixels[row][i] &= ~row_mask;
	      srn_display_pixels[row][i] |= srn_display_pixels[row][i+xStep] & row_mask; 
      }
      for (int i = this->xMax - xStep + 1; i <= this->xMax; i++) {
	      srn_display_pixels[row][i] &= ~row_mask;
      }
    }
  }

  //*****************************************
  
  if (yStep > 0) { // scroll up
    if (yStep > this->yMax - this->yMin +1 ) {
      yStep = this->yMax - this->yMin +1;
    }
    int row = this->yMin >> 3;
    int row_part = this->yMin & 0x7;
    int last_row = this->yMax >> 3;
    int last_row_part = this->yMax & 0x7;
    int yStep_row = yStep >> 3;
    int yStep_part = yStep & 7;
    uint8_t row_mask = 0xFF << row_part;
    if (last_row == row) {
      row_mask &= 0xFF >> (7 - last_row_part);
    }
    for (int i = this->xMin; i <= this->xMax; i++) {
      uint8_t new_val  = srn_display_pixels[row+yStep_row  ][i] >> yStep_part;
      if (row+yStep_row+1 < 16)
	      new_val |= srn_display_pixels[row+yStep_row+1][i] << (8 - yStep_part);
      srn_display_pixels[row][i] &= ~row_mask;
      srn_display_pixels[row][i] |= new_val & row_mask;
    }
    row += 1;
    for (; row < last_row - yStep_row; row++) {
      for (int i = this->xMin; i <= this->xMax; i++) {
      	uint8_t new_val  = srn_display_pixels[row+yStep_row  ][i] >> yStep_part;
      	if (row+yStep_row+1 < 16)
	        new_val |= srn_display_pixels[row+yStep_row+1][i] << (8 - yStep_part);
	      srn_display_pixels[row][i] = new_val;
      }
    }
    if (row == last_row - yStep_row) {
      row_mask = 0xFF >> (7 - last_row_part);
      for (int i = this->xMin; i <= this->xMax; i++) {
        uint8_t new_val =  srn_display_pixels[row+yStep_row  ][i] >> yStep_part;
        if (row+yStep_row+1 < 16)
	        new_val |= srn_display_pixels[row+yStep_row+1][i] << (8 - yStep_part);
	      srn_display_pixels[row][i] &= ~row_mask;
	      srn_display_pixels[row][i] |= new_val & row_mask;
      }
    }
    clear_display(this->xMin, this->yMax - yStep + 1, this->xMax, this->yMax);
  }  

  //*****************************************  

  if (xStep < 0) { // shift pixels left
    int row_part = this->yMin & 0x7;
    int row = this->yMin >> 3;
    int last_row = this->yMax >> 3;
    int last_row_part = this->yMax & 0x7;
    if (row == last_row || row_part != 0) {// top &  possible partial row
      uint8_t row_mask = 0xFF << row_part;
      if (row == last_row) {
	row_mask &= 0xFF >> (7 - last_row_part);
      }
      for (int i = this->xMax; i >= this->xMin - xStep; i--) {
	      srn_display_pixels[row][i] &= ~row_mask;
	      srn_display_pixels[row][i] |= srn_display_pixels[row][i+xStep] & row_mask; 
      }
      for (int i = this->xMin - xStep - 1; i >= this->xMin; i--) {
      	srn_display_pixels[row][i] &= ~row_mask;
      }
      row += 1;
    }
    for (; row < last_row; row++) { // full rows
      for (int i = this->xMax; i >= this->xMin - xStep; i--) {
	      srn_display_pixels[row][i] = srn_display_pixels[row][i+xStep]; 
      }
      for (int i = this->xMin - xStep - 1; i >= this->xMin; i--) {
	      srn_display_pixels[row][i] = 0;
      }
    }
    if (row == last_row) {// bottom partial row
      uint8_t row_mask = 0xFF >> (7 - last_row_part);
      for (int i = this->xMax; i >= this->xMin - xStep; i--) {
	      srn_display_pixels[row][i] &= ~row_mask;
	      srn_display_pixels[row][i] |= srn_display_pixels[row][i+xStep] & row_mask; 
      }
      for (int i = this->xMin - xStep - 1; i >= this->xMin; i--) {
	      srn_display_pixels[row][i] &= ~row_mask;
      }
    }
  }

  //*****************************************
  
  if (yStep < 0) { // scroll down
    yStep = -yStep;
    if (yStep > this->yMax - this->yMin +1 ) {
      yStep = this->yMax - this->yMin +1;
    }
    int row = this->yMax >> 3;
    int row_part = this->yMax & 0x7;
    int last_row = this->yMin >> 3;
    int last_row_part = this->yMin & 0x7;
    int yStep_row = yStep >> 3;
    int yStep_part = yStep & 7;
    uint8_t row_mask = 0xFF >> (7 - row_part);
    if (last_row == row) {
      row_mask &= 0xFF << last_row_part;
    }
    for (int i = this->xMin; i <= this->xMax; i++) {
      uint8_t new_val  = srn_display_pixels[row-yStep_row  ][i] << yStep_part;
      if (row+yStep_row > 0)
	      new_val |= srn_display_pixels[row-yStep_row-1][i] >> (8 - yStep_part);
      srn_display_pixels[row][i] &= ~row_mask;
      srn_display_pixels[row][i] |= new_val & row_mask;
    }
    row -= 1;
    for (; row > last_row + yStep_row; row--) {
      for (int i = this->xMin; i <= this->xMax; i++) {
	      uint8_t new_val  = srn_display_pixels[row-yStep_row  ][i] << yStep_part;
	    if (row+yStep_row > 0)
	        new_val |= srn_display_pixels[row-yStep_row-1][i] >> (8 - yStep_part);
	    srn_display_pixels[row][i] = new_val;
      }
    }
    if (row == last_row - yStep_row) {
      row_mask = 0xFF << last_row_part;
      for (int i = this->xMin; i <= this->xMax; i++) {
      uint8_t new_val  = srn_display_pixels[row-yStep_row  ][i] << yStep_part;
      if (row+yStep_row > 0)
	      new_val |= srn_display_pixels[row-yStep_row-1][i] >> (8 - yStep_part);
	    srn_display_pixels[row][i] &= ~row_mask;
	    srn_display_pixels[row][i] |= new_val & row_mask;
      }
    }
    clear_display(this->xMin, this->yMin, this->xMax, this->yMin + yStep - 1);
  }  
    
}

