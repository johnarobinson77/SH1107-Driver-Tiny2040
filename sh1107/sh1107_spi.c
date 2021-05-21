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

/*  code to talk to a sh1107 display controler for pimoroni 1.2" 128x128 monochrome display.


   GPIO 0 (pin 12) D/C 
   GPIO 1 (pin 15) Chip select -> CSB/!CS on bme280 board
   GPIO 2 (pin 14) SCK/spi0_sclk -> SCL/SCK on bme280 board
   GPIO 3 (pin 13) MOSI/spi0_tx -> MOSI on SH1107

   Note: The SPI0 on the Tiny2040 defined GPIO 0 as the RX pin.  But
   the SH1107 display controller does not have an MISO pin that would
   normally be connected to the RX pin.  So that pin is repurposed to
   be the D/C pin required by the SH1107.

*/

// this is the list of GPIO pins for the tiny2040 SPI0
#define SPI_CSN_PIN 1 
#define DATA_CMD_PIN 0
//#define SPI_RX_PIN 0
#define SPI_SCK_PIN 2
#define SPI_TX_PIN 3
#define spi_display spi0

// the next 5 functions are low lever SPI operations that
// are used to write commands or data to the SH1107

static inline void cs_select() {
  asm volatile("nop \n nop \n nop");
  gpio_put(SPI_CSN_PIN, 0);  // Active low
  asm volatile("nop \n nop \n nop");
}

static inline void cs_deselect() {
  asm volatile("nop \n nop \n nop");
  gpio_put(SPI_CSN_PIN, 1);
  asm volatile("nop \n nop \n nop");
}

static inline void cmd_select() {
  asm volatile("nop \n nop \n nop");
  gpio_put(DATA_CMD_PIN, 0);  // CMD = 1
  asm volatile("nop \n nop \n nop");
}

static inline void data_select() {
  asm volatile("nop \n nop \n nop");
  gpio_put(DATA_CMD_PIN, 1); // DATA = 0
  asm volatile("nop \n nop \n nop");
}

static void write_spi(uint8_t *buf, int num, bool data_cmd) {
  if (data_cmd) cmd_select(); else data_select();
  cs_select();
  spi_write_blocking(spi_display, buf, num);
  cs_deselect();
  //sleep_ms(1);
}

// SH1107 COMMANDS
// The next set of functions are used to send commands to the
// SH1107.  Discussions of wht these commands do can be found
// in the Commands chapter (page 23) of the SH1107 spec sheet.

void srn_set_col_page(int col, int page) {
  uint8_t buf[3];
  buf[0] = 0x10 | ((col >> 4) & 0x7);
  buf[1] = 0x00 | col & 0xF;
  buf[2] = 0xB0 | page & 0xf;
  write_spi(buf, 3, true);
}

void srn_set_mem_adr_mode(int p_v) {
  uint8_t buf[1];
  buf[0] = 0x20 | p_v & 11;
  write_spi(buf, 1, true);
}

void srn_set_contrast(int contrast) {
  uint8_t buf[2];
  buf[0] = 0x81;
  buf[1] = contrast & 0xFF;
  write_spi(buf, 2, true);
}

void srn_set_seg_rot(int p_v) {
  uint8_t buf[1];
  buf[0] = 0xa0 | p_v & 1;
  write_spi(buf, 1, true);
}

void srn_turn_entire_disp_on(bool on) {
  uint8_t buf[1];
  buf[0] = 0xA4;
  if (on) buf[0] |= 1;
  write_spi(buf, 1, true);
}

void srn_set_reverse_display(bool reverse) {
  uint8_t buf[1];
  buf[0] = 0xA6;
  if (reverse) buf[0] |= 1;
  write_spi(buf, 1, true);
}

void srn_set_display_offset(int offset) {
  uint8_t buf[2];
  buf[0] = 0x81;
  buf[1] = offset & 0x7F;
  write_spi(buf, 2, true);
}

void srn_turn_display_on(bool on) {
  uint8_t buf[1];
  buf[0] = 0xAE;
  if (on) buf[0] |= 1;
  write_spi(buf, 1, true);
}

void srn_reverse_disp_on(bool reverse) {
  uint8_t buf[1];
  buf[0] = 0xC0;
  if (reverse) buf[0] |= 8;
  write_spi(buf, 1, true);
}

void srn_set_display_start(int start_line) {
  uint8_t buf[2];
  buf[0] = 0xDB;
  buf[1] = start_line & 0x7F;
  write_spi(buf, 2, true);
}

// PIXEL DATA
// The display pixel buffer is is a local copy of the display buffer
// in the SH1107.  All drawing commands make changes to display_pixel array
// and then call refresh to copy the holw buffer out to the SH1107.

uint8_t srn_display_pixels[16][128];

void srn_refresh() {
  for (int j = 0; j < 16; j++) {
    srn_set_col_page(0, j);
    write_spi(srn_display_pixels[j], 128, false);
  }
}

void srn_fast_clear() {
  for (int j = 0; j < 16; j++) {
    for (int i = 0; i < 128; i++) {
      srn_display_pixels[j][i] = 0;
    }
  }
  srn_refresh();
}


// inits the SPI interface and clears the display
void init_sh1107_SPI() {
  // This example will use SPI0 at 0.5MHz.
  spi_init(spi_display, 1000 * 1000);
  gpio_set_function(SPI_SCK_PIN, GPIO_FUNC_SPI);
  gpio_set_function(SPI_TX_PIN, GPIO_FUNC_SPI);
  // Make the SPI pins available to picotool
  bi_decl(bi_2pins_with_func( SPI_TX_PIN, SPI_SCK_PIN, GPIO_FUNC_SPI));
  
  // Chip select is active-low, so we'll initialise it to a driven-high state
  gpio_init(SPI_CSN_PIN);
  gpio_set_dir(SPI_CSN_PIN, GPIO_OUT);
  gpio_put(SPI_CSN_PIN, 1);
  // Make the CS pin available to picotool
  bi_decl(bi_1pin_with_name(SPI_CSN_PIN, "SPI CS"));
  gpio_init(DATA_CMD_PIN);
  gpio_set_dir(DATA_CMD_PIN, GPIO_OUT);
  gpio_put(DATA_CMD_PIN, 1);
  // Make the DATA_CMD pin available to picotool
  bi_decl(bi_1pin_with_name(DATA_CMD_PIN, "SPI CS"));

  srn_turn_display_on(true);
  srn_turn_entire_disp_on(true);
  sleep_ms(1000);
  srn_turn_entire_disp_on(false);
  sleep_ms(500);
  
  srn_fast_clear();
  
}




