/**
 * Copyright (c) 2021 John Robinson.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef BLINK_H
#define BLINK_H

void init_tiny2040_leds();
void set_leds(bool red, bool grn, bool blu);
int start_blinking(bool red, bool grn, bool blu, int num);

#endif
