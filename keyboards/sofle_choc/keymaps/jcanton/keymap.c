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
    _SYMB,
    _ADJUST
};

enum custom_keycodes {
    KC_LOCK = QK_USER
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
[_QWERTY] = LAYOUT(
    KC_ESC,   KC_1,   KC_2,    KC_3,    KC_4,    KC_5,                     KC_6,   KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC,
    KC_TAB,   KC_Q,   KC_W,    KC_E,    KC_R,    KC_T,                     KC_Y,   KC_U,    KC_I,    KC_O,    KC_P,    KC_BSLS,
    KC_LCTL,  KC_A,   KC_S,    KC_D,    KC_F,    KC_G,                     KC_H,   KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
    KC_LSFT,  KC_Z,   KC_X,    KC_C,    KC_V,    KC_B,  KC_MUTE,   KC_MPLY,KC_N,   KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,
                      KC_LCTL, KC_LOPT, KC_LCMD, MO(_NAV), KC_ENT,  KC_SPC, MO(_SYMB), KC_RCMD, KC_ROPT, KC_RCTL
),
[_NAV] = LAYOUT(
    KC_GRV,   KC_F1,  KC_F2,   KC_F3,   KC_F4,   KC_F5,                    KC_F6,  KC_F7,   KC_F8,   KC_MINS, KC_EQL,  KC_DEL,
    KC_TAB,   XXXXXXX,XXXXXXX, KC_UP,   XXXXXXX, XXXXXXX,                  XXXXXXX,XXXXXXX, MS_UP,   KC_LBRC, KC_RBRC, XXXXXXX,
    KC_LCTL,  KC_BRIU,KC_LEFT, KC_DOWN, KC_RGHT, XXXXXXX,                  XXXXXXX,MS_LEFT, MS_DOWN, MS_RGHT, XXXXXXX, XXXXXXX,
    KC_LSFT,  KC_BRID,XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,  KC_LOCK, KC_F3, XXXXXXX,XXXXXXX, MS_BTN1, MS_BTN2, XXXXXXX, KC_RSFT,
                      KC_LCTL, KC_LOPT, KC_LCMD, _______, KC_ENT,   KC_SPC, _______, KC_RCTL, KC_ROPT, KC_RCMD
),
[_SYMB] = LAYOUT(
    _______, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                    KC_F6,  KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,
    KC_GRV,  KC_1,   KC_2,    KC_3,    KC_4,    KC_5,                      KC_6,   KC_7,    KC_8,    KC_9,    KC_0,    KC_F12,
    _______, KC_EXLM, KC_AT,  KC_HASH, KC_DLR,  KC_PERC,                   KC_CIRC,KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_PIPE,
    _______, KC_EQL,  KC_MINS, KC_PLUS, KC_LCBR, KC_RCBR, _______, _______,KC_LBRC, KC_SCLN, KC_COLN, KC_BSLS, XXXXXXX, _______,
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
    [_SYMB]    = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(KC_PGUP, KC_PGDN) },
    [_ADJUST]  = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(KC_PGUP, KC_PGDN) },
};
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
    }
    return true;
}
