/*
 * screens.h
 *
 * Объявляет функции экранов: главное окно, область примера и просмотр результата.
 */

#ifndef ARITHMET_SCREENS_H
#define ARITHMET_SCREENS_H

/* Рисует стартовый экран с настройками и текущими значениями app_settings. */
void create_main_screen(void);

/* Обновляет только колонку "ДА/НЕТ" для выбранных арифметических операций. */
void refresh_operation_status(void);

/* Очищает область примера, проверки и крупных баннеров между заданиями. */
void clear_exercise_area(void);

/* Показывает верный ответ и ждет Tab для продолжения или F10 для завершения сеанса. */
int wait_after_answer(int result, int right);

/* Открывает экран просмотра последнего результата и обрабатывает прокрутку. */
void show_session_results(void);

/* Рисует статичную часть экрана результата. */
void create_results_screen(void);

/* Перерисовывает прокручиваемый список решенных примеров. */
void refresh_results_screen(void);

#endif
