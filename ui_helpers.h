/*
 * ui_helpers.h
 *
 * Объявляет общие операции отрисовки и сигнала ошибки.
 */

#ifndef ARITHMET_UI_HELPERS_H
#define ARITHMET_UI_HELPERS_H

/* Перерисовывает активный экран после изменения размера терминала. */
void redraw_active_screen_for_resize(void);

/* Возвращает X-координату для центрирования UTF-8 текста в логическом экране 80x25. */
int centered_text_x(const char *text);

/* Печатает строку по центру указанной строки экрана. */
void print_centered_text(int y, const char *text);

/* Воспроизводит короткий сигнал ошибки через слой совместимости DOS. */
void play_error_sound(void);

#endif
