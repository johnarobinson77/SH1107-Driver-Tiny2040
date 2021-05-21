/**
 * Copyright (c) 2021 John Robinson.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SH1107_SPI_H
#define SH1007_SPI_H

// SH1107 COMMANDS
// The next set of functions are used to send commands to the
// SH1107.  Discussions of wht these commands do can be found
// in the Commands chapter (page 23) of the SH1107 spec sheet.

void srn_set_col_page(int col, int page);
void srn_set_mem_adr_mode(int p_v);
void srn_set_contrast(int contrast);
void srn_set_seg_rot(int p_v);
void srn_turn_entire_disp_on(bool on);
void srn_set_reverse_display(bool reverse);
void srn_set_display_offset(int offset);
void srn_turn_display_on(bool on);
void srn_reverse_disp_on(bool reverse);
void srn_set_display_start(int start_line);

// PIXEL DATA
// The display pixel buffer is is a local copy of the display buffer
// in the SH1107.  All drawing commands make changes to display_pixel array
// and then call refresh to copy the holw buffer out to the SH1107.
extern uint8_t srn_display_pixels[16][128];

//send the current display_pixels to the display.
void srn_refresh();

// full screen fast clear
void srn_fast_clear();

// This function inits the SPI interface, turns the entire dislay white
// and then clears the display.  Turning the display wall white is used
// as an indicator that the interface is working and can be remove if desired.
void init_sh1107_SPI();

// this is a macro that can be used to write to any pixel on the screen.
#define PUT_PIXEL(_X, _Y, _B) 				\
  srn_display_pixels[(_Y)>>3][(_X)] =  ((_B) == 0) ?	\
  srn_display_pixels[(_Y)>>3][(_X)] & ~(1 << ((_Y) & 7)) :	\
  srn_display_pixels[(_Y)>>3][(_X)] |  (1 << ((_Y) & 7))



#endif
