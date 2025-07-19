#include "display.h"
#include "imu.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, CONFIG_LOG_DEFAULT_LEVEL);

int main(void)
{
    LOG_INF("Main Start");

	imu_init();
    display_init();

    k_msleep(1000);

    while(1) {
        imu_loop();
        display_loop();
    }
 
    LOG_INF("Main End");
}