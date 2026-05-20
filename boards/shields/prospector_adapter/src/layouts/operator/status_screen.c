#include <lvgl.h>

#include "modifier_indicator.h"
#include "wpm_meter.h"
#include "layer_name.h"
#include "layer_display.h"
#include "connection_status.h"
#include "display_colors.h"

#include <fonts.h>

// Battery circles and Output widgets removed for this build:
//   - No batteries (dongle + halves are USB-powered)
//   - No BLE output profiles (Prospector is USB HID central)
// Bottom-right is reserved for the future Saturn animation widget.

static struct zmk_widget_modifier_indicator modifier_indicator_widget;
static struct zmk_widget_wpm_meter wpm_meter_widget;
static struct zmk_widget_layer_name layer_name_widget;
static struct zmk_widget_layer_display layer_display_widget;
static struct zmk_widget_connection_status connection_status_widget;

lv_obj_t *zmk_display_status_screen() {
    // Display: 240×280 px, Nord0 background.
    //
    //   0 ┌────────────────────────────────┐
    //     │  Modifier row  ⌘ ⌥ ⌃ ⇧         │  pos (25,   8)  230×24
    //  32 ├────────────────────────────────┤
    //     │  WPM bars + WPM number         │  pos (10,  42)  260×90
    // 132 ├────────────────────────────────┤
    //     │       <LAYER NAME, big>        │  pos (10, 138)  220×40, per-layer color
    // 178 ├────────────────────────────────┤
    //     │  • • • • • • •         L R     │  layer dots (10, 188) 200×6
    //     │                                │  conn status (215, 186) ~24×10
    // 200 ├────────────────────────────────┤
    //     │  KLOR           [Saturn slot]  │  KLOR (10, 220) — bottom-right reserved
    // 280 └────────────────────────────────┘

    lv_obj_t *screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen, lv_color_hex(DISPLAY_COLOR_BG), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(screen, 255, LV_PART_MAIN);

    zmk_widget_modifier_indicator_init(&modifier_indicator_widget, screen);
    lv_obj_set_pos(zmk_widget_modifier_indicator_obj(&modifier_indicator_widget), 25, 8);

    zmk_widget_wpm_meter_init(&wpm_meter_widget, screen);
    lv_obj_set_pos(zmk_widget_wpm_meter_obj(&wpm_meter_widget), 10, 42);

    zmk_widget_layer_name_init(&layer_name_widget, screen);
    lv_obj_set_pos(zmk_widget_layer_name_obj(&layer_name_widget), 10, 138);

    zmk_widget_layer_display_init(&layer_display_widget, screen);
    lv_obj_set_pos(zmk_widget_layer_display_obj(&layer_display_widget), 10, 188);

    zmk_widget_connection_status_init(&connection_status_widget, screen);
    lv_obj_set_pos(zmk_widget_connection_status_obj(&connection_status_widget), 215, 186);

    // ── KLOR wordmark (bottom-left, replacing battery circles) ───────────────
    lv_obj_t *klor_label = lv_label_create(screen);
    lv_label_set_text(klor_label, "KLOR");
    lv_obj_set_style_text_font(klor_label, &DINishExpanded_Light_36, LV_PART_MAIN);
    lv_obj_set_style_text_color(klor_label, lv_color_hex(DISPLAY_COLOR_MOD_ACTIVE), LV_PART_MAIN);  // Nord8
    lv_obj_set_pos(klor_label, 10, 220);

    return screen;
}
