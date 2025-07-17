#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <lvgl.h>
#include <zephyr/kernel.h>
#include <string.h>

static const int32_t sleep_time_ms = 50;  

uint32_t count = 0;
char buf[11] = {0};

lv_obj_t *hello_label;
lv_obj_t *counter_label;

int main(void)
{
    const struct device *display = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
    
    if (!device_is_ready(display)) {
        printk("Display device not ready\n");
        return -1;
    }

    printk("Display initialized successfully\n");
	
	// Create a static label widget
    hello_label = lv_label_create(lv_scr_act());
    lv_label_set_text(hello_label, "Hello, World!");
    lv_obj_set_pos(hello_label, 2, 0);
	lv_obj_set_style_text_font(hello_label, &lv_font_montserrat_14, LV_PART_MAIN);

    // Create a dynamic label widget
    counter_label = lv_label_create(lv_scr_act());
    lv_obj_align(counter_label, LV_ALIGN_BOTTOM_MID, 0, 0);	

	// Disable display blanking
    display_blanking_off(display);

	// Do forever
    while (1) {

        // Update counter label every second
        count++;
        if ((count % (1000 / sleep_time_ms)) == 0) {
            sprintf(buf, "%d", count / (1000 / sleep_time_ms));
            lv_label_set_text(counter_label, buf);
        }
	    
	    if ((count % 2) == 0) {
		    lv_obj_add_flag(hello_label, LV_OBJ_FLAG_HIDDEN);
	    } else {
		    lv_obj_clear_flag(hello_label, LV_OBJ_FLAG_HIDDEN);
	    }

        // Must be called periodically
        lv_task_handler();

        // Sleep
        k_msleep(sleep_time_ms);
    }
}