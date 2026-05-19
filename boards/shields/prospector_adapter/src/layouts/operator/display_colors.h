#pragma once

// ── Nord palette ─────────────────────────────────────────────────────────────
// Palette reference:
//   Nord0  #2E3440  Polar Night — main bg
//   Nord1  #3B4252  Polar Night
//   Nord2  #434C5E  Polar Night
//   Nord3  #4C566A  Polar Night — dividers, inactive elements
//   Nord4  #D8DEE9  Snow Storm — primary text
//   Nord6  #ECEFF4  Snow Storm — highlights
//   Nord7  #8FBCBB  Frost — teal accent
//   Nord8  #88C0D0  Frost — frost blue (active layer, primary accent)
//   Nord9  #81A1C1  Frost — softer blue
//   Nord10 #5E81AC  Frost — deep blue
//   Nord11 #BF616A  Aurora — red (errors, low-battery if ever needed)
//   Nord13 #EBCB8B  Aurora — amber (caps word)
//   Nord14 #A3BE8C  Aurora — green (success / USB active)

// Modifier row
#define DISPLAY_COLOR_MOD_ACTIVE       0x88C0D0   // Nord8 frost blue
#define DISPLAY_COLOR_MOD_INACTIVE     0x4C566A   // Nord3 dim slate
#define DISPLAY_COLOR_MOD_SEPARATOR    0x4C566A   // Nord3
#define DISPLAY_COLOR_MOD_CAPS_WORD    0xEBCB8B   // Nord13 amber

// WPM meter
#define DISPLAY_COLOR_WPM_BAR_ACTIVE   0x88C0D0   // Nord8 frost blue
#define DISPLAY_COLOR_WPM_BAR_INACTIVE 0x3B4252   // Nord1 dim
#define DISPLAY_COLOR_WPM_TEXT         0x88C0D0   // Nord8

// Layer display
#define DISPLAY_COLOR_LAYER_TEXT         0xD8DEE9 // Nord4 snow
#define DISPLAY_COLOR_LAYER_DOT_ACTIVE   0xD8DEE9 // Nord4
#define DISPLAY_COLOR_LAYER_DOT_INACTIVE 0x4C566A // Nord3

// ── Unused in current build ──────────────────────────────────────────────────
// Battery widget removed (no batteries) and Output widget removed (USB-only,
// no BT profiles in use). Constants kept in case the widgets come back later.

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
