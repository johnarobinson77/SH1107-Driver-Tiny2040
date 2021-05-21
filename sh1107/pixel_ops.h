/**
 * Copyright (c) 2021 John Robinson.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PIXEL_OPS_H
#define PIXEL_OPS_H

#include "sh1107_spi.h"

typedef struct screen_region{
  int xMin;
  int yMin;
  int xMax;
  int yMax;
} screen_region_t;



// Sets up a screen_region used to defin the boundaries used in many of the pixel
// operations.  
bool set_screen_region(screen_region_t *this, int xmin, int ymin, int xmax, int ymax) ;
bool clear_screen_region(screen_region_t *this);
void scroll_screen_region(screen_region_t *this, int xStep, int yStep);

static inline bool put_pixel(screen_region_t *this, int x, int y, int b) {
  if (x < this->xMin || y < this->yMin ||
      x > this->xMax || y > this->yMax ) return false;
  PUT_PIXEL(x,y,b);
  return true;
}

#endif
