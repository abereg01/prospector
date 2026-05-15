# Prospector + KLOR — Setup Reference

## What this repo is

Prospector is a standalone ZMK display module for a Seeeduino XIAO BLE dongle with an ST7789V display. In this build it acts as the **BLE central** for the KLOR split keyboard. The KLOR halves are peripherals; the Prospector dongle connects to the computer via USB and handles the keymap.

---

## Repo architecture

| Repo | Purpose | ZMK version |
|---|---|---|
| `prospector` (this repo) | Dongle firmware + display | v0.3 |
| `zmk-config-klor` | Left + right half firmware | v0.3 |

These are **separate repos** intentionally. Merging them would cause a west manifest conflict (both pin ZMK v0.3 but must resolve it independently).

---

## Hardware

| Device | Board | Role |
|---|---|---|
| Left half | nice!nano v2 | BLE peripheral |
| Right half | nice!nano v2 | BLE peripheral |
| Dongle | Seeeduino XIAO BLE | BLE central + USB HID to computer |
| Display | ST7789V (240×280) | Driven by Prospector on the dongle |

TRRS jacks are soldered on both halves for future optionality. They are **not used** in this setup — switching to wired TRRS requires reflashing both halves with a new firmware where the left half is central.

No batteries. All three devices are USB-powered.

---

## Flashing

### Enter UF2 bootloader
**Double-tap the reset button** on the device. A USB drive appears on your Mac. Drag the `.uf2` file onto it.

- Dongle → `klor_dongle prospector_adapter-seeeduino_xiao_ble-zmk.uf2`
- Left half → `klor_left-nice_nano_v2-zmk.uf2`
- Right half → `klor_right-nice_nano_v2-zmk.uf2`
- Settings reset → `settings_reset-seeeduino_xiao_ble-zmk.uf2` (dongle) or `settings_reset-nice_nano_v2-zmk.uf2` (halves)

### Initial pairing order (after settings reset)
1. Flash settings_reset to all three devices, then flash actual firmware
2. Plug in dongle via USB first
3. Power on left half
4. Power on right half

They pair automatically. The dongle advertises on Bluetooth as **"Prospector"**.

---

## Key files

### In this repo (`prospector/`)

| File | Purpose |
|---|---|
| `config/west.yml` | Pins ZMK to v0.3 |
| `build.yaml` | Builds `klor_dongle + prospector_adapter` and `settings_reset` |
| `config/klor_dongle.keymap` | Full KLOR keymap (copied from zmk-config-klor, dongle-safe) |
| `boards/shields/klor_dongle/` | Dongle shield — matrix transform, encoder stubs, 2 peripherals |
| `boards/shields/prospector_adapter/` | Display shield — ST7789V, LVGL widgets, Nord UI |

### In `zmk-config-klor/`

| File | What changed |
|---|---|
| `config/boards/shields/klor/Kconfig.defconfig` | Removed `ZMK_SPLIT_BLE_ROLE_CENTRAL` from left half — both halves are now peripherals |

**To revert KLOR to standalone wired (no dongle):** add back `config ZMK_SPLIT_BLE_ROLE_CENTRAL / default y` inside the `if SHIELD_KLOR_LEFT` block.

---

## Display UI — Nord theme

### Layout (240×280px)

```
 0  ┌──────────────────────────┐
    │  KLOR            [CAPS]  │  Header — Nord1 bg, FoundryGridnikMedium_20
36  ├──────────────────────────┤  Nord3 divider
    │                          │
    │      layer roller        │  Active layer: Nord8 #88C0D0 (frost blue), FRAC_Regular_48
    │        BASE              │  Inactive: Nord3 #4C566A, FRAC_Thin_48
    │                          │
219 ├──────────────────────────┤  Nord3 divider
    │  L ENC    │    R ENC     │  Encoder bar — Nord1 bg
    │  History  │    Scroll    │  Nord8 labels, Nord4 function names
280 └──────────────────────────┘
```

### Nord palette in use

| Constant | Hex | Usage |
|---|---|---|
| Nord0 | `#2E3440` | Main background |
| Nord1 | `#3B4252` | Header + encoder bar |
| Nord3 | `#4C566A` | Dividers, inactive layers |
| Nord4 | `#D8DEE9` | Primary text |
| Nord8 | `#88C0D0` | Active layer, encoder labels (frost blue) |
| Nord13 | `#EBCB8B` | Caps Word active (amber) |

### Available fonts (compiled in)

| Font | Size | Used for |
|---|---|---|
| `FoundryGridnikMedium_20` | 20px | Header, encoder labels |
| `SF_Compact_Text_Bold_32` | 32px | Caps Word icon |
| `FRAC_Regular_48` | 48px | Active layer name |
| `FRAC_Thin_48` | 48px | Inactive layer names |

To enable more fonts: uncomment them in `boards/shields/prospector_adapter/include/fonts.h` and make sure the corresponding `.c` file exists in `src/fonts/`.

---

## Known issues / watch-outs

- **ZMK main is broken** — always pin to `v0.3` in `config/west.yml`
- **Ambient light sensor disabled** — `CONFIG_PROSPECTOR_USE_AMBIENT_LIGHT_SENSOR=n` in `klor_dongle.conf`. Without this the backlight stays off (ALS thread never sets brightness when sensor is absent)
- **Battery bar removed** — no batteries in this build
- **Idle indicator removed** — original widget had a broken `ZMK_EVENT_RAISE` call and broken idle logic (never triggered without keystrokes). Removed from UI
- **Encoder events over BLE** — encoder events from both peripheral halves are forwarded to the dongle via ZMK split protocol. The dongle's keymap `sensor-bindings` handle them. All layers use the same bindings: left = UP/DOWN (history), right = PG_UP/PG_DN (scroll)

---

## Next features to build

- [ ] WPM widget
- [ ] Layer-specific encoder hint labels (dynamic, not static)
- [ ] Connection status indicator (shows when a half is disconnected)
- [ ] Layer-specific display themes / accent colors
