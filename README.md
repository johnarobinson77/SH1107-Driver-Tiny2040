# SH1107-Driver-Tiny2040

The code here is a mini driver for displays using the SH1107 driver chip for RP2040 based microcontrollers.  In this case the display is the [1.2 inch OLED display](https://shop.pimoroni.com/products/1-12-oled-breakout?variant=12628508704851) and the [Tiny2040 board](https://shop.pimoroni.com/products/tiny-2040) both from Pimoroni.  It is written in C, and the interface to the SH1107 is SPI through the SPI0 port on the Tiny2040, but it should be adaptable to other RP2040 boards.

The code from the lowest level to the highest level is as follows:
__sh1107_spi.c__ provides the SPI low-level interface, including many of the low-level sh1107 commands and sending the data to the sh1107 pixel buffer.  The externally available function calls are documented in SH1107.h.

__pixel_ops.c__ provides writes and scrolling pixels in the internal pixel buffer.  The programming model is that rendering is done to an internal pixel buffer, and then the call to srn_refersh() sends the contents of the pixel buffer to the SH1107.  The externally available function calls are documented in pixel_ops.h.

__draw_char.c_ provides the ability to describe a screen region as a text screen region and send text to that region.  The externally available function calls are available in draw_char.h.

__draw_graphics.c__ provides the ability to describe a screen region and draw lines, dots, and scrolling graphs.   Externally available function calls are in draw_graphics.h.

__Display_all.h__ is a single h file you can include that puls in the h files for all the previous

__sh1107_test.c__ has a main() the that tests the primary functionality of the code in the preceding files.  It is not required to be part of any project you might create with this code.  It may be useful for some usage examples.

__blink.c__ and blink.h blink the LEDs on the tyny2040.  sh1107_test.c uses it for debugging and progress indicators.  It is not needed for any project you might use this for.

The best example of what can be done with this driver can be found within the "#ifdef COMBINED_TEST" region of sh1107_test.c in which two independent text regions are placed below a scrolling graph.  Here is a picture of the display during that test.

![Combined Test](https://github.com/johnarobinson77/SH1107-Driver-Tiny2040/blob/main/display.jpg)
