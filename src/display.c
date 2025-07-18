#include "display.h"

LOG_MODULE_REGISTER(display_module, CONFIG_LOG_DEFAULT_LEVEL);

static const int32_t sleep_time_ms = 50;  

uint32_t count = 0;
char buf[11] = {0};

lv_obj_t *hello_label;
lv_obj_t *counter_label;

// 16x16 circle bitmap (1bpp, 32 bytes)
static const uint8_t circle_bitmap[32] = {
    0x00, 0x00, 0x00, 0x3C, 0x00, 0xFF, 0x03, 0xFF,
    0x07, 0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x1F, 0xFF,
    0x1F, 0xFF, 0x1F, 0xFF, 0x0F, 0xFF, 0x0F, 0xFF,
    0x07, 0xFF, 0x03, 0xFF, 0x00, 0xFF, 0x00, 0x3C
};

void draw_bottom_right_circle(const struct device *display_dev) {
    // Position calculation (bottom-right)
    const uint16_t x_pos = 128 - 16; // 112
    const uint16_t y_pos = 64 - 16;  // 48

    // Configure the buffer descriptor
    struct display_buffer_descriptor desc = {
        .width = 16,         // Bitmap width in pixels
        .height = 16,        // Bitmap height in pixels
        .pitch = 16,         // Width in bytes (16px = 2 bytes)
        .buf_size = 32       // Total bytes (16x16 @ 1bpp = 32 bytes)
    };

    display_write(display_dev, x_pos, y_pos, &desc, circle_bitmap);
    
    // Write to display with error checking
    if (display_write(display_dev, x_pos, y_pos, &desc, circle_bitmap)) {
        LOG_ERR("Failed to write circle bitmap!");
    } else {
        LOG_INF("Circle drawn at (%d, %d)", x_pos, y_pos);
    }
}

static const struct device *display;

int display_init() {
    display = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
    
    if (!device_is_ready(display)) {
        LOG_ERR("Display device not ready\n");
        return -1;
    }

    LOG_INF("Display initialized successfully\n");
	
    hello_label = lv_label_create(lv_scr_act());
    lv_label_set_text(hello_label, "Hello, World!");
    lv_obj_set_pos(hello_label, 2, 0);
	lv_obj_set_style_text_font(hello_label, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_clear_flag(hello_label, LV_OBJ_FLAG_HIDDEN);

    counter_label = lv_label_create(lv_scr_act());
    lv_obj_align(counter_label, LV_ALIGN_BOTTOM_MID, 0, 0);	

    // draw_bottom_right_circle(display);

    display_blanking_off(display);

    return 0;
}

int display_loop() {
    if (!device_is_ready(display)) {
        LOG_ERR("Display not ready, skipping LVGL\n");
        k_msleep(1000);
        return -1;  // Error
    }

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

    lv_task_handler();
    k_msleep(sleep_time_ms);
    return 0;
}