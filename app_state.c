/*
 * app_state.c
 *
 * Хранит глобальные данные, оставшиеся от DOS-версии: настройки, баннеры и буфер результата сеанса.
 */

#include "app_state.h"

const char * const banner_success[] =
{
    "█   █    ██      ███    ██     ████    ████   █  █    █",
    "██ ██   █  █    █  █   █  █    █  █    █      █  █    █",
    "█ █ █   █  █    █  █   █  █    █  █    ███    █  █    █",
    "█   █   █  █    █  █   █  █    █  █    █      █  █",
    "█   █    ██    ██  █    ██    ██████   ████   █████   █",
    "                              █    █              █"
};
const char * const banner_failure[] =
{
    "█  █   ████   ███    ████   ███    █  █    ██",
    "█  █   █      █  █   █      █  █   █  █   █  █",
    "████   ███    ███    ███    ███    ████   █  █",
    "█  █   █      █  █   █      █      █  █   █  █",
    "█  █   ████   ███    ████   █      █  █    ██    █   █   █"
};
const char * const banner_time_elapsed[] =
{
    "███  ███  ████ █   █  ███   ███  █    █ █ █ █   ███  ██",
    "█  █ █  █ █    ██ ██ █  █   █  █ █    █ █ █ █  █  █ █  █",
    "███  ███  ███  █ █ █  ███   ███  ███  █ █ █ █  █  █ █  █",
    "█  █ █    █    █   █   ██   █  █ █  █ █ █ █ █  █  █ █  █",
    "███  █    ████ █   █  █ █   ███  ███  █ █████ ██  █  ██"
};
const char * const banner_final_grade[] =
{
    "███   ██  █ █ █  ██     ██  █  █  ████ █  █ █  █  ██",
    "█  █ █  █ █ █ █ █  █   █  █ █  █  █    █  █ █ █  █  █",
    "███  ████ █ █ █ ████   █  █ █  █  ███  ████ ██   ████",
    "█  █ █  █ █ █ █ █  █   █  █ █  █  █    █  █ █ █  █  █",
    "███  █  █ █████ █  █    ██  █████ ████ █  █ █  █ █  █",
    "                                █"
};
const char * const banner_grade_1[] =
{
    "   █",
    "  ██",
    "   █",
    "   █",
    "   █"
};
const char * const banner_grade_2[] =
{
    " ██",
    "█  █",
    "  █",
    " █",
    "████"
};
const char * const banner_grade_3[] =
{
    " ██",
    "█  █",
    "  █",
    "█  █",
    " ██"
};
const char * const banner_grade_4[] =
{
    "█  █",
    "█  █",
    "████",
    "   █",
    "   █"
};
const char * const banner_grade_5[] =
{
    "████",
    "█",
    "████",
    "   █",
    "████"
};
int app_settings[10] = {1, 0, 0, 0, 1, 100, 10, 0};
char player_name[46];
const char *default_player_name = "неизвестно";
const char * const yes_no_labels[2] = {"НЕТ", "ДА "};
char operation_symbols[] = "+-*/:";
const char * const operation_names[5] = {"сложение", "вычитание", "умножение", "деление", "деление с остатком"};
FILE *settings_file;
char pressed_key_bytes[2];
int pressed_key_code;
const char *continue_prompt = "Для продолжения нажми клавишу <Tab>.";
const char * const footer_help[4] =
{
    " <+-*/:ИОДКС> - установки, <F1> - результат, <Esc> - выход, <Enter> - старт ",
    "      <->><<-><Home><End> - курсор, <Enter> - запомнить, <Esc> - выход      ",
    "<->><<-><Home><End> - курсор, <Enter> - ответ, <Esc> - сдаюсь, <F10> - выход",
    " <->><<-><Home><End> - курсор, <Enter> - ввод, <Esc> - сброс, <F10> - выход "
};

struct SessionResult session_result;

int results_scroll_offset;
int time_bar_position;
clock_t answer_started_at, current_time, answer_time_limit_ticks, cursor_updated_at, cursor_blink_interval;
int cursor_blink_speed = 10;
int mouse_x, mouse_y, mouse_buttons, mouse_dx, mouse_dy, mouse_driver_status, mouse_key_status;
int mouse_cursor_mask_a, mouse_cursor_mask_b, mouse_max_x, mouse_max_y, idle_poll_counter;
char *mouse_cursor_mask_a_bytes, *mouse_cursor_mask_b_bytes;
int active_screen = 1;
