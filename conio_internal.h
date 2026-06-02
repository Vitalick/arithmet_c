/*
 * conio_internal.h
 *
 * Внутренний интерфейс модулей conio. Не используется кодом приложения напрямую.
 */

#ifndef ARITHMET_CONIO_INTERNAL_H
#define ARITHMET_CONIO_INTERNAL_H

#define LOGICAL_SCREEN_WIDTH 80
#define LOGICAL_SCREEN_HEIGHT 25

extern char ScreenBuffer[4096 * 8];

int terminal_cell_visible(void);
void init_terminal_screen(void);
void apply_terminal_attribute(void);
void move_terminal_cursor(void);
void flush_terminal_output(void);

#endif
