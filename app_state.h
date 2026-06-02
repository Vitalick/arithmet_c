/*
 * app_state.h
 *
 * Объявляет общее состояние программы: настройки, текущий протокол сеанса и текстовые ресурсы интерфейса.
 */

#ifndef ARITHMET_APP_STATE_H
#define ARITHMET_APP_STATE_H

#include <stdio.h>
#include <time.h>

#define STRINGIFY_VALUE(value) #value
#define STRINGIFY(value) STRINGIFY_VALUE(value)

#if defined(ARITHMET_VERSION_MAJOR) && defined(ARITHMET_VERSION_MINOR) && defined(ARITHMET_VERSION_PATCH)
#define ARITHMET_VERSION_STRING STRINGIFY(ARITHMET_VERSION_MAJOR) "." STRINGIFY(ARITHMET_VERSION_MINOR) "." STRINGIFY(ARITHMET_VERSION_PATCH)
#else
#define ARITHMET_VERSION_STRING "dev"
#endif

/* ExerciseAnswer хранит один пример из завершенного или прерванного сеанса. */
struct ExerciseAnswer {
    /* values: 0 - первый аргумент, 1 - второй аргумент, 2 - верный результат, 3 - ответ ученика или код прерывания. */
    int values[5];
    /* operation_index указывает на операцию в operation_symbols и operation_names. */
    int operation_index;
    /* is_correct хранит 1 для верного ответа, 0 для ошибки и -10 для пустой записи. */
    int is_correct;
    int elapsed_seconds;
};

/* SessionResult собирает настройки и статистику последнего сеанса для просмотра и сохранения в протокол. */
struct SessionResult {
    char name[46];
    int enabled_operations[5];
    int planned_count;
    int answered_count;
    int result_limits[2];
    int time_limit_seconds;
    int grade;
    int operation_counts[5];
    int correct_count;
    int correct_operation_counts[5];
    struct ExerciseAnswer answers[200];
};

extern const char * const banner_success[];
extern const char * const banner_failure[];
extern const char * const banner_time_elapsed[];
extern const char * const banner_final_grade[];
extern const char * const banner_grade_1[];
extern const char * const banner_grade_2[];
extern const char * const banner_grade_3[];
extern const char * const banner_grade_4[];
extern const char * const banner_grade_5[];

extern int app_settings[10];
extern char player_name[46];
extern const char *default_player_name;
extern const char * const yes_no_labels[2];
extern char operation_symbols[];
extern const char * const operation_names[5];
extern FILE *settings_file;
extern char pressed_key_bytes[2];
extern int pressed_key_code;
extern const char *continue_prompt;
extern const char * const footer_help[4];
extern struct SessionResult session_result;

extern int results_scroll_offset;
extern int time_bar_position;
extern clock_t answer_started_at, current_time, answer_time_limit_ticks, cursor_updated_at, cursor_blink_interval;
extern int cursor_blink_speed;
extern int mouse_x, mouse_y, mouse_buttons, mouse_dx, mouse_dy, mouse_driver_status, mouse_key_status;
extern int mouse_cursor_mask_a, mouse_cursor_mask_b, mouse_max_x, mouse_max_y, idle_poll_counter;
extern char *mouse_cursor_mask_a_bytes, *mouse_cursor_mask_b_bytes;
extern int active_screen;

/*
 * app_settings повторяет старый массив настроек DOS-версии:
 * 0..4 - включенные операции, 5 - нижний предел результата,
 * 6 - верхний предел результата, 7 - количество примеров,
 * 8 - текущий ответ или служебный код, 9 - лимит времени в секундах.
 */

#endif
