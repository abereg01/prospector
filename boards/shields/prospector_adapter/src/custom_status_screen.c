#include <lvgl.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>

#include "widgets/layer_roller.h"
#include "widgets/layer_background.h"
#include "widgets/idle_indicator.h"
#include "widgets/caps_word_indicator.h"

#include <fonts.h>
#include <sf_symbols.h>

#include <zmk/keymap.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

// ── GMK Shuko palette ────────────────────────────────────────────────────────
// Chrome (header, encoder bar, dividers)
#define SHUKO_HEADER    0x1E2D3F   // deep navy — header / encoder bar bg
#define SHUKO_DIVIDER   0x3A5468   // slate-tinted divider
// Text
#define SHUKO_TEXT      0xF5F1E8   // Shuko cream — primary labels
#define SHUKO_ACCENT    0x7A99AC   // Shuko slate blue — encoder tag, highlights
// Caps word
#define SHUKO_CAPS      0xEBCB8B   // amber — caps word active indicator

static struct zmk_widget_layer_background layer_background_widget;
static struct zmk_widget_layer_roller     layer_roller_widget;
static struct zmk_widget_idle_indicator   idle_indicator_widget;
static struct zmk_widget_caps_word_indicator caps_word_indicator_widget;

// Helper: 1px horizontal divider at a given y position on the screen
static void make_hdivider(lv_obj_t *parent, int y) {
    lv_obj_t *div = lv_obj_create(parent);
    lv_obj_set_size(div, 240, 1);
    lv_obj_set_pos(div, 0, y);
    lv_obj_set_style_bg_color(div, lv_color_hex(SHUKO_DIVIDER), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(div, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(div, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(div, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(div, 0, LV_PART_MAIN);
}

lv_obj_t *zmk_display_status_screen() {
    // Display: 240 × 280 px
    //
    //  0 ┌──────────────────────────┐
    //    │  KLOR header (36px)      │  SHUKO_HEADER bg
    // 36 ├──────────────────────────┤  SHUKO_DIVIDER
    //    │                          │
    //    │  Layer area (183px)      │  layer_background widget (per-layer colour)
    //    │   — layer roller         │  transparent roller on top
    //    │   — idle indicator       │  breathing dot (shown after 15s idle)
    //    │                          │
    //219 ├──────────────────────────┤  SHUKO_DIVIDER
    //    │  Encoder bar (60px)      │  SHUKO_HEADER bg
    //280 └──────────────────────────┘

    lv_obj_t *screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen, lv_color_hex(SHUKO_HEADER), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(screen, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(screen, 0, LV_PART_MAIN);

    // ── Header bar (0–36px) ───────────────────────────────────────────────
    lv_obj_t *header = lv_obj_create(screen);
    lv_obj_set_size(header, 240, 36);
    lv_obj_set_pos(header, 0, 0);
    lv_obj_set_style_bg_color(header, lv_color_hex(SHUKO_HEADER), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(header, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(header, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(header, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(header, 0, LV_PART_MAIN);

    lv_obj_t *kb_label = lv_label_create(header);
    lv_label_set_text(kb_label, "KLOR");
    lv_obj_set_style_text_font(kb_label, &FoundryGridnikMedium_20, LV_PART_MAIN);
    lv_obj_set_style_text_color(kb_label, lv_color_hex(SHUKO_TEXT), LV_PART_MAIN);
    lv_obj_align(kb_label, LV_ALIGN_LEFT_MID, 12, 0);

#ifdef CONFIG_DT_HAS_ZMK_BEHAVIOR_CAPS_WORD_ENABLED
    zmk_widget_caps_word_indicator_init(&caps_word_indicator_widget, header);
    lv_obj_align(zmk_widget_caps_word_indicator_obj(&caps_word_indicator_widget),
                 LV_ALIGN_RIGHT_MID, -12, 0);
#endif

    // ── Divider (36px) ────────────────────────────────────────────────────
    make_hdivider(screen, 36);

    // ── Layer area (37–219px) ─────────────────────────────────────────────
    // 1. Coloured background (per-layer) — created first so it's behind the roller
    zmk_widget_layer_background_init(&layer_background_widget, screen);
    lv_obj_set_pos(zmk_widget_layer_background_obj(&layer_background_widget), 0, 37);

    // 2. Layer roller — transparent bg, sits on top of the coloured background
    zmk_widget_layer_roller_init(&layer_roller_widget, screen);
    lv_obj_set_size(zmk_widget_layer_roller_obj(&layer_roller_widget), 240, 182);
    lv_obj_set_pos(zmk_widget_layer_roller_obj(&layer_roller_widget), 0, 37);

    // 3. Idle indicator — breathing dot, visible after 15s of inactivity
    zmk_widget_idle_indicator_init(&idle_indicator_widget, screen);
    lv_obj_set_pos(zmk_widget_idle_indicator_obj(&idle_indicator_widget), 0, 37);

    // ── Divider (219px) ───────────────────────────────────────────────────
    make_hdivider(screen, 219);

    // ── Encoder hints bar (220–280px) ────────────────────────────────────
    // NOTE: Both encoders currently do Volume Up/Down on all layers.
    // Labels will update when the encoder refactor (left=arrows, right=scroll) is complete.
    lv_obj_t *enc_bar = lv_obj_create(screen);
    lv_obj_set_size(enc_bar, 240, 60);
    lv_obj_set_pos(enc_bar, 0, 220);
    lv_obj_set_style_bg_color(enc_bar, lv_color_hex(SHUKO_HEADER), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(enc_bar, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(enc_bar, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(enc_bar, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(enc_bar, 0, LV_PART_MAIN);

    // Vertical centre divider
    lv_obj_t *enc_vdiv = lv_obj_create(enc_bar);
    lv_obj_set_size(enc_vdiv, 1, 60);
    lv_obj_set_pos(enc_vdiv, 120, 0);
    lv_obj_set_style_bg_color(enc_vdiv, lv_color_hex(SHUKO_DIVIDER), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(enc_vdiv, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(enc_vdiv, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(enc_vdiv, 0, LV_PART_MAIN);

    // Left encoder (left half, x=0–119)
    lv_obj_t *enc_l_tag = lv_label_create(enc_bar);
    lv_label_set_text(enc_l_tag, "L ENC");
    lv_obj_set_style_text_font(enc_l_tag, &FoundryGridnikMedium_20, LV_PART_MAIN);
    lv_obj_set_style_text_color(enc_l_tag, lv_color_hex(SHUKO_ACCENT), LV_PART_MAIN);
    lv_obj_set_pos(enc_l_tag, 10, 6);

    lv_obj_t *enc_l_fn = lv_label_create(enc_bar);
    lv_label_set_text(enc_l_fn, "Volume");
    lv_obj_set_style_text_font(enc_l_fn, &FoundryGridnikMedium_20, LV_PART_MAIN);
    lv_obj_set_style_text_color(enc_l_fn, lv_color_hex(SHUKO_TEXT), LV_PART_MAIN);
    lv_obj_set_pos(enc_l_fn, 10, 32);

    // Right encoder (right half, x=121–239)
    lv_obj_t *enc_r_tag = lv_label_create(enc_bar);
    lv_label_set_text(enc_r_tag, "R ENC");
    lv_obj_set_style_text_font(enc_r_tag, &FoundryGridnikMedium_20, LV_PART_MAIN);
    lv_obj_set_style_text_color(enc_r_tag, lv_color_hex(SHUKO_ACCENT), LV_PART_MAIN);
    lv_obj_set_pos(enc_r_tag, 130, 6);

    lv_obj_t *enc_r_fn = lv_label_create(enc_bar);
    lv_label_set_text(enc_r_fn, "Volume");
    lv_obj_set_style_text_font(enc_r_fn, &FoundryGridnikMedium_20, LV_PART_MAIN);
    lv_obj_set_style_text_color(enc_r_fn, lv_color_hex(SHUKO_TEXT), LV_PART_MAIN);
    lv_obj_set_pos(enc_r_fn, 130, 32);

    return screen;
}
