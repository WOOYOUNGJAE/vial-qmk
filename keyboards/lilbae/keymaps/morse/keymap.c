/*
Copyright 2022 Bryan Ong

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include QMK_KEYBOARD_H

enum layer_names {
    _ENTER,
    _MORSE
};

enum custom_keycodes {
    BAE_ENT = SAFE_RANGE,
    BAE_MRS
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_ENTER] = LAYOUT(
                 KC_NO,
        KC_NO,   BAE_ENT, KC_NO),

    [_MORSE] = LAYOUT(
                 KC_NO,
        KC_NO,   BAE_MRS, KC_NO),
};

static const uint8_t PROGMEM morse_table[64] = {
    [2]  = KC_E, [3]  = KC_T,
    [4]  = KC_I, [5]  = KC_A, [6]  = KC_N, [7]  = KC_M,
    [8]  = KC_S, [9]  = KC_U, [10] = KC_R, [11] = KC_W,
    [12] = KC_D, [13] = KC_K, [14] = KC_G, [15] = KC_O,
    [16] = KC_H, [17] = KC_V, [18] = KC_F, [20] = KC_L,
    [22] = KC_P, [23] = KC_J, [24] = KC_B, [25] = KC_X,
    [26] = KC_C, [27] = KC_Y, [28] = KC_Z, [29] = KC_Q,
    [32] = KC_5, [33] = KC_4, [35] = KC_3, [39] = KC_2,
    [47] = KC_1, [48] = KC_6, [56] = KC_7, [60] = KC_8,
    [62] = KC_9, [63] = KC_0,
};

static uint16_t press_timer;
static uint16_t release_timer;
static uint8_t  tap_count;
static uint8_t  morse_len;
static uint8_t  morse_bits;
static bool     key_down;
static bool     enter_held;
static bool     word_pending;

static void reset_input(void) {
    tap_count    = 0;
    morse_len    = 0;
    morse_bits   = 0;
    word_pending = false;
}

static void toggle_layer(void) {
    reset_input();
    layer_move(get_highest_layer(layer_state) == _ENTER ? _MORSE : _ENTER);
}

static void flush_enters(void) {
    for (; tap_count > 0; tap_count--) {
        tap_code(KC_ENT);
    }
}

static void decode_morse(void) {
    if (morse_len > 0 && morse_len <= 5) {
        uint8_t kc = pgm_read_byte(&morse_table[(1 << morse_len) | morse_bits]);
        if (kc != KC_NO) {
            tap_code(kc);
            word_pending = true;
        }
    }
    morse_len  = 0;
    morse_bits = 0;
    tap_count  = 0;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (keycode == BAE_ENT || keycode == BAE_MRS) {
        key_down = record->event.pressed;
    }
    switch (keycode) {
        case BAE_ENT:
            if (record->event.pressed) {
                press_timer = timer_read();
            } else if (enter_held) {
                unregister_code(KC_ENT);
                enter_held = false;
            } else {
                if (++tap_count >= LAYER_TOGGLE_TAPS) {
                    toggle_layer();
                } else {
                    release_timer = timer_read();
                }
            }
            return false;

        case BAE_MRS:
            if (record->event.pressed) {
                press_timer = timer_read();
            } else {
                bool dash = timer_elapsed(press_timer) >= MORSE_DASH_MS;
                if (morse_len < 7) {
                    morse_bits = (morse_bits << 1) | dash;
                    morse_len++;
                }
                tap_count = dash ? 0 : tap_count + 1;
                if (tap_count >= LAYER_TOGGLE_TAPS && morse_len == tap_count) {
                    toggle_layer();
                } else {
                    release_timer = timer_read();
                }
            }
            return false;
    }
    return true;
}

void housekeeping_task_user(void) {
    bool pressed = key_down;

    if (get_highest_layer(layer_state) == _ENTER) {
        if (pressed &&!enter_held && timer_elapsed(press_timer) >= MORSE_DASH_MS) {
            flush_enters();
            register_code(KC_ENT);
            enter_held = true;
        } else if (!pressed && tap_count > 0 && timer_elapsed(release_timer) >= ENTER_TAP_GAP_MS) {
            flush_enters();
        }
        return;
    }

    if (pressed) {
        return;
    }
    if (morse_len > 0 && timer_elapsed(release_timer) >= MORSE_CHAR_GAP_MS) {
        decode_morse();
    } else if (word_pending && timer_elapsed(release_timer) >= MORSE_WORD_GAP_MS) {
        tap_code(KC_SPC);
        word_pending = false;
    }
}
