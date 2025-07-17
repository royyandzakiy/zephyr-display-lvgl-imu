#ifndef DISPLAY_H
#define DISPLAY_H

#include <zephyr/drivers/display.h>
#include <lvgl.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

int display_init();
int display_loop();

#endif // DISPLAY_H