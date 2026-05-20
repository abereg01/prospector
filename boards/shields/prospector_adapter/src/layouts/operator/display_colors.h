#pragma once

#include <stdint.h>

// ── Nord palette ─────────────────────────────────────────────────────────────
// Palette reference:
//   Nord0  #2E3440  Polar Night — main bg
//   Nord1  #3B4252  Polar Night
//   Nord2  #434C5E  Polar Night
//   Nord3  #4C566A  Polar Night — dividers, inactive elements
//   Nord4  #D8DEE9  Snow Storm — primary text
//   Nord6  #ECEFF4  Snow Storm — highlights
//   Nord7  #8FBCBB  Frost — teal accent
//   Nord8  #88C0D0  Frost — frost blue (active mods, default accent)
//   Nord9  #81A1C1  Frost — softer blue
//   Nord10 #5E81AC  Frost — deep blue
//   Nord11 #BF616A  Aurora — red (errors, disconnect)
//   Nord12 #D08770  Aurora — orange (NUM layer)
//   Nord13 #EBCB8B  Aurora — amber (caps word, NAV layer)
//   Nord14 #A3BE8C  Aurora — green (RAISE layer, connected)
//   Nord15 #B48EAD  Aurora — purple (Colemak layer)

#define DISPLAY_COLOR_BG               0x2E3440   // Nord0 — screen background

// Modifier row
#define DISPLAY_COLOR_MOD_ACTIVE       0x88C0D0   // Nord8 frost blue
#define DISPLAY_COLOR_MOD_INACTIVE     0x4C566A   // Nord3 dim slate
#define DISPLAY_COLOR_MOD_SEPARATOR    0x4C566A   // Nord3
#define DISPLAY_COLOR_MOD_CAPS_WORD    0xEBCB8B   // Nord13 amber

// WPM meter — bars get the active layer's accent color at runtime via
// display_color_for_layer(); WPM_BAR_ACTIVE is the fallback / BASE layer color.
#define DISPLAY_COLOR_WPM_BAR_ACTIVE   0x88C0D0   // Nord8 frost blue
#define DISPLAY_COLOR_WPM_BAR_INACTIVE 0x3B4252   // Nord1 dim
#define DISPLAY_COLOR_WPM_TEXT         0x88C0D0   // Nord8

// Layer display
#define DISPLAY_COLOR_LAYER_TEXT         0xD8DEE9 // Nord4 snow — fallback / BASE
#define DISPLAY_COLOR_LAYER_DOT_ACTIVE   0xD8DEE9 // Nord4 — overridden by per-layer color
#define DISPLAY_COLOR_LAYER_DOT_INACTIVE 0x4C566A // Nord3

// Connection status (split halves)
#define DISPLAY_COLOR_CONN_CONNECTED     0xA3BE8C // Nord14 green
#define DISPLAY_COLOR_CONN_DISCONNECTED  0xBF616A // Nord11 red
#define DISPLAY_COLOR_CONN_UNKNOWN       0x4C566A // Nord3 dim — at boot before first event

// ── Per-layer accent colors ──────────────────────────────────────────────────
// Canonical palette from zmk-config-klor/draw/keymap-drawer.yaml svg_style.
// Index space matches the dongle keymap #defines:
//   BASE=0  COLEMAK=1  SYSTEM=2  RAISE=3  NAV=4  NUM=5  SCROLL=6

#define DISPLAY_COLOR_LAYER_BASE     0xD8DEE9   // Nord4   snow (no tint)
#define DISPLAY_COLOR_LAYER_COLEMAK  0xB48EAD   // Nord15  purple
#define DISPLAY_COLOR_LAYER_SYSTEM   0x88C0D0   // Nord8   blue
#define DISPLAY_COLOR_LAYER_RAISE    0xA3BE8C   // Nord14  green
#define DISPLAY_COLOR_LAYER_NAV      0xEBCB8B   // Nord13  yellow
#define DISPLAY_COLOR_LAYER_NUM      0xD08770   // Nord12  orange
#define DISPLAY_COLOR_LAYER_SCROLL   0x8FBCBB   // Nord7   teal

static inline uint32_t display_color_for_layer(uint8_t index) {
    switch (index) {
        case 0:  return DISPLAY_COLOR_LAYER_BASE;
        case 1:  return DISPLAY_COLOR_LAYER_COLEMAK;
        case 2:  return DISPLAY_COLOR_LAYER_SYSTEM;
        case 3:  return DISPLAY_COLOR_LAYER_RAISE;
        case 4:  return DISPLAY_COLOR_LAYER_NAV;
        case 5:  return DISPLAY_COLOR_LAYER_NUM;
        case 6:  return DISPLAY_COLOR_LAYER_SCROLL;
        default: return DISPLAY_COLOR_LAYER_BASE;
    }
}

// ── Unused in current build ──────────────────────────────────────────────────
// Battery and Output widgets removed (no batteries, no BT profile use).
// Constants kept for future re-use.

#define DISPLAY_COLOR_BATTERY_FILL     0xA3BE8C   // Nord14 green
#define DISPLAY_COLOR_BATTERY_RING     0x4C566A   // Nord3
#define DISPLAY_COLOR_BATTERY_BG       0x3B4252   // Nord1
#define DISPLAY_COLOR_BATTERY_LABEL    0xD8DEE9   // Nord4

#define DISPLAY_COLOR_BATTERY_DISCONNECTED_FILL  0x3B4252  // Nord1
#define DISPLAY_COLOR_BATTERY_DISCONNECTED_RING  0x2E3440  // Nord0
#define DISPLAY_COLOR_BATTERY_DISCONNECTED_LABEL 0x4C566A  // Nord3

#define DISPLAY_COLOR_BATTERY_LOW_FILL  0xD08770  // Nord12 orange
#define DISPLAY_COLOR_BATTERY_LOW_RING  0x4C566A  // Nord3

#define DISPLAY_COLOR_USB_ACTIVE_BG        0xA3BE8C   // Nord14 green
#define DISPLAY_COLOR_USB_INACTIVE_BG      0x3B4252   // Nord1
#define DISPLAY_COLOR_BLE_ACTIVE_BG        0x88C0D0   // Nord8
#define DISPLAY_COLOR_BLE_INACTIVE_BG      0x3B4252   // Nord1
#define DISPLAY_COLOR_OUTPUT_ACTIVE_TEXT   0x2E3440   // Nord0
#define DISPLAY_COLOR_OUTPUT_INACTIVE_TEXT 0x4C566A   // Nord3

#define DISPLAY_COLOR_SLOT_ACTIVE_BG   0x88C0D0   // Nord8
#define DISPLAY_COLOR_SLOT_INACTIVE_BG 0x3B4252   // Nord1
#define DISPLAY_COLOR_SLOT_TEXT        0xD8DEE9   // Nord4
