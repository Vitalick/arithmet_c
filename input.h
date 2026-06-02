/*
 * input.h
 *
 * Объявляет ввод полей и проверку горячих клавиш для русского и латинского вариантов.
 */

#ifndef ARITHMET_INPUT_H
#define ARITHMET_INPUT_H

/* Проверяет горячую клавишу в русской UTF-8 раскладке и в латинском fallback-варианте. */
int key_matches_utf8_or_latin(const char key[2], const char *lower, const char *upper,
                              char latin_lower, char latin_upper);

/* Редактирует поле на экране: имя, числовую настройку или ответ на пример. */
void edit_input_field(int sx, int sy, int n, int nn);

#endif
