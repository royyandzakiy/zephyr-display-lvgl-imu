#ifndef _IMU_H
#define _IMU_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <stdio.h>
#include <zephyr/sys/__assert.h>
#include <zephyr/logging/log.h>

int imu_init();
int imu_loop();

#endif // _IMU_H