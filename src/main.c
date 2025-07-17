#include "display.h"
// #include <zephyr/logging/log.h>
// LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

// int main(void)
// {
//     // display_init();

//     // while (1) {
//     //     display_loop();
//     // }
// }

/*
 * Copyright (c) 2019 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

int imu_init();
int imu_loop();

int main(void)
{
	imu_init();

    while(1) {
        imu_loop();
    }
}

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <stdio.h>
#include <zephyr/sys/__assert.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(mpu6050_module, CONFIG_LOG_DEFAULT_LEVEL);

/* Static assertions for safety */
BUILD_ASSERT(DT_HAS_COMPAT_STATUS_OKAY(invensense_mpu6050), "No MPU6050 compatible device found");

/* Device pointer */
static const struct device *const mpu6050 = DEVICE_DT_GET_ONE(invensense_mpu6050);

/* Thread-safe timestamp function */
static const char *now_str(void)
{
    static char buf[16]; /* HH:MM:SS.MMM */
    static struct k_spinlock lock;
    k_spinlock_key_t key;
    
    uint32_t now = k_uptime_get_32();
    unsigned int ms = now % MSEC_PER_SEC;
    unsigned int s, min, h;

    now /= MSEC_PER_SEC;
    s = now % 60U;
    now /= 60U;
    min = now % 60U;
    h = now / 60U;

    key = k_spin_lock(&lock);
    snprintf(buf, sizeof(buf), "%u:%02u:%02u.%03u", h, min, s, ms);
    k_spin_unlock(&lock, key);
    
    return buf;
}

/* Safe sensor reading with error checking */
static int process_mpu6050(void)
{
    int rc;
    struct sensor_value temperature, accel[3], gyro[3];
    
    rc = sensor_sample_fetch(mpu6050);
    if (rc != 0) {
        LOG_ERR("Sample fetch failed: %d", rc);
        return rc;
    }

    rc = sensor_channel_get(mpu6050, SENSOR_CHAN_ACCEL_XYZ, accel);
    if (rc != 0) {
        LOG_ERR("Accel read failed: %d", rc);
        return rc;
    }

    rc = sensor_channel_get(mpu6050, SENSOR_CHAN_GYRO_XYZ, gyro);
    if (rc != 0) {
        LOG_ERR("Gyro read failed: %d", rc);
        return rc;
    }

    rc = sensor_channel_get(mpu6050, SENSOR_CHAN_DIE_TEMP, &temperature);
    if (rc != 0) {
        LOG_ERR("Temp read failed: %d", rc);
        return rc;
    }

    LOG_INF("[%s]: %.2f°C | Accel: %.2f,%.2f,%.2f m/s² | Gyro: %.2f,%.2f,%.2f rad/s",
           now_str(),
           sensor_value_to_double(&temperature),
           sensor_value_to_double(&accel[0]),
           sensor_value_to_double(&accel[1]),
           sensor_value_to_double(&accel[2]),
           sensor_value_to_double(&gyro[0]),
           sensor_value_to_double(&gyro[1]),
           sensor_value_to_double(&gyro[2]));

    return 0;
}

#ifdef CONFIG_MPU6050_TRIGGER
static struct sensor_trigger trigger = {
    .type = SENSOR_TRIG_DATA_READY,
    .chan = SENSOR_CHAN_ALL,
};

static void handle_drdy(const struct device *dev, const struct sensor_trigger *trig)
{
    ARG_UNUSED(trig);
    if (process_mpu6050() != 0) {
        sensor_trigger_set(dev, &trigger, NULL);
        LOG_WRN("Disabled trigger due to errors");
    }
}
#endif

int imu_init(void)
{
    if (!device_is_ready(mpu6050)) {
        LOG_ERR("MPU6050 device not ready");
        return -ENODEV;
    }

#ifdef CONFIG_MPU6050_TRIGGER
    if (sensor_trigger_set(mpu6050, &trigger, handle_drdy) < 0) {
        LOG_ERR("Trigger setup failed");
        return -EIO;
    }
    LOG_INF("Configured triggered sampling");
#endif

    return 0;
}

int imu_loop(void)
{
#ifndef CONFIG_MPU6050_TRIGGER
    int rc = process_mpu6050();
    if (rc != 0) {
        k_sleep(K_MSEC(500));
        return rc;
    }
    k_sleep(K_SECONDS(1));
#endif
    return 0;
}