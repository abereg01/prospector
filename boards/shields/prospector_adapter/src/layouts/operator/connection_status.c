#include "connection_status.h"

#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/events/split_central_status_changed.h>

#include "display_colors.h"

// Connection status indicator: one small circle per split peripheral slot.
// Each circle reflects the most recent zmk_split_central_status_changed event
// for that slot. At boot, before any event has arrived, the indicators show
// in the "unknown" color (dim Nord3) so we don't lie either way.
//
// Slot ordering matches the BLE pairing order — for KLOR that's left then
// right, so indicator[0] is the left half and indicator[1] is the right half.

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

enum slot_visual_state {
    SLOT_UNKNOWN = 0,
    SLOT_DISCONNECTED,
    SLOT_CONNECTED,
};

static enum slot_visual_state slot_state[ZMK_SPLIT_BLE_PERIPHERAL_COUNT];

struct connection_status_state {
    enum slot_visual_state slots[ZMK_SPLIT_BLE_PERIPHERAL_COUNT];
};

static uint32_t color_for_slot_state(enum slot_visual_state s) {
    switch (s) {
        case SLOT_CONNECTED:    return DISPLAY_COLOR_CONN_CONNECTED;
        case SLOT_DISCONNECTED: return DISPLAY_COLOR_CONN_DISCONNECTED;
        case SLOT_UNKNOWN:
        default:                return DISPLAY_COLOR_CONN_UNKNOWN;
    }
}

static void connection_status_update_cb(struct connection_status_state state) {
    struct zmk_widget_connection_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        for (int i = 0; i < ZMK_SPLIT_BLE_PERIPHERAL_COUNT; i++) {
            lv_obj_set_style_bg_color(widget->indicators[i],
                lv_color_hex(color_for_slot_state(state.slots[i])), LV_PART_MAIN);
        }
    }
}

static struct connection_status_state connection_status_get_state(const zmk_event_t *eh) {
    if (eh != NULL) {
        const struct zmk_split_central_status_changed *ev = as_zmk_split_central_status_changed(eh);
        if (ev != NULL && ev->slot < ZMK_SPLIT_BLE_PERIPHERAL_COUNT) {
            slot_state[ev->slot] = ev->connected ? SLOT_CONNECTED : SLOT_DISCONNECTED;
        }
    }

    struct connection_status_state state;
    for (int i = 0; i < ZMK_SPLIT_BLE_PERIPHERAL_COUNT; i++) {
        state.slots[i] = slot_state[i];
    }
    return state;
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_connection_status, struct connection_status_state,
                            connection_status_update_cb, connection_status_get_state)
ZMK_SUBSCRIPTION(widget_connection_status, zmk_split_central_status_changed);

int zmk_widget_connection_status_init(struct zmk_widget_connection_status *widget, lv_obj_t *parent) {
    widget->obj = lv_obj_create(parent);

    int dot_size = 10;
    int dot_gap = 4;
    int total_width = ZMK_SPLIT_BLE_PERIPHERAL_COUNT * dot_size +
                      (ZMK_SPLIT_BLE_PERIPHERAL_COUNT - 1) * dot_gap;

    lv_obj_set_size(widget->obj, total_width, dot_size);
    lv_obj_set_style_bg_opa(widget->obj, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(widget->obj, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(widget->obj, 0, LV_PART_MAIN);

    for (int i = 0; i < ZMK_SPLIT_BLE_PERIPHERAL_COUNT; i++) {
        widget->indicators[i] = lv_obj_create(widget->obj);
        lv_obj_set_size(widget->indicators[i], dot_size, dot_size);
        lv_obj_set_pos(widget->indicators[i], i * (dot_size + dot_gap), 0);
        lv_obj_set_style_bg_color(widget->indicators[i],
            lv_color_hex(DISPLAY_COLOR_CONN_UNKNOWN), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(widget->indicators[i], LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_set_style_border_width(widget->indicators[i], 0, LV_PART_MAIN);
        lv_obj_set_style_radius(widget->indicators[i], LV_RADIUS_CIRCLE, LV_PART_MAIN);
        lv_obj_set_style_pad_all(widget->indicators[i], 0, LV_PART_MAIN);
    }

    sys_slist_append(&widgets, &widget->node);
    widget_connection_status_init();

    return 0;
}

lv_obj_t *zmk_widget_connection_status_obj(struct zmk_widget_connection_status *widget) {
    return widget->obj;
}
