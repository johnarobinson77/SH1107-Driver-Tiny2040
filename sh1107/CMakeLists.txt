cmake_minimum_required(VERSION 3.13)

set(PICO_BOARD "pimoroni_tiny2040")

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(/agx_ssd/tiny2040/SDK/pico/pico-examples/pico_sdk_import.cmake)

project(Display)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project
add_executable(sh1107
  draw_graphics.c
  draw_char.c
  pixel_ops.c
  sh1107_spi.c
  blink.c
  sh1107_test.c
  )

# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(sh1107 pico_stdlib hardware_spi)

# create map/bin/hex file etc.
pico_add_extra_outputs(sh1107)

# add url via pico_set_program_url
#example_auto_set_url(sh1107)
