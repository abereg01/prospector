#include "aurora.h"
#include "display_colors.h"

#include <lvgl.h>
#include <stdlib.h>

// Nord aurora — five vertical "curtains" of color sliding horizontally at
// staggered speeds, each with a vertical color gradient (top: Nord accent
// color, bottom: background) and gentle height pulsing. The combined effect
// simulates an aurora borealis in the Nord palette.
//
// All animation is driven by LVGL's lv_anim_t timers; once init returns,
// the widget keeps animating with no further calls.

// Curtain palette — drawn from Nord Frost + Aurora groups for the
// blue/teal/green/purple aurora look. Order chosen so adjacent curtains
// don't clash when they overlap (alternating warm/cool tones).
static const uint32_t curtain_colors[AURORA_CURTAIN_COUNT] = {
    0x88C0D0,   // Nord8  — frost blue
    0xA3BE8C,   // Nord14 — green
    0x8FBCBB,   // Nord7  — teal
    0xB48EAD,   // Nord15 — purple
    0x5E81AC,   // Nord10 — deep frost blue
};

// Per-curtain animation timings (ms). Different periods so the curtains
// drift out of phase with each other, creating organic-looking interference.
static const uint32_t curtain_durations[AURORA_CURTAIN_COUNT] = {
    9000, 11000, 7500, 13000, 10500,
};

// Per-curtain start delay (ms) so they don't all enter from the left at boot.
static const uint32_t curtain_start_delays[AURORA_CURTAIN_COUNT] = {
    0, 1800, 3600, 800, 5400,
};

// Per-curtain pulse period (ms) for height breathing.
static const uint32_t curtain_pulse_periods[AURORA_CURTAIN_COUNT] = {
    3000, 4200, 3600, 5000, 3300,
};

static void anim_set_x(void *obj, int32_t v) {
    lv_obj_set_x((lv_obj_t *)obj, v);
}

static void anim_set_height(void *obj, int32_t v) {
    lv_obj_set_height((lv_obj_t *)obj, v);
}

int zmk_widget_aurora_init(struct zmk_widget_aurora *widget, lv_obj_t *parent,
                            int width, int height) {
    widget->obj = lv_obj_create(parent);
    lv_obj_set_size(widget->obj, width, height);
    // Solid Nord0 background so the curtains have something to fade into
    // when they slide off-canvas. Also lets the widget cleanly cover whatever
    // sits behind it (matters once this goes full-screen for the screensaver).
    lv_obj_set_style_bg_color(widget->obj, lv_color_hex(DISPLAY_COLOR_BG), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(widget->obj, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(widget->obj, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(widget->obj, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(widget->obj, 0, LV_PART_MAIN);
    lv_obj_clear_flag(widget->obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_clip_corner(widget->obj, true, LV_PART_MAIN);

    // Curtains are sized so they overlap on screen — ~30% of widget width each
    // gives nice blending where two curtains meet.
    int curtain_width = width / 3;
    if (curtain_width < 10) curtain_width = 10;

    // Vertical pulse range: between 70% and 100% of widget height.
    int pulse_min = (height * 7) / 10;
    int pulse_max = height;
    if (pulse_min < 20) pulse_min = 20;
    if (pulse_max <= pulse_min) pulse_max = pulse_min + 4;

    for (int i = 0; i < AURORA_CURTAIN_COUNT; i++) {
        widget->curtains[i] = lv_obj_create(widget->obj);
        lv_obj_set_size(widget->curtains[i], curtain_width, pulse_max);

        // Start curtains spread off-screen left so they enter staggered.
        lv_obj_set_pos(widget->curtains[i], -curtain_width - i * 20, 0);

        // Vertical gradient: top = Nord accent, bottom = Nord0 — gives the
        // soft "fading away" curtain look at the bottom edge.
        lv_obj_set_style_bg_color(widget->curtains[i], lv_color_hex(curtain_colors[i]), LV_PART_MAIN);
        lv_obj_set_style_bg_grad_color(widget->curtains[i], lv_color_hex(DISPLAY_COLOR_BG), LV_PART_MAIN);
        lv_obj_set_style_bg_grad_dir(widget->curtains[i], LV_GRAD_DIR_VER, LV_PART_MAIN);
        // ~50% opacity so overlapping curtains blend in additive-feeling way.
        lv_obj_set_style_bg_opa(widget->curtains[i], 130, LV_PART_MAIN);
        lv_obj_set_style_border_width(widget->curtains[i], 0, LV_PART_MAIN);
        lv_obj_set_style_pad_all(widget->curtains[i], 0, LV_PART_MAIN);
        lv_obj_set_style_radius(widget->curtains[i], 0, LV_PART_MAIN);
        lv_obj_clear_flag(widget->curtains[i], LV_OBJ_FLAG_SCROLLABLE);

        // Horizontal drift: left edge of canvas to right edge of canvas,
        // looping forever with ease-in-out so the motion feels organic.
        lv_anim_t a_x;
        lv_anim_init(&a_x);
        lv_anim_set_var(&a_x, widget->curtains[i]);
        lv_anim_set_exec_cb(&a_x, anim_set_x);
        lv_anim_set_values(&a_x, -curtain_width, width);
        lv_anim_set_time(&a_x, curtain_durations[i]);
        lv_anim_set_repeat_count(&a_x, LV_ANIM_REPEAT_INFINITE);
        lv_anim_set_delay(&a_x, curtain_start_delays[i]);
        lv_anim_set_path_cb(&a_x, lv_anim_path_ease_in_out);
        lv_anim_start(&a_x);

        // Height pulse: gentle breathing between pulse_min and pulse_max.
        // playback_time mirrors the forward motion so it ping-pongs.
        lv_anim_t a_h;
        lv_anim_init(&a_h);
        lv_anim_set_var(&a_h, widget->curtains[i]);
        lv_anim_set_exec_cb(&a_h, anim_set_height);
        lv_anim_set_values(&a_h, pulse_min, pulse_max);
        lv_anim_set_time(&a_h, curtain_pulse_periods[i]);
        lv_anim_set_playback_time(&a_h, curtain_pulse_periods[i]);
        lv_anim_set_repeat_count(&a_h, LV_ANIM_REPEAT_INFINITE);
        lv_anim_set_path_cb(&a_h, lv_anim_path_ease_in_out);
        lv_anim_start(&a_h);
    }

    return 0;
}

lv_obj_t *zmk_widget_aurora_obj(struct zmk_widget_aurora *widget) {
    return widget->obj;
}
