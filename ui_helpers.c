/*
 * ui_helpers.c
 *
 * Содержит небольшие общие операции интерфейса, которые нужны нескольким экранам.
 */

#include <string.h>

#include <conio.h>
#include "asm.h"
#include "app_state.h"
#include "screens.h"
#include "ui_helpers.h"

void redraw_active_screen_for_resize(void) {
    if (active_screen == 2) {
        create_results_screen();
        refresh_results_screen();
    } else {
        create_main_screen();
    }
}

int centered_text_x(const char *text) {
    int i;
    int width = 0;

    for (i = 0; text[i] != 0x0; i++) {
        if (((unsigned char) text[i] & 0xC0) != 0x80) {
            width++;
        }
    }

    if (width >= 80) {
        return 1;
    }

    return (80 - width) / 2 + 1;
}

void print_centered_text(int y, const char *text) {
    CprintXY(centered_text_x(text), y, text);
}

void play_error_sound(void) {
    sound(777);
    delay(300);
    nosound();
}
