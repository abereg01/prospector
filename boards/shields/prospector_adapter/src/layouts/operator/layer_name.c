#include "layer_name.h"

#include <ctype.h>
#include <zmk/display.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/event_manager.h>
#include <zmk/keymap.h>

#include <fonts.h>
#include "display_colors.h"

// Standalone layer-name widget. Previously the layer name was rendered as a
// label inside wpm_meter.c overlapping the bar chart — pulled out into its own
// zone so it can be larger and the color can track the per-layer accent.

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct layer_name_state {
    uint8_t index;
};

static void layer_name_update_cb(struct layer_name_state state) {
    struct zmk_widget_layer_name *widget;
    uint32_t color = display_color_for_layer(state.index);
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        const char *layer_name = zmk_keymap_layer_name(zmk_keymap_layer_index_to_id(state.index));
        char display_name[32];

        if (layer_name && *layer_name) {
            snprintf(display_name, sizeof(display_name), "%s", layer_name);
        } else {
            snprintf(display_name, sizeof(display_name), "Layer %d", state.index);
        }

#if IS_ENABLED(CONFIG_PROSPECTOR_LAYER_NAME_UPPERCASE)
        for (int i = 0; display_name[i]; i++) {
            display_name[i] = toupper((unsigned char)display_name[i]);
        }
#endif

        lv_label_set_text(widget->label, display_name);
        lv_obj_set_style_text_color(widget->label, lv_color_hex(color), LV_PART_MAIN);
    }
}

static struct layer_name_state layer_name_get_state(const zmk_event_t *eh) {
    return (struct layer_name_state){
        .index = zmk_keymap_highest_layer_active(),
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_layer_name, struct layer_name_state,
                            layer_name_update_cb, layer_name_get_state)
ZMK_SUBSCRIPTION(widget_layer_name, zmk_layer_state_changed);

int zmk_widget_layer_name_init(struct zmk_widget_layer_name *widget, lv_obj_t *parent) {
    widget->obj = lv_obj_create(parent);
    lv_obj_set_size(widget->obj, 220, 40);
    lv_obj_set_style_bg_opa(widget->obj, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(widget->obj, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(widget->obj, 0, LV_PART_MAIN);

    widget->label = lv_label_create(widget->obj);
    lv_label_set_text(widget->label, "");
    lv_obj_set_style_text_font(widget->label, &DINishExpanded_Light_36, LV_PART_MAIN);
    lv_obj_set_style_text_color(widget->label, lv_color_hex(DISPLAY_COLOR_LAYER_TEXT), LV_PART_MAIN);
    lv_obj_align(widget->label, LV_ALIGN_CENTER, 0, 0);

    sys_slist_append(&widgets, &widget->node);
    widget_layer_name_init();

    return 0;
}

lv_obj_t *zmk_widget_layer_name_obj(struct zmk_widget_layer_name *widget) {
    return widget->obj;
}
