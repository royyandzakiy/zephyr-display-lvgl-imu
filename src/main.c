#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <lvgl.h>
#include <zephyr/kernel.h>
#include <string.h>

int main(void)
{
    const struct device *display = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
    
    if (!device_is_ready(display)) {
        printk("Display device not ready\n");
        return -1;
    }

    printk("Display initialized successfully\n");
    
    return 0;
}