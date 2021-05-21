/**
 * Copyright (c) 2021 John Robinson.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "pixel_ops.h"
#include "draw_char.h"
#include "draw_graphics.h"
 
bool clear_window(graph_screen_region_t *this) {
  this->next_x = this->win_lft;
  return clear_screen_region(&this->sr);
}  

bool map_window(graph_screen_region_t *this,
		float win_l, float win_t, float win_r, float win_b,
		int   pix_l, int   pix_t, int   pix_r, int   pix_b) {
  //make sure the drawing region isinside the display 
  if (pix_l < 0 || pix_l > 127 || pix_r < 0 || pix_r > 127 |
      pix_t < 0 || pix_t > 127 || pix_b < 0 || pix_b > 127 ) return false;
  this->win_lft = win_l;
  this->win_rgt = win_r;
  this->win_top = win_t;
  this->win_bot = win_b;
  set_screen_region(&this->sr, pix_l, pix_t, pix_r, pix_b);
  this->xscl = (float)(this->sr.xMax - this->sr.xMin + 1) / (this->win_rgt - this->win_lft);
  this->yscl = (float)(this->sr.yMax - this->sr.yMin + 1) / (this->win_bot - this->win_top);
  this->xoff = this->sr.xMin - this->win_lft * this->xscl;
  this->yoff = this->sr.yMin - this->win_top * this->yscl;
  return clear_window(this);
}

void draw_line(graph_screen_region_t *this, float x1, float y1, float x2, float y2 ) {

  // transform endpoints
  x1 = x1 * this->xscl + this->xoff;
  x2 = x2 * this->xscl + this->xoff;
  y1 = y1 * this->yscl + this->yoff;
  y2 = y2 * this->yscl + this->yoff;
 
  float x,y,dx,dy,step;
  int i;
  
  dx = (x2 - x1);
  dy = (y2 - y1);
  float adx = fabs(dx);
  float ady = fabs(dy);
  
  if(adx >= ady)
    step = adx;
  else
    step = ady;
  
  dx /= step;
  dy /= step;
  
  x = x1;
  y = y1;
  
  i = 1;
  while(i <= step) {
    put_pixel(&this->sr, (int)x, (int)y, 1);
    x = x + dx;
    y = y + dy;
    i = i + 1;
  }
}

void draw_point(graph_screen_region_t *this, float x1, float y1) {

  // transform endpoints
  x1 = x1 * this->xscl + this->xoff;
  y1 = y1 * this->yscl + this->yoff;
  put_pixel(&this->sr, (int)x1, (int)y1, 1);
}

bool map_autoscroll_bar_window(graph_screen_region_t *this,
    float win_t, float win_b,
		int   pix_l, int   pix_t, int   pix_r, int   pix_b){
  //make sure the drawing region isinside the display 
  if (pix_l < 0 || pix_l > 127 || pix_r < 0 || pix_r > 127 |
      pix_t < 0 || pix_t > 127 || pix_b < 0 || pix_b > 127 ) return false;
  this->win_lft = pix_l;
  this->win_rgt = pix_r;
  this->win_top = win_t;
  this->win_bot = win_b;
  set_screen_region(&this->sr, pix_l, pix_t, pix_r, pix_b);
  this->xscl = (float)(this->sr.xMax - this->sr.xMin + 1) / (this->win_rgt - this->win_lft);
  this->yscl = (float)(this->sr.yMax - this->sr.yMin + 1) / (this->win_bot - this->win_top);
  this->xoff = this->sr.xMin - this->win_lft * this->xscl;
  this->yoff = this->sr.yMin - this->win_top * this->yscl;
  return clear_window(this);
}

void draw_next_as_bar(graph_screen_region_t *this, float yVal){
  if (this->next_x >= this->win_rgt) {
    scroll_screen_region(&this->sr, 1, 0);
    this->next_x = this->win_rgt;
  } else {
    this->next_x += 1.0;
  }
  int i_next_x = (int)this->next_x;
  int i_yval = (int)(yVal * this->yscl + this->yoff);
  for (; i_yval <= this->sr.yMax; i_yval++) {
    put_pixel(&this->sr, i_next_x, i_yval, 1);
  }
}

void draw_next_as_line(graph_screen_region_t *this, float yVal){
  if (this->next_x == this->win_lft) {  // handle the case of no last y value
    draw_point(this, this->next_x, yVal); //draw a point at first column
    this->last_yVal = yVal; // save yval for next loop
    this->next_x += 1.0;
    return; // and we're done
  } 
  if (this->next_x >= this->win_rgt) { // if at the right edge, scroll the window.
    scroll_screen_region(&this->sr, 1, 0);
    this->next_x = this->win_rgt;
  } else {
    this->next_x += 1.0;
  }
  draw_line(this, this->next_x-1.0, this->last_yVal, this->next_x, yVal );
  this->last_yVal = yVal;
}

