/*
 * conio_terminal.c
 *
 * Настраивает реальный терминал и связывает логический экран 80x25 с ANSI/WinAPI выводом.
 */

#include "conio.h"
#include "asm.h"
#include "terminal_platform.h"
#include "conio_internal.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

static int cursor_visible = 1;
static int terminal_screen_initialized = 0;
static volatile sig_atomic_t terminal_size_changed = 1;
static void (*terminal_resize_handler)(void) = NULL;

/* terminal_cell_visible проверяет, попадает ли текущая логическая ячейка в фактическое окно терминала. */
int terminal_cell_visible(void) {
    unsigned int x = ScreenOffsetX + CX + 1;
    unsigned int y = ScreenOffsetY + CY + 1;

    return (x >= 1) && (x <= (unsigned int) TerminalCols) &&
           (y >= 1) && (y <= (unsigned int) TerminalRows);
}

/* update_terminal_size обновляет размер терминала и рассчитывает смещение для центрирования экрана 80x25. */
static void update_terminal_size(void) {
    UpdateTerminalSize();
    ScreenOffsetX = (TerminalCols > LOGICAL_SCREEN_WIDTH) ? ((TerminalCols - LOGICAL_SCREEN_WIDTH) / 2) : 0;
    ScreenOffsetY = (TerminalRows > LOGICAL_SCREEN_HEIGHT) ? ((TerminalRows - LOGICAL_SCREEN_HEIGHT) / 2) : 0;
}

void RestoreTerminalScreen(void) {
    printf("\033[0m\033[?25h\033[?1049l");
    fflush(stdout);
    RestoreTerminalConsole();
}

void RestoreTerminalScreenFromSignal(void) {
    static const char sequence[] = "\033[0m\033[?25h\033[?1049l";

    WriteTerminalSignalSequence(sequence, sizeof(sequence) - 1);
}

void init_terminal_screen(void) {
    if (terminal_screen_initialized) {
        return;
    }

    terminal_screen_initialized = 1;
    ConfigureTerminalConsole();
    InitTerminalScreenSignals(&terminal_size_changed);
    update_terminal_size();
    terminal_size_changed = 0;
    printf("\033[?1049h");
    printf("\033[2J");
    printf("\033[H");
    atexit(RestoreTerminalScreen);
    fflush(stdout);
}

/* ansi_color переводит DOS-цвет 0-15 в ANSI-код цвета переднего плана. */
static int ansi_color(int color) {
    static const int colors[16] = {
        30, 34, 32, 36, 31, 35, 33, 37,
        90, 94, 92, 96, 91, 95, 93, 97
    };

    return colors[color & 0x0F];
}

int RefreshTerminalSize(void) {
    init_terminal_screen();
    if (PollTerminalResize()) {
        terminal_size_changed = 1;
    }
    if (!terminal_size_changed) {
        return 0;
    }

    terminal_size_changed = 0;
    update_terminal_size();
    printf("\033[0m");
    printf("\033[2J");
    printf("\033[H");
    if (terminal_resize_handler != NULL) {
        terminal_resize_handler();
    } else {
        apply_terminal_attribute();
        move_terminal_cursor();
    }
    fflush(stdout);
    return 1;
}

void SetTerminalResizeHandler(void (*handler)(void)) {
    terminal_resize_handler = handler;
}

void apply_terminal_attribute(void) {
    int fg = ATTRIBUTE & 0x0F;

    init_terminal_screen();
    printf("\033[%dm", ansi_color(fg));
}

void move_terminal_cursor(void) {
    unsigned int x;
    unsigned int y;

    init_terminal_screen();
    x = ScreenOffsetX + CX + 1;
    y = ScreenOffsetY + CY + 1;

    if (x < 1) {
        x = 1;
    }
    if (y < 1) {
        y = 1;
    }
    if (x > (unsigned int) TerminalCols) {
        x = TerminalCols;
    }
    if (y > (unsigned int) TerminalRows) {
        y = TerminalRows;
    }

    printf("\033[%u;%uH", y, x);
}

void flush_terminal_output(void) {
    fflush(stdout);
}

void _setcursortype(int type) {
    init_terminal_screen();
    cursor_visible = type != _NOCURSOR;
    printf(cursor_visible ? "\033[?25h" : "\033[?25l");
    flush_terminal_output();
}
