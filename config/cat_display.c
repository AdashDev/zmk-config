/*
 * Custom cat animation display for ZMK OLED
 * This file needs to be integrated into your ZMK build system
 * Place this in your custom module or modify ZMK source accordingly
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/logging/log.h>

#include <zmk/display.h>
#include <zmk/display/widgets.h>
#include <zmk/event_manager.h>
#include <zmk/events/activity_state_changed.h>
#include <zmk/events/ble_active_profile_changed.h>
#include <zmk/endpoints.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if IS_ENABLED(CONFIG_ZMK_DISPLAY)

// Cat animation frames (simplified ASCII art converted to bitmap)
static const unsigned char cat_frame_1[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x18, 0xFF, 0xFF, 0x18, 0x18, 0xFF, 0xFF, 0x18, 0x00, 0x00, 0x00,
    0x18, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x18, 0x00, 0x00,
    0x18, 0xFF, 0xFF, 0x18, 0xFF, 0xFF, 0x18, 0xFF, 0xFF, 0x18, 0x00, 0x00,
    0x18, 0xFF, 0xFF, 0xFF, 0x18, 0x18, 0xFF, 0xFF, 0xFF, 0x18, 0x00, 0x00,
    0x00, 0x18, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const unsigned char cat_frame_2[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x18, 0xFF, 0xFF, 0x18, 0x18, 0xFF, 0xFF, 0x18, 0x00, 0x00, 0x00,
    0x18, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x18, 0x00, 0x00,
    0x18, 0xFF, 0xFF, 0x18, 0xFF, 0xFF, 0x18, 0xFF, 0xFF, 0x18, 0x00, 0x00,
    0x18, 0xFF, 0xFF, 0xFF, 0x3C, 0x3C, 0xFF, 0xFF, 0xFF, 0x18, 0x00, 0x00,
    0x00, 0x18, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x18, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const unsigned char* cat_frames[] = {
    cat_frame_1,
    cat_frame_2
};

#define CAT_FRAME_COUNT 2
#define CAT_FRAME_WIDTH 12
#define CAT_FRAME_HEIGHT 10

static lv_obj_t *cat_canvas;
static int current_frame = 0;
static struct k_work_delayable cat_animation_work;

static void update_cat_animation(struct k_work *work) {
    if (cat_canvas == NULL) {
        return;
    }

    // Update to next frame
    current_frame = (current_frame + 1) % CAT_FRAME_COUNT;
    
    // Clear canvas
    lv_canvas_fill_bg(cat_canvas, lv_color_black(), LV_OPA_COVER);
    
    // Draw current frame
    // This is a simplified example - you'd need to properly convert the bitmap
    // to LVGL compatible format and position it correctly
    
    // Schedule next frame update
    k_work_reschedule(&cat_animation_work, K_MSEC(1000)); // 1 second per frame
}

static int cat_display_widget_init(lv_obj_t *parent) {
    cat_canvas = lv_canvas_create(parent);
    
    if (cat_canvas == NULL) {
        return -ENOMEM;
    }
    
    // Set up canvas
    lv_obj_set_size(cat_canvas, CAT_FRAME_WIDTH * 4, CAT_FRAME_HEIGHT * 4); // Scale up 4x
    
    // Initialize animation work
    k_work_init_delayable(&cat_animation_work, update_cat_animation);
    
    // Start animation
    k_work_reschedule(&cat_animation_work, K_NO_WAIT);
    
    return 0;
}

static lv_obj_t *widget_cat_display_create(lv_obj_t *parent) {
    lv_obj_t *widget_container = lv_obj_create(parent);
    lv_obj_set_size(widget_container, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    
    cat_display_widget_init(widget_container);
    
    return widget_container;
}

static struct zmk_widget_cat_display {
    sys_snode_t node;
    lv_obj_t *obj;
} cat_display_widget;

int zmk_widget_cat_display_init(struct zmk_widget_cat_display *widget, lv_obj_t *parent) {
    widget->obj = widget_cat_display_create(parent);
    sys_slist_append(&zmk_display_widgets, &widget->node);

    return 0;
}

lv_obj_t *zmk_widget_cat_display_obj(struct zmk_widget_cat_display *widget) {
    return widget->obj;
}

// Event handler for activity state changes
static int cat_display_event_listener(const zmk_event_t *eh) {
    const struct zmk_activity_state_changed *activity_event = as_zmk_activity_state_changed(eh);
    if (activity_event) {
        if (activity_event->state == ZMK_ACTIVITY_ACTIVE) {
            // Resume animation when active
            k_work_reschedule(&cat_animation_work, K_NO_WAIT);
        } else {
            // Pause animation when inactive to save power
            k_work_cancel_delayable(&cat_animation_work);
        }
    }
    return 0;
}

ZMK_LISTENER(cat_display, cat_display_event_listener);
ZMK_SUBSCRIPTION(cat_display, zmk_activity_state_changed);

#endif // IS_ENABLED(CONFIG_ZMK_DISPLAY)