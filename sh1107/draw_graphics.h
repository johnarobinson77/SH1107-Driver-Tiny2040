/**
 * Copyright (c) 2021 John Robinson.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* draw_graphics.h
 * The functions and structions in draw_graphics.h and draw_graphics.c 
 * allow drawing genral line drawing as well as scrolling bar graphs and line
 * graphs.
 */

#ifndef DRAW_GRAPHICS_H
#define DRAW_GRAPHICS_H

// Provides a drawing contect for wither general line and dot drawing or
// for the autoscrolling line and bar graphs.  This structure should always
// be initialized with either the map_window() or map_autoscroll_bar_window()
// functions.
typedef struct graph_screen_region {
  float win_lft, win_rgt;
  float win_top, win_bot;
  float xscl, xoff;
  float yscl, yoff;
  float next_x;
  float last_yVal;
  screen_region_t sr;
} graph_screen_region_t;

// clears the region of the screen mapped in graph_screen_region.  If
// the graph_screen_region is for the auto scrolling graphs, the next
// value position is set to the left side of the region.
bool clear_window(graph_screen_region_t *this);

// maps the graph floating point range in X and Y to a screen region.  The
// screen region parameters are inclusive.
// the win_* values can be any floting values.  The difine the range of
// values that the draw_line() and draw_point() fuctions will transform 
// end up in the screen region.  
bool map_window(graph_screen_region_t *this,
		float win_l, float win_t, float win_r, float win_b,
		int   pix_l, int   pix_t, int   pix_r, int   pix_b);

// Same as above but only in Y.  X floating point range of the window 
// is mapped to the same as the screen ragion x range.  This makes it
// easier to address the aoutoscroll function
bool map_autoscroll_bar_window(graph_screen_region_t *this,
    float win_t, float win_b,
		int   pix_l, int   pix_t, int   pix_r, int   pix_b);

// Draws a line in the graph_screen_region.  line endpoints should be in the 
// window range defined in the map_window function.  Portions of the line 
// outside that region will not be drawn.
void draw_line(graph_screen_region_t *this, float x1, float y1, float x2, float y2 );

// draws a point in the graph_screen_region.  The coordinate should be in the 
// window range defined in the map_window function.  dots outside that region 
// will not be drawn.
void draw_point(graph_screen_region_t *this, float x1, float y1);

// Takes a y value and plots a bar in a bar graph to the right of the last value.
// When the screen region is full, the bargraph scrolls to the right.
// Must use map_autoscroll_bar_window to initialize the screen region.
void draw_next_as_bar(graph_screen_region_t *this, float yVal);

// Takes a y value and plots a line from the last value to the next incrament to the right.
// When the screen region is full, the bargraph scrolls to the right.
// Must use map_autoscroll_bar_window to initialize the screen region.
void draw_next_as_line(graph_screen_region_t *this, float yVal);

#endif
