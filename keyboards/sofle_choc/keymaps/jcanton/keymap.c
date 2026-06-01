/* Copyright 2025 Brian Low
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
#include QMK_KEYBOARD_H
#include <raw_hid.h>

enum layer_names {
    _QWERTY,
    _NAV,
    _LEDS,
    _ADJUST
};

enum custom_keycodes {
    KC_LOCK = QK_USER,
    KC_LED_MODE,
    KC_VIZ_TOGGLE,
};

static const uint8_t led_modes[] = {LED_FLAG_ALL, LED_FLAG_KEYLIGHT, LED_FLAG_UNDERGLOW, 0};
static const uint8_t led_mode_count = sizeof(led_modes) / sizeof(led_modes[0]);
static uint8_t led_mode_idx = 0;
static bool layer_changed = false;

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
[_QWERTY] = LAYOUT(
    KC_ESC,   KC_1,   KC_2,    KC_3,    KC_4,    KC_5,                     KC_6,   KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC,
    KC_TAB,   KC_Q,   KC_W,    KC_E,    KC_R,    KC_T,                     KC_Y,   KC_U,    KC_I,    KC_O,    KC_P,    KC_BSLS,
    KC_LCTL,  KC_A,   KC_S,    KC_D,    KC_F,    KC_G,                     KC_H,   KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
    KC_LSFT,  KC_Z,   KC_X,    KC_C,    KC_V,    KC_B,  KC_MUTE,   KC_MPLY,KC_N,   KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,
                      KC_LCTL, KC_LOPT, KC_LCMD, MO(_NAV), KC_ENT,  KC_SPC, MO(_LEDS), KC_RCMD, KC_ROPT, KC_RCTL
),
[_NAV] = LAYOUT(
    KC_GRV,   KC_F1,  KC_F2,   KC_F3,   KC_F4,   KC_F5,                    KC_F6,  KC_F7,   KC_F8,   KC_MINS, KC_EQL,  KC_DEL,
    KC_TAB,   XXXXXXX,XXXXXXX, KC_UP,   XXXXXXX, XXXXXXX,                  XXXXXXX,XXXXXXX, MS_UP,   KC_LBRC, KC_RBRC, XXXXXXX,
    KC_LCTL,  KC_BRMU,KC_LEFT, KC_DOWN, KC_RGHT, XXXXXXX,                  XXXXXXX,MS_LEFT, MS_DOWN, MS_RGHT, XXXXXXX, XXXXXXX,
    KC_LSFT,  KC_BRMD,XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,  KC_LOCK, KC_F3, XXXXXXX,MS_BTN1, MS_BTN2, XXXXXXX, XXXXXXX, KC_RSFT,
                      KC_LCTL, KC_LOPT, KC_LCMD, _______, KC_ENT,   KC_SPC, _______, KC_RCTL, KC_ROPT, KC_RCMD
),
[_LEDS] = LAYOUT(
    _______, _______,  _______,  _______, _______, _______,                   _______, _______, _______, _______, _______, KC_VIZ_TOGGLE,
    _______, RM_NEXT, RM_PREV, RM_HUEU, RM_HUED, RM_VALU,                  RM_VALD, RM_SATU, RM_SATD, RM_SPDU, RM_SPDD, RM_TOGG,
    _______, KC_LED_MODE, _______, _______, _______, _______,                _______, KC_LED_MODE, _______, _______, _______, _______,
    _______, _______, _______, _______, _______, _______,  _______, _______, _______, _______, _______, _______, _______, _______,
                      _______, _______, _______, _______, _______,  _______, _______, _______, _______, _______
),
[_ADJUST] = LAYOUT(
    _______, _______, _______, _______, _______, _______,                   _______, _______, _______, _______, _______, _______,
    _______, KC_INS,  KC_PSCR, KC_APP,  XXXXXXX, XXXXXXX,                  KC_PGUP, _______, KC_UP,   _______, _______, KC_BSPC,
    _______, KC_LOPT, KC_LCTL, KC_LSFT, XXXXXXX, KC_CAPS,                  KC_PGDN, KC_LEFT, KC_DOWN, KC_RGHT, KC_DEL,  KC_BSPC,
    _______, KC_UNDO, KC_CUT,  KC_COPY, KC_PASTE,XXXXXXX,  _______, _______,XXXXXXX, _______, XXXXXXX, _______, XXXXXXX, _______,
                      _______, _______, _______, _______, _______,  _______, _______, _______, _______, _______
)
};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [_QWERTY] = { ENCODER_CCW_CW(KC_VOLU, KC_VOLD), ENCODER_CCW_CW(KC_PGUP, KC_PGDN) },
    [_NAV]     = { ENCODER_CCW_CW(MS_WHLU, MS_WHLD), ENCODER_CCW_CW(KC_MPRV, KC_MNXT) },
    [_LEDS]    = { ENCODER_CCW_CW(RM_VALD, RM_VALU), ENCODER_CCW_CW(KC_LED_MODE, RM_NEXT) },
    [_ADJUST]  = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(KC_PGUP, KC_PGDN) },
};
#endif

#ifdef OLED_ENABLE
oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    return OLED_ROTATION_270;
}

static void render_master_status(void) {
    oled_write_P(PSTR("\n\n"), false);
    switch (get_highest_layer(layer_state)) {
        case _QWERTY:
            oled_write_ln_P(PSTR("QWRTY"), false);
            break;
        case _NAV:
            oled_write_ln_P(PSTR("NAV  "), false);
            break;
        case _LEDS:
            oled_write_ln_P(PSTR("LEDS "), false);
            break;
        case _ADJUST:
            oled_write_ln_P(PSTR("ADJST"), false);
            break;
        default:
            oled_write_ln_P(PSTR("?????"), false);
            break;
    }
    oled_write_P(PSTR("\n\n"), false);
    oled_write_ln_P(PSTR("LAYER"), false);
    switch (get_highest_layer(layer_state)) {
        case _QWERTY:
            oled_write_ln_P(PSTR("Base "), false);
            break;
        case _NAV:
            oled_write_ln_P(PSTR("Nav  "), false);
            break;
        case _LEDS:
            oled_write_ln_P(PSTR("LEDs "), false);
            break;
        case _ADJUST:
            oled_write_ln_P(PSTR("Adjst"), false);
            break;
        default:
            oled_write_ln_P(PSTR("?????"), false);
            break;
    }
    oled_write_P(PSTR("\n\n"), false);
    led_t led_state = host_keyboard_led_state();
    oled_write_ln_P(PSTR("CPSLK"), led_state.caps_lock);
}

static void render_slave_status(void) {
    oled_write_P(PSTR("\n\n\n"), false);
    oled_write_ln_P(PSTR("LED"), false);
    oled_write_ln_P(PSTR("MODE"), false);
    oled_write_P(PSTR("\n"), false);
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
    switch (keycode) {
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

void raw_hid_receive_user(uint8_t *data, uint8_t length) {
    if (data[0] == 0x80) {
        uint8_t response[32] = {0};
        response[0] = 0x80;
        response[1] = get_highest_layer(layer_state);
        raw_hid_send(response, 32);
    }
}
