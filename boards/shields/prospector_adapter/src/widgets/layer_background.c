#include "layer_background.h"

#include <zmk/display.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/event_manager.h>
#include <zmk/keymap.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

// ── GMK Shuko per-layer background colours ───────────────────────────────────
// All dark enough that the slate-blue roller text (#7A99AC active, #4A6070 inactive)
// remains readable. Subtle but distinct — gives clear visual layer feedback.
//
//  BASE   — deep navy        #1E2D3F  (default / resting state)
//  LOWER  — slate blue wash  #243D55  (mod layer, cooler)
//  RAISE  — mid slate        #1E3348  (symbol layer, slightly different hue)
//  ADJUST — dark slate       #182A3C  (system/BT layer, darkest)

static const uint32_t layer_colors[] = {
    0x1E2D3F,   // 0 BASE
    0x243D55,   // 1 LOWER
    0x1E3348,   // 2 RAISE
    0x182A3C,   // 3 ADJUST
};

#define NUM_LAYER_COLORS ARRAY_SIZE(layer_colors)

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct layer_background_state {
    uint8_t layer_index;
};

static void layer_background_update_cb(struct layer_background_state state) {
    uint8_t idx = (state.layer_index < NUM_LAYER_COLORS) ? state.layer_index
                                                          : NUM_LAYER_COLORS - 1;
    lv_color_t color = lv_color_hex(layer_colors[idx]);

    struct zmk_widget_layer_background *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        lv_obj_set_style_bg_color(widget->obj, color, LV_PART_MAIN);
    }
}

static struct layer_background_state layer_background_get_state(const zmk_event_t *eh) {
    return (struct layer_background_state){
        .layer_index = zmk_keymap_highest_layer_active(),
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_layer_background, struct layer_background_state,
                            layer_background_update_cb, layer_background_get_state)
ZMK_SUBSCRIPTION(widget_layer_background, zmk_layer_state_changed);

int zmk_widget_layer_background_init(struct zmk_widget_layer_background *widget,
                                     lv_obj_t *parent) {
    widget->obj = lv_obj_create(parent);
    lv_obj_set_size(widget->obj, 240, 182);
    lv_obj_set_style_bg_color(widget->obj, lv_color_hex(layer_colors[0]), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(widget->obj, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(widget->obj, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(widget->obj, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(widget->obj, 0, LV_PART_MAIN);

    sys_slist_prepend(&widgets, &widget->node);
    widget_layer_background_init();
    return 0;
}

lv_obj_t *zmk_widget_layer_background_obj(struct zmk_widget_layer_background *widget) {
    return widget->obj;
}
