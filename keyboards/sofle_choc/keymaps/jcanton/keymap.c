/* Copyright 2025 Jacopo Canton
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "config.h"
#include QMK_KEYBOARD_H
#include <raw_hid.h>

enum layer_names {
    _QWERTY,
    _NAV,
    _MOUSE,
    _SYM,
    _FUN,
    _NUM,
    _LEDS,
};

enum custom_keycodes {
    KC_LOCK = QK_USER,
    KC_LED_MODE,
    KC_VIZ_TOGGLE,
    KC_FUNLEDS,
    KC_NAVMOU,
    KC_SYMNUM,
};

static const uint8_t led_modes[] = {LED_FLAG_ALL, LED_FLAG_KEYLIGHT, LED_FLAG_UNDERGLOW, 0};
static const uint8_t led_mode_count = sizeof(led_modes) / sizeof(led_modes[0]);
static uint8_t led_mode_idx = 0;
static bool layer_changed = false;
static bool funleds_shift_used = false;
static uint8_t navmou_layer = 0;
static uint8_t symnum_layer = 0;
static uint8_t del_keycode = KC_BSPC;
static uint16_t lshift_press_time = 0;
static uint16_t lshift_release_time = 0;
static bool     lshift_pending = false;
static bool     lshift_tapped = false;

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
[_QWERTY] = LAYOUT(
    KC_ESC,   KC_1,   KC_2,   KC_3,    KC_4,    KC_5,                                 KC_6,       KC_7,    KC_8,    KC_9,   KC_0,    KC_BSPC,
    KC_TAB,   KC_Q,   KC_W,   KC_E,    KC_R,    KC_T,                                 KC_Y,       KC_U,    KC_I,    KC_O,   KC_P,    KC_BSLS,
    KC_LCTL,  KC_A,   KC_S,   KC_D,    KC_F,    KC_G,                                 KC_H,       KC_J,    KC_K,    KC_L,   KC_SCLN, KC_QUOT,
    KC_LSFT,  KC_Z,   KC_X,   KC_C,    KC_V,    KC_B, KC_MUTE,               KC_MPLY, KC_N,       KC_M,    KC_COMM, KC_DOT, KC_SLSH, KC_RSFT,
              MT(MOD_LCTL, KC_GRV), KC_LOPT, KC_LCMD, KC_NAVMOU, KC_ENT, KC_SPC, KC_SYMNUM, KC_FUNLEDS, MT(MOD_RALT, KC_LBRC), MT(MOD_RCTL, KC_RBRC)
),
[_NAV] = LAYOUT(
    _______, _______, _______, _______, _______, _______,                    _______, _______, _______, KC_MINS, KC_EQL,  KC_VIZ_TOGGLE,
    _______, KC_BRID, KC_BRIU, KC_MCTL, _______, _______,                    C(KC_Z), C(KC_X), C(KC_C), C(KC_V), C(KC_Y), _______,
    _______, _______, _______, _______, _______, _______,                    KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, _______, KC_CAPS,
    _______, _______, _______, _______, _______, _______, _______,  _______, KC_HOME, KC_PGDN, KC_PGUP, KC_END,  _______, KC_INS,
              MT(MOD_LCTL, KC_GRV), KC_LOPT, KC_LCMD, KC_NAVMOU, KC_ENT,     KC_SPC, KC_SYMNUM, KC_FUNLEDS, MT(MOD_RALT, KC_LBRC), MT(MOD_RCTL, KC_RBRC)
),
[_MOUSE] = LAYOUT(
    _______, _______, _______, _______, _______, _______,                    _______, _______, _______, KC_MINS, KC_EQL,  _______,
    _______, _______, _______, _______, _______, _______,                    C(KC_Z), C(KC_X), C(KC_C), C(KC_V), C(KC_Y), _______,
    _______, _______, _______, _______, _______, _______,                    MS_LEFT, MS_DOWN, MS_UP,   MS_RGHT, _______, _______,
    _______, _______, _______, _______, _______, _______, _______,  _______, MS_WHLL, MS_WHLU, MS_WHLD, MS_WHLR, _______, _______,
              MT(MOD_LCTL, KC_GRV), KC_LOPT, KC_LCMD, KC_NAVMOU, KC_ENT,    MS_BTN1, MS_BTN2, MS_BTN3, MT(MOD_RALT, KC_LBRC), MT(MOD_RGUI, KC_RBRC)
),
[_SYM] = LAYOUT(
    _______, _______, _______, _______, _______, _______,                    _______, _______, _______, _______, _______, _______,
    _______, KC_GRV,  KC_LCBR, KC_RCBR, KC_LBRC, KC_RBRC,                    _______, _______, _______, _______, _______, KC_BSLS,
    _______, KC_TILD, KC_LPRN, KC_RPRN, KC_LT,   KC_GT,                      _______, _______, _______, _______, _______, KC_QUES,
    _______, KC_LSFT, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC,  _______, _______, _______, _______, _______, _______, _______,
              MT(MOD_LCTL, KC_GRV), KC_LOPT, KC_LCMD, KC_NAVMOU, KC_ENT,    KC_SPC, KC_SYMNUM, KC_FUNLEDS, MT(MOD_RALT, KC_LBRC), MT(MOD_RCTL, KC_RBRC)
),
[_FUN] = LAYOUT(
    _______, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                      KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,
    _______, KC_BRID, KC_BRIU, KC_MCTL, _______, _______,                    _______, _______, _______, _______, _______, KC_F12,
    _______, _______, _______, _______, _______, _______,                    _______, _______, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______, _______,  _______, _______, _______, _______, _______, _______, _______,
              MT(MOD_LCTL, KC_GRV), KC_LOPT, KC_LCMD, KC_NAVMOU, KC_ENT,    KC_SPC, KC_SYMNUM, KC_FUNLEDS, MT(MOD_RALT, KC_LBRC), MT(MOD_RCTL, KC_RBRC)
),
[_NUM] = LAYOUT(
    _______, _______, _______, _______, _______, _______,                    _______, _______, _______, _______, _______, _______,
    _______, KC_7,    KC_8,    KC_9,    KC_MINS, KC_PLUS,                    _______, _______, _______, _______, _______, _______,
    _______, KC_4,    KC_5,    KC_6,    KC_ASTR, KC_SLSH,                    _______, _______, _______, _______, _______, _______,
    _______, KC_1,    KC_2,    KC_3,    KC_0,    KC_DOT,  _______,  _______, _______, _______, _______, _______, _______, _______,
              MT(MOD_LCTL, KC_GRV), KC_LOPT, KC_LCMD, KC_NAVMOU, KC_ENT,    KC_SPC, KC_SYMNUM, KC_FUNLEDS, MT(MOD_RALT, KC_LBRC), MT(MOD_RCTL, KC_RBRC)
),
[_LEDS] = LAYOUT(
    _______, _______,     _______, _______, _______, _______,                    _______, _______, _______, _______, _______, _______,
    _______, KC_LED_MODE, _______, _______, _______, _______,                    _______, _______, _______, _______, _______, _______,
    _______, RM_NEXT,     RM_PREV, RM_HUEU, RM_HUED, RM_VALU,                    RM_VALD, RM_SATU, RM_SATD, RM_SPDU, RM_SPDD, RM_TOGG,
    _______, _______,     _______, _______, _______, _______, _______,  _______, _______, _______, _______, _______, _______, _______,
              MT(MOD_LCTL, KC_GRV), KC_LOPT, KC_LCMD, KC_NAVMOU, KC_ENT,    KC_SPC, KC_SYMNUM, KC_FUNLEDS, MT(MOD_RALT, KC_LBRC), MT(MOD_RCTL, KC_RBRC)
),
};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [_QWERTY] = { ENCODER_CCW_CW(KC_VOLU, KC_VOLD), ENCODER_CCW_CW(KC_MPRV, KC_MNXT) },
    [_NAV]    = { ENCODER_CCW_CW(MS_WHLU, MS_WHLD), ENCODER_CCW_CW(KC_PGUP, KC_PGDN) },
    [_MOUSE]  = { ENCODER_CCW_CW(MS_WHLU, MS_WHLD), ENCODER_CCW_CW(KC_MPRV, KC_MNXT) },
    [_SYM]    = { ENCODER_CCW_CW(KC_VOLU, KC_VOLD), ENCODER_CCW_CW(KC_PGUP, KC_PGDN) },
    [_FUN]    = { ENCODER_CCW_CW(KC_BRID, KC_BRIU), ENCODER_CCW_CW(KC_VOLD, KC_VOLU) },
    [_NUM]    = { ENCODER_CCW_CW(KC_VOLU, KC_VOLD), ENCODER_CCW_CW(KC_PGUP, KC_PGDN) },
    [_LEDS]   = { ENCODER_CCW_CW(RM_VALD, RM_VALU), ENCODER_CCW_CW(KC_LED_MODE, RM_NEXT) },
};
#endif

#ifdef OLED_ENABLE
oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    return OLED_ROTATION_270;
}

// static void render_logo(void) {
//     static const char PROGMEM raw_logo[] = {
//         0,  0,  0,  0,  0,  0,  0,  0,  0,128, 64, 32, 16, 16,  8,  8,  8,  8,
//         16, 16, 32, 64,128,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//         0,  0,  0,  0,254,  1,  0, 24, 36, 36, 24,  0,  0,  0, 24, 36, 36, 24,
//         1,254,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//         31, 32, 24,  4, 24, 32, 24,  4, 24, 32, 24,  4, 24, 32, 24,  7,  0,  0,
//         0,  0,  0,  0,  0,  0,
//     };
//     oled_write_raw_P(raw_logo, sizeof(raw_logo));
// }

static void render_master_status(void) {
    uint8_t current = get_highest_layer(layer_state);
    oled_write_P(PSTR("QWRTY"), current == _QWERTY);
    oled_write_ln_P(PSTR(""), false);
    oled_write_P(PSTR("NAV  "), current == _NAV);
    oled_write_ln_P(PSTR(""), false);
    oled_write_P(PSTR("MOUSE"), current == _MOUSE);
    oled_write_ln_P(PSTR(""), false);
    oled_write_P(PSTR("SYM  "), current == _SYM);
    oled_write_ln_P(PSTR(""), false);
    oled_write_P(PSTR("FUN  "), current == _FUN);
    oled_write_ln_P(PSTR(""), false);
    oled_write_P(PSTR("NUM  "), current == _NUM);
    oled_write_ln_P(PSTR(""), false);
    oled_write_P(PSTR("LEDS "), current == _LEDS);
    oled_write_ln_P(PSTR(""), false);
    led_t led_state = host_keyboard_led_state();
    oled_write_ln_P(PSTR("CPSLK"), led_state.caps_lock);
    // oled_write_ln_P(PSTR(""), false);
    // render_logo();
}

static void render_slave_status(void) {
    oled_write_ln_P(PSTR("LED"), false);
    oled_write_ln_P(PSTR("MODE"), false);
    oled_write_P(PSTR(""), false);
    uint8_t flags = rgb_matrix_get_flags();
    if (flags == LED_FLAG_ALL) {
        oled_write_ln_P(PSTR("All"), false);
    } else if (flags & LED_FLAG_KEYLIGHT) {
        oled_write_ln_P(PSTR("Keys"), false);
    } else if (flags & LED_FLAG_UNDERGLOW) {
        oled_write_ln_P(PSTR("Glow"), false);
    } else {
        oled_write_ln_P(PSTR("Off"), false);
    }
    oled_write_ln_P(PSTR(""), false);
    const char *name = rgb_matrix_get_mode_name(rgb_matrix_get_mode());
    char buf[6] = {0};
    for (uint8_t i = 0; i < 5 && name[i]; i++) buf[i] = name[i];
    oled_write_ln(buf, false);
    oled_write_ln_P(PSTR(""), false);
    oled_write_ln_P(PSTR(""), false);
    oled_write_P(PSTR("/\\_/") , false);
    oled_write_ln_P(PSTR("\\") , false);
    oled_write_P(PSTR("(o.o)") , false);
    oled_write_ln_P(PSTR(""), false);
    oled_write_P(PSTR(" >^<")  , false);
    oled_write_ln_P(PSTR(""), false);
}

bool oled_task_user(void) {
    if (is_keyboard_master()) {
        render_master_status();
    } else {
        render_slave_status();
    }
    return false;
}
#endif

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed && keycode != KC_LSFT) {
        lshift_pending = false;
        if (keycode != KC_NAVMOU) {
            lshift_tapped = false;
        }
    }
    switch (keycode) {
        case KC_LSFT:
            if (record->event.pressed) {
                lshift_press_time = timer_read();
                lshift_pending = true;
            } else {
                if (lshift_pending && timer_elapsed(lshift_press_time) < TAPPING_TERM) {
                    lshift_tapped = true;
                    lshift_release_time = timer_read();
                }
                lshift_pending = false;
            }
            return true;
        case KC_BSPC:
            if (record->event.pressed) {
                uint8_t mods = get_mods();
                if (mods & MOD_MASK_SHIFT) {
                    del_keycode = KC_DEL;
                    unregister_mods(MOD_MASK_SHIFT);
                    register_code(KC_DEL);
                    set_mods(mods);
                } else {
                    del_keycode = KC_BSPC;
                    register_code(KC_BSPC);
                }
            } else {
                unregister_code(del_keycode);
            }
            return false;
        case KC_LOCK:
            if (record->event.pressed) {
                register_mods(MOD_LGUI | MOD_LCTL);
                register_code(KC_Q);
            } else {
                unregister_code(KC_Q);
                unregister_mods(MOD_LGUI | MOD_LCTL);
            }
            return false;
        case KC_LED_MODE:
            if (record->event.pressed) {
                led_mode_idx = (led_mode_idx + 1) % led_mode_count;
                rgb_matrix_set_flags(led_modes[led_mode_idx]);
                if (led_modes[led_mode_idx] == 0) {
                    rgb_matrix_set_color_all(0, 0, 0);
                }
            }
            return false;
        case KC_VIZ_TOGGLE:
            if (record->event.pressed) {
                uint8_t response[32] = {0};
                response[0] = 0x81;
                raw_hid_send(response, 32);
            }
            return false;
        case KC_FUNLEDS:
            if (record->event.pressed) {
                if (get_mods() & MOD_MASK_SHIFT) {
                    layer_invert(_LEDS);
                    funleds_shift_used = true;
                } else {
                    funleds_shift_used = false;
                    layer_on(_FUN);
                }
            } else {
                if (!funleds_shift_used) {
                    layer_off(_FUN);
                }
            }
            return false;
        case KC_NAVMOU:
            if (record->event.pressed) {
                if (lshift_tapped && timer_elapsed(lshift_release_time) < TAPPING_TERM) {
                    navmou_layer = _MOUSE;
                } else {
                    navmou_layer = _NAV;
                }
                lshift_tapped = false;
                layer_on(navmou_layer);
            } else {
                layer_off(navmou_layer);
            }
            return false;
        case KC_SYMNUM:
            if (record->event.pressed) {
                symnum_layer = (get_mods() & MOD_BIT(KC_RSFT)) ? _NUM : _SYM;
                layer_on(symnum_layer);
            } else {
                layer_off(symnum_layer);
            }
            return false;
    }
    return true;
}

layer_state_t layer_state_set_user(layer_state_t state) {
    layer_changed = true;
    return state;
}

void housekeeping_task_user(void) {
    if (layer_changed) {
        layer_changed = false;
        uint8_t response[32] = {0};
        response[0] = 0x80;
        response[1] = get_highest_layer(layer_state);
        raw_hid_send(response, 32);
    }
}

void raw_hid_receive(uint8_t *data, uint8_t length) {
    if (data[0] == 0x80) {
        uint8_t response[32] = {0};
        response[0] = 0x80;
        response[1] = get_highest_layer(layer_state);
        raw_hid_send(response, 32);
    }
}
