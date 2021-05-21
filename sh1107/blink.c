/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"

#ifndef TINY2040_LED_R_PIN
#warning this blink code requires a tiny2040 board with RGB led
#endif

bool tiny2040_led_inited = false;

void init_tiny2040_leds() {
  if (!tiny2040_led_inited) {
    gpio_init(TINY2040_LED_R_PIN);
    gpio_set_dir(TINY2040_LED_R_PIN, GPIO_OUT);
    gpio_init(TINY2040_LED_G_PIN);
    gpio_set_dir(TINY2040_LED_G_PIN, GPIO_OUT);
    gpio_init(TINY2040_LED_B_PIN);
    gpio_set_dir(TINY2040_LED_B_PIN, GPIO_OUT);
    gpio_put(TINY2040_LED_R_PIN, 1);
    gpio_put(TINY2040_LED_G_PIN, 1);
    gpio_put(TINY2040_LED_B_PIN, 1);
    tiny2040_led_inited = true;
  }
}

void set_leds(bool red, bool grn, bool blu) {
    if (red) gpio_put(TINY2040_LED_R_PIN, 0);
    else gpio_put(TINY2040_LED_R_PIN, 1);
    if (grn) gpio_put(TINY2040_LED_G_PIN, 0);
    else gpio_put(TINY2040_LED_G_PIN, 1);
    if (blu) gpio_put(TINY2040_LED_B_PIN, 0);
    else gpio_put(TINY2040_LED_B_PIN, 1);
}

int start_blinking(bool red, bool grn, bool blu, int num) {
  init_tiny2040_leds();
  for (int i = 0;  i < num; i++) {
    if (red) gpio_put(TINY2040_LED_R_PIN, 0);
    if (grn) gpio_put(TINY2040_LED_G_PIN, 0);
    if (blu) gpio_put(TINY2040_LED_B_PIN, 0);
    sleep_ms(250);
    if (red) gpio_put(TINY2040_LED_R_PIN, 1);
    if (grn) gpio_put(TINY2040_LED_G_PIN, 1);
    if (blu) gpio_put(TINY2040_LED_B_PIN, 1);
    sleep_ms(250);
  }
  return 0;
}
