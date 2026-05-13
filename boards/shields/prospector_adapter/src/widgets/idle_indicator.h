#pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>

struct zmk_widget_idle_indicator {
    sys_snode_t node;
    lv_obj_t *obj;
    lv_obj_t *pulse_obj;  // The element that will breathe
};

int zmk_widget_idle_indicator_init(struct zmk_widget_idle_indicator *widget, lv_obj_t *parent);
lv_obj_t *zmk_widget_idle_indicator_obj(struct zmk_widget_idle_indicator *widget);
