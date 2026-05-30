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

enum layer_names {
    _QWERTY,
    _NAV,
    _LEDS,
    _ADJUST
};

enum custom_keycodes {
    KC_LOCK = QK_USER,
    KC_KEY_BRI_UP,
    KC_KEY_BRI_DN,
    KC_UGL_BRI_UP,
    KC_UGL_BRI_DN,
};

static uint8_t key_brightness = 255;
static uint8_t ugl_brightness = 128;

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
    KC_LCTL,  XXXXXXX,KC_LEFT, KC_DOWN, KC_RGHT, XXXXXXX,                  XXXXXXX,MS_LEFT, MS_DOWN, MS_RGHT, XXXXXXX, XXXXXXX,
    KC_LSFT,  XXXXXXX,XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,  KC_LOCK, KC_F3, XXXXXXX,MS_BTN1, MS_BTN2, XXXXXXX, KC_RSFT,
                      KC_LCTL, KC_LOPT, KC_LCMD, _______, KC_ENT,   KC_SPC, _______, KC_RCTL, KC_ROPT, KC_RCMD
),
[_LEDS] = LAYOUT(
    _______, _______, _______, _______, _______, _______,                   _______, _______, _______, _______, _______, _______,
    _______, RM_NEXT, RM_PREV, RM_HUEU, RM_HUED, RM_VALU,                  RM_VALD, RM_SATU, RM_SATD, RM_SPDU, RM_SPDD, RM_TOGG,
    _______, KC_KEY_BRI_UP, KC_KEY_BRI_DN, _______, _______, _______,     _______, KC_UGL_BRI_UP, KC_UGL_BRI_DN, _______, _______, _______,
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
    [_QWERTY] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(KC_PGUP, KC_PGDN) },
    [_NAV]     = { ENCODER_CCW_CW(MS_WHLU, MS_WHLD), ENCODER_CCW_CW(KC_MPRV, KC_MNXT) },
    [_LEDS]    = { ENCODER_CCW_CW(KC_KEY_BRI_DN, KC_KEY_BRI_UP), ENCODER_CCW_CW(KC_UGL_BRI_DN, KC_UGL_BRI_UP) },
    [_ADJUST]  = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(KC_PGUP, KC_PGDN) },
};
#endif

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    for (uint8_t i = led_min; i < led_max; i++) {
        if (g_led_config.flags[i] & LED_FLAG_KEYLIGHT) {
            rgb_matrix_set_color(i, key_brightness, key_brightness, key_brightness);
        } else if (g_led_config.flags[i] & LED_FLAG_UNDERGLOW) {
            rgb_matrix_set_color(i, ugl_brightness, ugl_brightness, ugl_brightness);
        }
    }
    return false;
}

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
        case KC_KEY_BRI_UP:
            if (record->event.pressed && key_brightness < 255 - 31) key_brightness += 31;
            return false;
        case KC_KEY_BRI_DN:
            if (record->event.pressed && key_brightness > 31) key_brightness -= 31;
            return false;
        case KC_UGL_BRI_UP:
            if (record->event.pressed && ugl_brightness < 255 - 31) ugl_brightness += 31;
            return false;
        case KC_UGL_BRI_DN:
            if (record->event.pressed && ugl_brightness > 31) ugl_brightness -= 31;
            return false;
    }
    return true;
}
