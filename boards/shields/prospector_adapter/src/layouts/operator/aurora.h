#pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>

#define AURORA_CURTAIN_COUNT 5

struct zmk_widget_aurora {
    sys_snode_t node;
    lv_obj_t *obj;
    lv_obj_t *curtains[AURORA_CURTAIN_COUNT];
};

// Initialize the aurora widget inside `parent` and size it to `width × height`.
// The widget is self-animating once initialized — it starts its own LVGL
// animation timers and continues indefinitely. Designed to scale: the same
// widget works as a small bottom-right preview (~100×75) or as a full-screen
// (240×280) idle screensaver. Use lv_obj_set_pos on widget->obj to place it.
int zmk_widget_aurora_init(struct zmk_widget_aurora *widget, lv_obj_t *parent,
                            int width, int height);
lv_obj_t *zmk_widget_aurora_obj(struct zmk_widget_aurora *widget);
