#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, CONFIG_LOG_DEFAULT_LEVEL);

#include <zephyr/kernel.h>
#include <zephyr/drivers/display.h>
#include <lvgl.h>
#include <string.h>

// Settings
static const int32_t sleep_time_ms = 50;        // Target 20 FPS

int main(void)
{
    LOG_INF("Main Start");
    uint32_t count = 0;
    char buf[11] = {0};
    const struct device *display;
    lv_obj_t *hello_label;
    lv_obj_t *counter_label;
    lv_obj_t *rect;
    lv_obj_t *circle;
    lv_style_t rect_style;
    lv_style_t circle_style;
    lv_point_t rect_points[5] = { {0, 0}, {120, 0}, {120, 20}, {0, 20}, {0, 0} };
    const uint32_t circle_radius = 15;

    // Initialize the display
    display = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
    if (!device_is_ready(display)) {
        LOG_ERR("Error: display not ready\r\n");
        return 0;
    }

    // Create a static label widget
    hello_label = lv_label_create(lv_scr_act());
    lv_label_set_text(hello_label, "Hello, World!");
    lv_obj_align(hello_label, LV_ALIGN_TOP_MID, 0, 5);

    // Create a dynamic label widget
    counter_label = lv_label_create(lv_scr_act());
    lv_obj_align(counter_label, LV_ALIGN_BOTTOM_MID, 0, 0);

    // Set line style
    lv_style_init(&rect_style);
    lv_style_set_line_color(&rect_style, lv_color_hex(0x0000FF));
    lv_style_set_line_width(&rect_style, 3);

    // Create a rectangle out of lines
    rect = lv_line_create(lv_scr_act());
    lv_obj_add_style(rect, &rect_style, 0);
    lv_line_set_points(rect,
                       rect_points,
                       sizeof(rect_points) / sizeof(rect_points[0]));
    lv_obj_align(rect, LV_ALIGN_TOP_MID, 0, 0);

    // Set circle style
    lv_style_init(&circle_style);
    lv_style_set_radius(&circle_style, circle_radius);
    lv_style_set_bg_opa(&circle_style, LV_OPA_100);
    lv_style_set_bg_color(&circle_style, lv_color_hex(0xFF0000));

    // Create an object with the new style
    circle = lv_obj_create(lv_scr_act());
    lv_obj_set_size(circle, circle_radius * 2, circle_radius * 2);
    lv_obj_add_style(circle, &circle_style, 0);
    lv_obj_align(circle, LV_ALIGN_CENTER, 0, 5);

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

        // Must be called periodically
        lv_task_handler();

        // Sleep
        k_msleep(sleep_time_ms);
    }
}