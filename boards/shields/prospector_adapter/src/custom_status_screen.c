#include <lvgl.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>

#include "widgets/layer_roller.h"
#include "widgets/caps_word_indicator.h"

#include <fonts.h>
#include <sf_symbols.h>

#include <zmk/keymap.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

// Nord color palette
// Polar Night
#define NORD0  0x2E3440   // main background
#define NORD1  0x3B4252   // header / encoder bar
#define NORD3  0x4C566A   // dividers, inactive elements
// Snow Storm
#define NORD4  0xD8DEE9   // primary text
// Frost
#define NORD8  0x88C0D0   // accent — active layer, encoder labels
// Aurora
#define NORD13 0xEBCB8B   // amber — caps word active

static struct zmk_widget_layer_roller layer_roller_widget;
static struct zmk_widget_caps_word_indicator caps_word_indicator_widget;

// Helper: 1px horizontal divider at a given y position on parent
static void make_hdivider(lv_obj_t *parent, int y) {
    lv_obj_t *div = lv_obj_create(parent);
    lv_obj_set_size(div, 240, 1);
    lv_obj_set_pos(div, 0, y);
    lv_obj_set_style_bg_color(div, lv_color_hex(NORD3), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(div, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(div, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(div, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(div, 0, LV_PART_MAIN);
}

lv_obj_t *zmk_display_status_screen() {
    // Display: 240 × 280 px
    //
    //  0 ┌──────────────────────────┐
    //    │  KLOR header (36px)      │  Nord1 bg
    // 36 ├──────────────────────────┤  Nord3 divider
    //    │                          │
    //    │   Layer roller (183px)   │  Nord0 bg
    //    │                          │
    //219 ├──────────────────────────┤  Nord3 divider
    //    │  Encoder hints (60px)    │  Nord1 bg
    //280 └──────────────────────────┘

    lv_obj_t *screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen, lv_color_hex(NORD0), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(screen, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(screen, 0, LV_PART_MAIN);

    // ── Header bar (0–36px) ───────────────────────────────────
    lv_obj_t *header = lv_obj_create(screen);
    lv_obj_set_size(header, 240, 36);
    lv_obj_set_pos(header, 0, 0);
    lv_obj_set_style_bg_color(header, lv_color_hex(NORD1), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(header, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(header, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(header, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(header, 0, LV_PART_MAIN);

    lv_obj_t *kb_label = lv_label_create(header);
    lv_label_set_text(kb_label, "KLOR");
    lv_obj_set_style_text_font(kb_label, &FoundryGridnikMedium_20, LV_PART_MAIN);
    lv_obj_set_style_text_color(kb_label, lv_color_hex(NORD4), LV_PART_MAIN);
    lv_obj_align(kb_label, LV_ALIGN_LEFT_MID, 12, 0);

#ifdef CONFIG_DT_HAS_ZMK_BEHAVIOR_CAPS_WORD_ENABLED
    zmk_widget_caps_word_indicator_init(&caps_word_indicator_widget, header);
    lv_obj_align(zmk_widget_caps_word_indicator_obj(&caps_word_indicator_widget),
                 LV_ALIGN_RIGHT_MID, -12, 0);
#endif

    // ── Divider (36px) ────────────────────────────────────────
    make_hdivider(screen, 36);

    // ── Layer roller (37–219px) ───────────────────────────────
    zmk_widget_layer_roller_init(&layer_roller_widget, screen);
    lv_obj_set_size(zmk_widget_layer_roller_obj(&layer_roller_widget), 240, 182);
    lv_obj_set_pos(zmk_widget_layer_roller_obj(&layer_roller_widget), 0, 37);

    // ── Divider (219px) ───────────────────────────────────────
    make_hdivider(screen, 219);

    // ── Encoder hints bar (220–280px) ─────────────────────────
    lv_obj_t *enc_bar = lv_obj_create(screen);
    lv_obj_set_size(enc_bar, 240, 60);
    lv_obj_set_pos(enc_bar, 0, 220);
    lv_obj_set_style_bg_color(enc_bar, lv_color_hex(NORD1), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(enc_bar, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(enc_bar, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(enc_bar, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(enc_bar, 0, LV_PART_MAIN);

    // Vertical centre divider
    lv_obj_t *enc_vdiv = lv_obj_create(enc_bar);
    lv_obj_set_size(enc_vdiv, 1, 60);
    lv_obj_align(enc_vdiv, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_color(enc_vdiv, lv_color_hex(NORD3), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(enc_vdiv, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(enc_vdiv, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(enc_vdiv, 0, LV_PART_MAIN);

    // Left encoder: "L ENC" (Nord8 accent) + "History" (Nord4)
    lv_obj_t *enc_l_tag = lv_label_create(enc_bar);
    lv_label_set_text(enc_l_tag, "L ENC");
    lv_obj_set_style_text_font(enc_l_tag, &FoundryGridnikMedium_20, LV_PART_MAIN);
    lv_obj_set_style_text_color(enc_l_tag, lv_color_hex(NORD8), LV_PART_MAIN);
    lv_obj_align(enc_l_tag, LV_ALIGN_TOP_LEFT, 10, 6);

    lv_obj_t *enc_l_fn = lv_label_create(enc_bar);
    lv_label_set_text(enc_l_fn, "History");
    lv_obj_set_style_text_font(enc_l_fn, &FoundryGridnikMedium_20, LV_PART_MAIN);
    lv_obj_set_style_text_color(enc_l_fn, lv_color_hex(NORD4), LV_PART_MAIN);
    lv_obj_align(enc_l_fn, LV_ALIGN_TOP_LEFT, 10, 32);

    // Right encoder: "R ENC" (Nord8 accent) + "Scroll" (Nord4)
    lv_obj_t *enc_r_tag = lv_label_create(enc_bar);
    lv_label_set_text(enc_r_tag, "R ENC");
    lv_obj_set_style_text_font(enc_r_tag, &FoundryGridnikMedium_20, LV_PART_MAIN);
    lv_obj_set_style_text_color(enc_r_tag, lv_color_hex(NORD8), LV_PART_MAIN);
    lv_obj_align(enc_r_tag, LV_ALIGN_TOP_MID, 10, 6);

    lv_obj_t *enc_r_fn = lv_label_create(enc_bar);
    lv_label_set_text(enc_r_fn, "Scroll");
    lv_obj_set_style_text_font(enc_r_fn, &FoundryGridnikMedium_20, LV_PART_MAIN);
    lv_obj_set_style_text_color(enc_r_fn, lv_color_hex(NORD4), LV_PART_MAIN);
    lv_obj_align(enc_r_fn, LV_ALIGN_TOP_MID, 10, 32);

    return screen;
}
