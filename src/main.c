#include "display.h"
#include "imu.h"
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, CONFIG_LOG_DEFAULT_LEVEL);

int main(void)
{
	imu_init();
    display_init();

    while(1) {
        imu_loop();
        display_loop();
    }
}