#include "idle_indicator.h"

#include <zephyr/kernel.h>
#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/keymap.h>
#include <zmk/events/keycode_state_changed.h>

#include <fonts.h>
#include <sf_symbols.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

// Configuration
#define IDLE_TIMEOUT_MS 15000  // 15 seconds before showing animation
#define ANIMATION_DURATION_MS 1500  // 1.5 sec breathing cycle

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);
static int64_t last_activity_time = 0;

struct idle_indicator_state {
    bool is_idle;
};

static void idle_indicator_set_idle(lv_obj_t *obj, bool idle) {
    if (idle) {
        // Show the breathing pulse animation
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
    } else {
        // Hide the pulse when active
        lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
    }
}

static void idle_indicator_update_cb(struct idle_indicator_state state) {
    struct zmk_widget_idle_indicator *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        idle_indicator_set_idle(widget->pulse_obj, state.is_idle);
    }
}

static struct idle_indicator_state idle_indicator_get_state(const zmk_event_t *eh) {
    int64_t current_time = k_uptime_get();
    int64_t time_since_activity = current_time - last_activity_time;
    bool is_idle = (time_since_activity > IDLE_TIMEOUT_MS);

    return (struct idle_indicator_state){
        .is_idle = is_idle,
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_idle_indicator, struct idle_indicator_state,
                            idle_indicator_update_cb, idle_indicator_get_state)
ZMK_SUBSCRIPTION(widget_idle_indicator, zmk_keycode_state_changed);

// Reset idle timer on keystroke
static int idle_indicator_listener(const zmk_event_t *eh) {
    if (as_zmk_keycode_state_changed(eh) != NULL) {
        last_activity_time = k_uptime_get();
        ZMK_EVENT_RAISE(eh);
        return ZMK_LISTENER_ACTION_CONTINUE;
    }
    return ZMK_LISTENER_ACTION_CONTINUE;
}

ZMK_LISTENER(idle_indicator_activity, idle_indicator_listener);
ZMK_SUBSCRIPTION(idle_indicator_activity, zmk_keycode_state_changed);

static void create_breathing_animation(lv_obj_t *pulse_obj) {
    // Create a breathing effect by animating opacity
    static lv_anim_t pulse_anim;
    lv_anim_init(&pulse_anim);
    lv_anim_set_values(&pulse_anim, 100, 255);  // Opacity range: dimmer to brighter
    lv_anim_set_time(&pulse_anim, ANIMATION_DURATION_MS);
    lv_anim_set_playback_time(&pulse_anim, ANIMATION_DURATION_MS);  // Animate back down
    lv_anim_set_repeat_count(&pulse_anim, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_repeat_delay(&pulse_anim, 0);
    lv_anim_set_path_cb(&pulse_anim, lv_anim_path_ease_in_out);
    lv_anim_set_exec_cb(&pulse_anim, (lv_anim_exec_xcb_t)lv_obj_set_style_opa);
    lv_anim_set_var(&pulse_anim, pulse_obj);
    lv_anim_start(&pulse_anim);
}

int zmk_widget_idle_indicator_init(struct zmk_widget_idle_indicator *widget, lv_obj_t *parent) {
    // Initialize last activity time to now (so animation doesn't show immediately)
    last_activity_time = k_uptime_get();

    // Create main container
    widget->obj = lv_obj_create(parent);
    lv_obj_set_size(widget->obj, 60, 60);
    lv_obj_set_style_bg_opa(widget->obj, 0, LV_PART_MAIN);  // Transparent background
    lv_obj_set_style_border_width(widget->obj, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(widget->obj, 0, LV_PART_MAIN);

    // Create pulse element (circle or icon)
    widget->pulse_obj = lv_obj_create(widget->obj);
    lv_obj_set_size(widget->pulse_obj, 20, 20);
    lv_obj_set_style_bg_color(widget->pulse_obj, lv_color_hex(0x80D0FF), LV_PART_MAIN);  // Light blue
    lv_obj_set_style_bg_opa(widget->pulse_obj, 200, LV_PART_MAIN);
    lv_obj_set_style_radius(widget->pulse_obj, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_border_width(widget->pulse_obj, 0, LV_PART_MAIN);
    lv_obj_align(widget->pulse_obj, LV_ALIGN_CENTER, 0, 0);

    // Start with hidden (only show when idle)
    lv_obj_add_flag(widget->pulse_obj, LV_OBJ_FLAG_HIDDEN);

    // Create the breathing animation
    create_breathing_animation(widget->pulse_obj);

    sys_slist_prepend(&widgets, &widget->node);
    return 0;
}

lv_obj_t *zmk_widget_idle_indicator_obj(struct zmk_widget_idle_indicator *widget) {
    return widget->obj;
}
