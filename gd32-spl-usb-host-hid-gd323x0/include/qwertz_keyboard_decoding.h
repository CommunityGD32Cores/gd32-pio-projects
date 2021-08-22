#ifndef QWERTZ_KEYBOARD_DECODING_H_
#define QWERTZ_KEYBOARD_DECODING_H_

#include "gd32f3x0.h"
#include <stdint.h>
#include <stdbool.h>

const char* get_str_for_scancode_qwertz(uint8_t scancode, bool is_shift_pressed, bool is_alt_gr_pressed, bool* err);

#endif /* QWERTZ_KEYBOARD_DECODING_H_ */