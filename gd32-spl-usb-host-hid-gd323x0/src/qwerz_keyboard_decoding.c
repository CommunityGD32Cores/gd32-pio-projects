#include "qwertz_keyboard_decoding.h"
#include <usbh_hid_keybd.h>

#define CASE_SCANCODE(scancode, decoded_str) case (scancode): return (decoded_str)

/* german keyboard layout (QWERTZ) */
/*
* ┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───────┐
* │ ^ │ 1 │ 2 │ 3 │ 4 │ 5 │ 6 │ 7 │ 8 │ 9 │ 0 │ ß │ ´ │       │
* ├───┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─────┤
* │     │ Q │ W │ E │ R │ T │ Z │ U │ I │ O │ P │ Ü │ + │     │
* ├─────┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┐    │
* │      │ A │ S │ D │ F │ G │ H │ J │ K │ L │ Ö │ Ä │ # │    │
* ├────┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴───┴────┤
* │    │ < │ Y │ X │ C │ V │ B │ N │ M │ , │ . │ - │          │
* ├────┼───┴┬──┴─┬─┴───┴───┴───┴───┴───┴──┬┴───┼───┴┬────┬────┤
* │    │    │    │                        │    │    │    │    │
* └────┴────┴────┴────────────────────────┴────┴────┴────┴────┘
*/

const char* get_str_for_scancode_qwertz(uint8_t scancode, bool is_shift_pressed, bool is_alt_gr_pressed, bool* err) {
    if(!err) return "";

    //decode special keys which are always the same
    switch(scancode) {
        CASE_SCANCODE(KEY_ERRORROLLOVER, "<ROLLOVER>");
        CASE_SCANCODE(KEY_POSTFAIL, "<POSTFAIL>");
        CASE_SCANCODE(KEY_ERRORUNDEFINED, "<ERROR_UNDEFINED>");
        CASE_SCANCODE(KEY_KEYPAD_1_END, "1");
        CASE_SCANCODE(KEY_KEYPAD_2_DOWN_ARROW, "2");
        CASE_SCANCODE(KEY_KEYPAD_3_PAGEDN, "3");
        CASE_SCANCODE(KEY_KEYPAD_4_LEFT_ARROW, "4");
        CASE_SCANCODE(KEY_KEYPAD_5, "5");
        CASE_SCANCODE(KEY_KEYPAD_6_RIGHT_ARROW, "6");
        CASE_SCANCODE(KEY_KEYPAD_7_HOME, "7");
        CASE_SCANCODE(KEY_KEYPAD_8_UP_ARROW, "8");
        CASE_SCANCODE(KEY_KEYPAD_9_PAGEUP, "9");
        CASE_SCANCODE(KEY_KEYPAD_0_INSERT, "0");
        CASE_SCANCODE(KEY_ESCAPE, "ESC");
        CASE_SCANCODE(KEY_F1, "F1");
        CASE_SCANCODE(KEY_F2, "F2");
        CASE_SCANCODE(KEY_F3, "F3");
        CASE_SCANCODE(KEY_F4, "F4");
        CASE_SCANCODE(KEY_F5, "F5");
        CASE_SCANCODE(KEY_F6, "F6");
        CASE_SCANCODE(KEY_F7, "F7");
        CASE_SCANCODE(KEY_F8, "F8");
        CASE_SCANCODE(KEY_F9, "F9");
        CASE_SCANCODE(KEY_F10, "F10");
        CASE_SCANCODE(KEY_F11, "F11");
        CASE_SCANCODE(KEY_F12, "F12");
        CASE_SCANCODE(KEY_BACKSPACE, "BACKSPACE");
        CASE_SCANCODE(KEY_ENTER, "ENTER");
        CASE_SCANCODE(KEY_SPACEBAR, "SPACE");
        CASE_SCANCODE(KEY_KEYPAD_ENTER, "ENTER(Keypad)");
        CASE_SCANCODE(KEY_TAB, "TAB");
        CASE_SCANCODE(KEY_CAPS_LOCK, "CAPSLOCK");
        CASE_SCANCODE(KEY_DOWNARROW, "ARROW_DOWN");
        CASE_SCANCODE(KEY_UPARROW, "ARROW_UP");
        CASE_SCANCODE(KEY_RIGHTARROW, "ARROW_LEFT");
        CASE_SCANCODE(KEY_LEFTARROW, "ARROW_RIGHT");
        CASE_SCANCODE(KEY_PRINTSCREEN, "PRINTSCREEN");
        CASE_SCANCODE(KEY_SCROLL_LOCK, "SCROLLLOCK");
        CASE_SCANCODE(KEY_PAUSE, "PAUSE");
        CASE_SCANCODE(KEY_INSERT, "INS"); //"einfg"
        CASE_SCANCODE(KEY_DELETE, "DEL"); //"entf"
        CASE_SCANCODE(KEY_HOME, "HOME"); //"pos1"
        CASE_SCANCODE(KEY_END1, "END"); //"ende"
        CASE_SCANCODE(KEY_PAGEUP, "PAGEUP"); //"Bild-hoch"
        CASE_SCANCODE(KEY_PAGEDOWN, "PAGEDOWN"); //"Bild-runter"
        CASE_SCANCODE(KEY_KEYPAD_SLASH, "÷");
        CASE_SCANCODE(KEY_KEYPAD_ASTERIKS, "×");
        CASE_SCANCODE(KEY_KEYPAD_MINUS, "-");
        CASE_SCANCODE(KEY_KEYPAD_PLUS, "+");
        CASE_SCANCODE(KEY_KEYPAD_COMMA, ",");
        CASE_SCANCODE(KEY_KEYPAD_NUM_LOCK_AND_CLEAR, "NUMPAD");
        CASE_SCANCODE(KEY_APPLICATION, "RIGHTMENU");
    }

    //decode those that are sensitive to having left-shift or right-alt pressed
    if(is_shift_pressed) {
        //uppercase
        switch(scancode) {
            CASE_SCANCODE(KEY_GRAVE_ACCENT_AND_TILDE, "°");
            CASE_SCANCODE(KEY_1_EXCLAMATION_MARK, "!");
            CASE_SCANCODE(KEY_2_AT, "\"");
            CASE_SCANCODE(KEY_3_NUMBER_SIGN, "§");
            CASE_SCANCODE(KEY_4_DOLLAR, "$");
            CASE_SCANCODE(KEY_5_PERCENT, "%%");
            CASE_SCANCODE(KEY_6_CARET, "&");
            CASE_SCANCODE(KEY_7_AMPERSAND, "/");
            CASE_SCANCODE(KEY_8_ASTERISK, "(");
            CASE_SCANCODE(KEY_9_OPARENTHESIS, ")");
            CASE_SCANCODE(KEY_0_CPARENTHESIS, "=");
            CASE_SCANCODE(KEY_MINUS_UNDERSCORE, "?");
            CASE_SCANCODE(KEY_EQUAL_PLUS, "`");
            CASE_SCANCODE(KEY_Q, "Q");
            CASE_SCANCODE(KEY_W, "W");
            CASE_SCANCODE(KEY_E, "E");
            CASE_SCANCODE(KEY_R, "R");
            CASE_SCANCODE(KEY_T, "T");
            CASE_SCANCODE(KEY_Y, "Z");
            CASE_SCANCODE(KEY_U, "U");
            CASE_SCANCODE(KEY_I, "I");
            CASE_SCANCODE(KEY_O, "O");
            CASE_SCANCODE(KEY_P, "P");
            CASE_SCANCODE(KEY_OBRACKET_AND_OBRACE, "Ü");
            CASE_SCANCODE(KEY_CBRACKET_AND_CBRACE, "*");
            CASE_SCANCODE(KEY_A, "A");
            CASE_SCANCODE(KEY_S, "S");
            CASE_SCANCODE(KEY_D, "D");
            CASE_SCANCODE(KEY_F, "F");
            CASE_SCANCODE(KEY_G, "G");
            CASE_SCANCODE(KEY_H, "H");
            CASE_SCANCODE(KEY_J, "J");
            CASE_SCANCODE(KEY_K, "K");
            CASE_SCANCODE(KEY_L, "L");
            CASE_SCANCODE(KEY_SEMICOLON_COLON, "Ö");
            CASE_SCANCODE(KEY_SINGLE_AND_DOUBLE_QUOTE, "Ä");
            CASE_SCANCODE(KEY_NONUS_NUMBER_SIGN_TILDE, "'");
            CASE_SCANCODE(KEY_NONUS_BACK_SLASH_VERTICAL_BAR, ">");
            CASE_SCANCODE(KEY_Z, "Y");
            CASE_SCANCODE(KEY_X, "X");
            CASE_SCANCODE(KEY_C, "C");
            CASE_SCANCODE(KEY_V, "V");
            CASE_SCANCODE(KEY_B, "B");
            CASE_SCANCODE(KEY_N, "N");
            CASE_SCANCODE(KEY_M, "M");
            CASE_SCANCODE(KEY_COMMA_AND_LESS, ";");
            CASE_SCANCODE(KEY_DOT_GREATER, ":");
            CASE_SCANCODE(KEY_SLASH_QUESTION, "_");
            default: *err = true; return "";
        }
    } else if(is_alt_gr_pressed) {
        //only specific symbols are mapable at all
        switch(scancode) {
            CASE_SCANCODE(KEY_2_AT, "²");
            CASE_SCANCODE(KEY_3_NUMBER_SIGN, "³");
            CASE_SCANCODE(KEY_7_AMPERSAND, "{");
            CASE_SCANCODE(KEY_8_ASTERISK, "[");
            CASE_SCANCODE(KEY_9_OPARENTHESIS, "]");
            CASE_SCANCODE(KEY_0_CPARENTHESIS, "}");
            CASE_SCANCODE(KEY_MINUS_UNDERSCORE, "\\");
            CASE_SCANCODE(KEY_Q, "@");
            CASE_SCANCODE(KEY_E, "€");
            CASE_SCANCODE(KEY_CBRACKET_AND_CBRACE, "~");
            CASE_SCANCODE(KEY_NONUS_BACK_SLASH_VERTICAL_BAR, "|");
            CASE_SCANCODE(KEY_M, "µ");
            default: *err = true; return "";
        }
    } else {
        //'normal' lowercase
        switch(scancode) {
            CASE_SCANCODE(KEY_GRAVE_ACCENT_AND_TILDE, "^");
            CASE_SCANCODE(KEY_1_EXCLAMATION_MARK, "1");
            CASE_SCANCODE(KEY_2_AT, "2");
            CASE_SCANCODE(KEY_3_NUMBER_SIGN, "3");
            CASE_SCANCODE(KEY_4_DOLLAR, "4");
            CASE_SCANCODE(KEY_5_PERCENT, "5");
            CASE_SCANCODE(KEY_6_CARET, "6");
            CASE_SCANCODE(KEY_7_AMPERSAND, "7");
            CASE_SCANCODE(KEY_8_ASTERISK, "8");
            CASE_SCANCODE(KEY_9_OPARENTHESIS, "9");
            CASE_SCANCODE(KEY_0_CPARENTHESIS, "0");
            CASE_SCANCODE(KEY_MINUS_UNDERSCORE, "ß");
            CASE_SCANCODE(KEY_EQUAL_PLUS, "´");
            CASE_SCANCODE(KEY_Q, "q");
            CASE_SCANCODE(KEY_W, "w");
            CASE_SCANCODE(KEY_E, "e");
            CASE_SCANCODE(KEY_R, "r");
            CASE_SCANCODE(KEY_T, "t");
            CASE_SCANCODE(KEY_Y, "z");
            CASE_SCANCODE(KEY_U, "u");
            CASE_SCANCODE(KEY_I, "i");
            CASE_SCANCODE(KEY_O, "o");
            CASE_SCANCODE(KEY_P, "p");
            CASE_SCANCODE(KEY_OBRACKET_AND_OBRACE, "ü");
            CASE_SCANCODE(KEY_CBRACKET_AND_CBRACE, "+");
            CASE_SCANCODE(KEY_A, "a");
            CASE_SCANCODE(KEY_S, "s");
            CASE_SCANCODE(KEY_D, "d");
            CASE_SCANCODE(KEY_F, "f");
            CASE_SCANCODE(KEY_G, "g");
            CASE_SCANCODE(KEY_H, "h");
            CASE_SCANCODE(KEY_J, "j");
            CASE_SCANCODE(KEY_K, "k");
            CASE_SCANCODE(KEY_L, "l");
            CASE_SCANCODE(KEY_SEMICOLON_COLON, "ö");
            CASE_SCANCODE(KEY_SINGLE_AND_DOUBLE_QUOTE, "ä");
            CASE_SCANCODE(KEY_NONUS_NUMBER_SIGN_TILDE, "#");
            CASE_SCANCODE(KEY_NONUS_BACK_SLASH_VERTICAL_BAR, "<");
            CASE_SCANCODE(KEY_Z, "y");
            CASE_SCANCODE(KEY_X, "x");
            CASE_SCANCODE(KEY_C, "c");
            CASE_SCANCODE(KEY_V, "v");
            CASE_SCANCODE(KEY_B, "b");
            CASE_SCANCODE(KEY_N, "n");
            CASE_SCANCODE(KEY_M, "m");
            CASE_SCANCODE(KEY_COMMA_AND_LESS, ",");
            CASE_SCANCODE(KEY_DOT_GREATER, ".");
            CASE_SCANCODE(KEY_SLASH_QUESTION, "-");
            default: *err = true; return "";
        }
        return "";
    }
}