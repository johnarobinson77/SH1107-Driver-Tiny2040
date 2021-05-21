/**
 * Copyright (c) 2021 John Robinson.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "pixel_ops.h"
#include "draw_char.h"
#include "draw_graphics.h"
#include "blink.h"
 
#define PIXEL_SCROLL_TEST
#define CHAR_TEST
#define BOX_TEST
#define COMBINED_TEST

/* sh1107_test.c
 * This is the main() for the test of the sh1107 driver code.  There
 * are several tests ebeded here but the main one is the sin test that 
 * pues two text regions and one autoscrol graphic region at the op
 * and is the best example of how to use the code.
 */

// randomize randomizes the pixels on the screen.  Used as part of the scroll test
void randomize() {
  int upper = 255;
  int lower = 0;
  for (int r = 0;  r < 16; r++) {
    for(int c = 0; c < 128; c++) {
      srn_display_pixels[r][c] = (rand() % (upper - lower + 1)) + lower;
      srn_display_pixels[r][c] |= 0x1;
    }
  }
  srn_refresh();
}


int main() {
  // standrd init call for RP2040
  stdio_init_all();
  // this init sets up the SPI interface to the display and ends with a clear screen
  init_sh1107_SPI();
  // this inits the GPIO that drave the RGB LED on the tiny2040 board and is not 
  // required for the display.  Only here for code debug purposes
  init_tiny2040_leds();
  // set the LED to cyan (green + blue)
  set_leds(false, true, true);

   // create a table of values 
  float sint[64][2];
  for (int i=0;  i<64; i++) {
    float t = (float)i/64.0;
    float r = t * 2.0 * M_PI;
    //sint[i][1] = t < .5 ? 4.0*t-1.0 : -4.0*t + 3.0 ;
    sint[i][1] = sin(r);
    sint[i][0] = t * 4.0 - 2.0;
  }

  // these two variables hold time values
  static uint64_t t1;
  static uint64_t t2;

  // two character screen regions are used for various tests
  char_screen_region_t csr1;
  char_screen_region_t csr2;
  // two graphics screen regions are used for various
  graph_screen_region_t gsr;
  graph_screen_region_t gsras;

  // test loop counter
  int l = 0;

while (1) {

 
#ifdef PIXEL_SCROLL_TEST
 screen_region_t sr;
 for (l = 0;  l < 800; l++) {
    if (0 == l%100) { 
      randomize();
      int n = ((l / 100) % 50);
      set_screen_region(&sr, 8, 8, 119, n+8);
      set_leds(false, false, true);
    } else if (50 == l%100) {
      int n = ((l / 100) % 50) + 1;
      set_screen_region(&sr, 8, 8+n, 119, 50+8);
      set_leds(false, true, false);
    }
    scroll_screen_region(&sr, 1, 0);
    srn_refresh();
 }
 for (l = 0;  l < 800; l++) {
    if (0 == l%100) { 
      randomize();
      int n = ((l / 100) % 50);
      set_screen_region(&sr, 8, 8, 8+57, 8+n);
      set_leds(false, false, true);
    } else if (50 == l%100) {
      int n = ((l / 100) % 50) + 1;
      set_screen_region(&sr, 8+58, 8+n, 119, 50+8);
      set_leds(false, true, false);
    }
    scroll_screen_region(&sr, 0, 1);
    srn_refresh();
 }
 for (l = 0;  l < 800; l++) {
    if (0 == l%100) { 
      randomize();
      int n = ((l / 100) % 50);
      set_screen_region(&sr, 8, 8, 119, n+8);
      set_leds(false, false, true);
    } else if (50 == l%100) {
      int n = ((l / 100) % 50) + 1;
      set_screen_region(&sr, 8, 8+n, 119, 50+8);
      set_leds(false, true, false);
    }
    scroll_screen_region(&sr, -1, 0);
    srn_refresh();
 }
 for (l = 0;  l < 800; l++) {
    if (0 == l%100) { 
      randomize();
      int n = ((l / 100) % 50);
      set_screen_region(&sr, 8, 8, 8+57, 8+n);
      set_leds(false, false, true);
    } else if (50 == l%100) {
      int n = ((l / 100) % 50) + 1;
      set_screen_region(&sr, 8+58, 8+n, 119, 50+8);
      set_leds(false, true, false);
    }
    scroll_screen_region(&sr, 0,-1);
    srn_refresh();
 }
#endif


#ifdef CHAR_TEST
  // set up the screen with two regions and test the vertical 
  // chracter scroll.  Note that the character regions are 
  // defined at the resolution of 8x8 pixel characters.  For
  // the 1107, that is 16 x 16 characters, 0,0 is the left top,
  // 15,15 is the right bottom of the screen
    init_char_screen_region(&csr1, 1, 1, 15, 8);
    init_char_screen_region(&csr2, 4, 9, 15, 15);
    
    char test[] = "Line";
    srn_fast_clear();  // clear the whole screen
    l = 0;
    while (l < 64) {
      char pl[256];
      // measure the time to print a line to the screen
      t1 = to_us_since_boot(get_absolute_time());
      sprintf(pl, "\n%s %d",test, l);
      srn_print(&csr1, pl);
      srn_refresh();
      t2 = to_us_since_boot(get_absolute_time());
      start_blinking(true, false, false, 1);
      sprintf(pl, "ref: %d\n", t2 - t1);
      srn_print(&csr2, pl);
      if (l % 16 == 15) {
        srn_refresh();
        start_blinking(true, true, true, 5);      
        clear_text(&csr1);
      }
      l++;
    }
    // scroll down test
    srn_fast_clear();  // clear the whole screen
    l = 0;
    while (l < 64) {
      char pl[256];
      // measure the time to print a line to the screen
      t1 = to_us_since_boot(get_absolute_time());
      sprintf(pl, "%s %d",test, l);
      scroll_text(&csr1,-1);
      start_char_at(&csr1,0,0);
      srn_print(&csr1, pl);
      srn_refresh();
      t2 = to_us_since_boot(get_absolute_time());
      start_blinking(true, false, false, 1);
      sprintf(pl, "ref: %d\n", t2 - t1);
      srn_print(&csr2, pl);
      if (l % 16 == 15) {
        srn_refresh();
        start_blinking(true, true, true, 5);      
        clear_text(&csr1);
      }
      l++;
    }
#endif
    
#ifdef BOX_TEST
  // this test draws linse at various angles  to test the algorithm.
    float bl = -0.9;
    float br =  0.9;
    float bt =  0.9;
    float bb = -0.9;
    start_blinking(false, true, false, 2);
    
    srn_fast_clear();  // clear the whole screen
    // map a screen region on the top half of the screen.
    map_window(&gsr, -2.0, 1.0, 2.0, -1.0, 0, 0, 127, 63);
    l = 0;
    while (l < 10){
      clear_window(&gsr);
      draw_line (&gsr, (br-bl)*0.00+bl, bt, (br-bl)*1.00+bl, bb);
      draw_point  (&gsr, (br-bl)*0.125+bl, bt);
      draw_line (&gsr, (br-bl)*0.25+bl, bt, (br-bl)*0.75+bl, bb);
      draw_point  (&gsr, (br-bl)*0.375+bl, bt);
      draw_line (&gsr, (br-bl)*0.50+bl, bt, (br-bl)*0.50+bl, bb);
      draw_point  (&gsr, (br-bl)*0.625+bl, bt);
      draw_line (&gsr, (br-bl)*0.75+bl, bt, (br-bl)*0.25+bl, bb);
      draw_point  (&gsr, (br-bl)*0.875+bl, bt);
      draw_line (&gsr, (br-bl)*1.00+bl, bt, (br-bl)*0.00+bl, bb);
      srn_refresh();
      start_blinking(true, false, false, 4);
      draw_line (&gsr, bl, (bb-bt)*0.00+bt, br, (bb-bt)*1.00+bt);
      draw_point  (&gsr, bl, (bb-bt)*0.125+bt);
      draw_line (&gsr, bl, (bb-bt)*0.25+bt, br, (bb-bt)*0.75+bt);
      draw_point  (&gsr, bl, (bb-bt)*0.375+bt);
      draw_line (&gsr, bl, (bb-bt)*0.50+bt, br, (bb-bt)*0.50+bt);
      draw_point  (&gsr, bl, (bb-bt)*0.625+bt);
      draw_line (&gsr, bl, (bb-bt)*0.75+bt, br, (bb-bt)*0.25+bt);
      draw_point  (&gsr, bl, (bb-bt)*0.875+bt);
      draw_line (&gsr, bl, (bb-bt)*1.00+bt, br, (bb-bt)*0.00+bt);
      srn_refresh();
      start_blinking(true, true, false, 4);
      clear_window(&gsr);
      draw_line (&gsr, (br-bl)*0.00+bl, bb, (br-bl)*1.00+bl, bt);
      draw_point (&gsr, (br-bl)*0.125+bl, bb);
      draw_line (&gsr, (br-bl)*0.25+bl, bb, (br-bl)*0.75+bl, bt);
      draw_point (&gsr, (br-bl)*0.375+bl, bb);
      draw_line (&gsr, (br-bl)*0.50+bl, bb, (br-bl)*0.50+bl, bt);
      draw_point (&gsr, (br-bl)*0.625+bl, bb);
      draw_line (&gsr, (br-bl)*0.75+bl, bb, (br-bl)*0.25+bl, bt);
      draw_point (&gsr, (br-bl)*0.875+bl, bb);
      draw_line (&gsr, (br-bl)*1.00+bl, bb, (br-bl)*0.00+bl, bt);
      srn_refresh();
      start_blinking(false, true, false, 4);
      draw_line (&gsr, br, (bb-bt)*0.00+bt, bl, (bb-bt)*1.00+bt);
      draw_point (&gsr, br, (bb-bt)*0.125+bt);
      draw_line (&gsr, br, (bb-bt)*0.25+bt, bl, (bb-bt)*0.75+bt);
      draw_point (&gsr, br, (bb-bt)*0.375+bt);
      draw_line (&gsr, br, (bb-bt)*0.50+bt, bl, (bb-bt)*0.50+bt);
      draw_point (&gsr, br, (bb-bt)*0.625+bt);
      draw_line (&gsr, br, (bb-bt)*0.75+bt, bl, (bb-bt)*0.25+bt);
      draw_point (&gsr, br, (bb-bt)*0.875+bt);
      draw_line (&gsr, br, (bb-bt)*1.00+bt, bl, (bb-bt)*0.00+bt);
      srn_refresh();
      start_blinking(false, true, true, 4);
      l++;
    }
#endif

#ifdef COMBINED_TEST
    srn_fast_clear();
    // set up two character regions side by side on the bottom of the screen.
    // left one for values, right one for srn_refresh time.
    // set up the whole text box and write the text header
    init_char_screen_region(&csr1, 0, 9,  7, 15);
    srn_print(&csr1, "Value");
    // reset the text box to exlude the header so it doesn't get cleared
    init_char_screen_region(&csr1, 0, 10,  8, 15);
    
    // set up the whole text box and write the text header
    init_char_screen_region(&csr2, 8, 9, 15, 15);
    srn_print(&csr2, "time*16");
    // reset the text box to exlude the header so it doesn't get cleared
    init_char_screen_region(&csr2, 9, 10, 15, 15);
    
    // set up a graphics region on the top half of the screen
    map_window(&gsr, -2.0, 1.0, 2.0, -1.0, 0, 0, 127, 63);
    map_autoscroll_bar_window(&gsras, 1.0, -1.0, 0, 0, 127, 63);
    srn_refresh(); 

    char val_str[32];

    t1 = to_us_since_boot(get_absolute_time());

    l = 0;
    set_leds(false, false, true);
    while(l < 500) {
      draw_next_as_line(&gsras, sint[(l) & 0x3F][1]);
      sprintf(val_str, "\n% 6.4f", sint[(l) & 0x3F][1]);
      srn_print(&csr1, val_str);
      if (0 == (l & 0xF)) {
        t2 = to_us_since_boot(get_absolute_time());
        sprintf(val_str, "\n%d", t2 - t1);
        srn_print(&csr2, val_str);
        t1 = t2;
      }
      srn_refresh();
      if (0 == (l & 0xFF)) {
        clear_window(&gsras);
      }
      l++;
    }

    l = 0;
    set_leds(false, false, true);
    while(l < 500) {
      draw_next_as_bar(&gsras, sint[(l) & 0x3F][1]);
      sprintf(val_str, "\n% 6.4f", sint[(l) & 0x3F][1]);
      srn_print(&csr1, val_str);
      if (0 == (l & 0xF)) {
        t2 = to_us_since_boot(get_absolute_time());
        sprintf(val_str, "\n%d", t2 - t1);
        srn_print(&csr2, val_str);
        t1 = t2;
      }
      srn_refresh();
      if (0 == (l & 0xFF)) {
        clear_window(&gsras);
      }
      l++;
    }
  #endif
  }
}

