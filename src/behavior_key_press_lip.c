/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_key_press_lip

#include <zephyr/device.h>
#include <drivers/behavior.h>
#include <zephyr/logging/log.h>
#include <zmk/behavior.h>
#include <zmk/hid.h>

#include <zmk/event_manager.h>
#include <zmk/events/keycode_state_changed.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

// NOTE: checked in Kconfig & CMakeLists.txt
// #if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

struct behavior_key_press_lip_config {
    uint8_t index;
    uint8_t keycodes_count;
    uint32_t keycodes[];
};

struct behavior_key_press_lip_data {
    uint8_t keycode_states_count;
    bool keycode_states[];
};

static int kp_lip_behavior_get_index(const struct device *dev, uint32_t binding_p1) {
    const struct behavior_key_press_lip_config *config = dev->config;
    for (int u = 0; u < config->keycodes_count; u++) {
        if (config->keycodes[u] == binding_p1) {
            return u;
        }
    }
    return -1;
}

static int on_key_press_lip_binding_pressed(struct zmk_behavior_binding *binding,
                                       struct zmk_behavior_binding_event event) {
    const struct device *dev = zmk_behavior_get_binding(binding->behavior_dev);
    struct behavior_key_press_lip_data *data = dev->data;

    uint8_t keycode_idx = kp_lip_behavior_get_index(dev, binding->param1);
    if (keycode_idx >= 0) {
        const struct behavior_key_press_lip_config *config = dev->config;
        for (int u = 0; u < config->keycodes_count; u++) {
            if (u == keycode_idx) {
                continue;
            }
            if (data->keycode_states[u]) {
                uint32_t cfg_keycode = config->keycodes[u];
                data->keycode_states[u] = false; // supressing next release
                LOG_DBG("lip release keycode 0x%02X %d", cfg_keycode, u);
                raise_zmk_keycode_state_changed_from_encoded(cfg_keycode, 
                                                             false, k_uptime_get());
            }
        }
        data->keycode_states[keycode_idx] = true;
    }

    LOG_DBG("press position %d keycode 0x%02X", event.position, binding->param1);
    return raise_zmk_keycode_state_changed_from_encoded(binding->param1, true, 
                                                        event.timestamp);
}

static int on_key_press_lip_binding_released(struct zmk_behavior_binding *binding,
                                        struct zmk_behavior_binding_event event) {
    const struct device *dev = zmk_behavior_get_binding(binding->behavior_dev);
    struct behavior_key_press_lip_data *data = dev->data;

    uint8_t keycode_idx = kp_lip_behavior_get_index(dev, binding->param1);
    if (keycode_idx >= 0) {
        if (!data->keycode_states[keycode_idx]) {
            LOG_DBG("supressed");
            return ZMK_BEHAVIOR_OPAQUE;
        }
        data->keycode_states[keycode_idx] = false;
    }

    LOG_DBG("release position %d keycode 0x%02X", event.position, binding->param1);
    return raise_zmk_keycode_state_changed_from_encoded(binding->param1, false,
                                                        event.timestamp);
}

static const struct behavior_driver_api behavior_key_press_lip_driver_api = {
    .binding_pressed = on_key_press_lip_binding_pressed,
    .binding_released = on_key_press_lip_binding_released,
};

static int behavior_key_press_lip_init(const struct device *dev) {
    const struct behavior_key_press_lip_config *config = dev->config;
    struct behavior_key_press_lip_data *data = dev->data;
    return 0;
}

#define K0SOCD_EXTRACT_KC_STATE(idx, drv_inst) (false)

#define K0SOCD_INST(n)                                                                           \
    static struct behavior_key_press_lip_data behavior_key_press_lip_data_##n = {                \
        .keycode_states_count = DT_INST_PROP(n, keycodes),                                       \
        .keycode_states = COND_CODE_1(                                                           \
            DT_INST_NODE_HAS_PROP(n, keycodes),                                                  \
            ({LISTIFY(DT_INST_PROP_LEN(n, keycodes), K0SOCD_EXTRACT_KC_STATE, (, ), n)}),        \
            ({})),                                                                               \
    };                                                                                           \
    static struct behavior_key_press_lip_config behavior_key_press_lip_config_##n = {            \
        .index = n,                                                                              \
        .keycodes = DT_INST_PROP(n, keycodes),                                                   \
        .keycodes_count = DT_INST_PROP_LEN(n, keycodes),                                         \
    };                                                                                           \
    BEHAVIOR_DT_INST_DEFINE(n, behavior_key_press_lip_init, NULL,                                \
                            &behavior_key_press_lip_data_##n,                                    \
                            &behavior_key_press_lip_config_##n,                                  \
                            POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,                    \
                            &behavior_key_press_lip_driver_api);

DT_INST_FOREACH_STATUS_OKAY(K0SOCD_INST)

// #endif /* DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT) */
