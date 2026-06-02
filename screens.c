/*
 * screens.c
 *
 * Отвечает за основной экран настроек и экран просмотра результатов последнего сеанса.
 */

#include <conio.h>
#include <stdio.h>
#include <string.h>

#include "asm.h"
#include "app_state.h"
#include "keys.h"
#include "mouse.h"
#include "screens.h"
#include "ui_helpers.h"

/* create_main_screen рисует статичную разметку стартового экрана и текущие настройки. */
void create_main_screen(void) {
    int ii;
    char tmp[10];
    int title_x;

    TextColor(YELLOW);
    TextBackGround(BLUE);
    ClrScr();
    TextColor(GREEN);
    title_x = centered_text_x("У С Т Н Ы Й    С Ч Е Т");
    CprintXY(title_x, 1, "У С Т Н Ы Й    С Ч Е Т");
    print_centered_text(2, "==== версия " ARITHMET_VERSION_STRING " ====");
    CprintXY(title_x + 1, 3, "ВАН");
    CprintXY(title_x + 17, 3, "1999");
    TextColor(LIGHTCYAN);
    CprintXY(3, 4, "Действие:");
    CprintXY(3, 5, "~~~~~~~~~");
    for (ii = 0; ii < 5; ii++) {
        TextColor(YELLOW);
        GetCharXY(7, 6 + ii, operation_symbols[ii]);
        TextColor(LIGHTCYAN);
        CprintXY(9, 6 + ii, operation_names[ii]);
    }
    refresh_operation_status();
    TextColor(LIGHTCYAN);
    TextBackGround(BLUE);
    CprintXY(29, 6, "Имя:");
    CprintXY(29, 7, "Величина результата:  от        до");
    CprintXY(29, 8, "Количество примеров:");
    CprintXY(60, 8, "Сложность:");
    CprintXY(36, 10, "Оценка:");
    CprintXY(36, 11, "~~~~~~~");
    CprintXY(10, 12, "Пример:");
    CprintXY(10, 13, "~~~~~~~");
    CprintXY(58, 12, "Верный ответ:");
    CprintXY(58, 13, "~~~~~~~~~~~~~");
    CprintXY(10, 14, "Проверка:   а)");
    CprintXY(10, 15, "~~~~~~~~~   б)");
    CprintXY(58, 14, "Верных ответов:");
    CprintXY(58, 15, "~~~~~~~~~~~~~~~");
    TextColor(YELLOW);
    CprintXY(29, 6, "И");
    CprintXY(51, 7, "о");
    CprintXY(61, 7, "д");
    CprintXY(29, 8, "К");
    CprintXY(60, 8, "С");
    TextColor(RED);
    CprintXY(3, 25, footer_help[0]);
    TextColor(LIGHTMAGENTA);
    CprintXY(34, 6, player_name);
    sprintf(tmp, "%-5i", app_settings[5]);
    CprintXY(54, 7, tmp);
    sprintf(tmp, "%-5i", app_settings[6]);
    CprintXY(64, 7, tmp);
    sprintf(tmp, "%-4i", app_settings[7]);
    CprintXY(51, 8, tmp);
    sprintf(tmp, "%-2i", app_settings[9]);
    CprintXY(72, 8, tmp);
}

/* refresh_operation_status обновляет индикаторы включенных арифметических операций. */
void refresh_operation_status(void) {
    int ii;

    TextBackGround(BLUE);
    TextColor(LIGHTMAGENTA);
    for (ii = 0; ii < 5; ii++)
        CprintXY(2, 6 + ii, yes_no_labels[app_settings[ii]]);
}

/* clear_exercise_area очищает зоны примера, проверки и баннеров между заданиями. */
void clear_exercise_area(void) {
    int ll;
    char tmps[30];

    for (ll = 0; ll < 29; ll++)
        tmps[ll] = ' ';
    tmps[29] = 0x0;
    TextColor(LIGHTCYAN);
    TextBackGround(BLUE);
    CprintXY(27, 12, tmps);
    CprintXY(27, 14, tmps);
    CprintXY(27, 15, tmps);
    for (ll = 0; ll < 8; ll++) {
        CprintXY(1, 17 + ll, tmps);
        CprintXY(21, 17 + ll, tmps);
        CprintXY(41, 17 + ll, tmps);
        CprintXY(52, 17 + ll, tmps);
    }
}

/* wait_after_answer показывает верный ответ и ждет Tab/F10 перед продолжением. */
int wait_after_answer(int result, int right) {
    char tmp[10];
    int tmpOR;

    CprintXY(23, 23, continue_prompt);
    TextColor(LIGHTMAGENTA);
    sprintf(tmp, "%-3i", result);
    CprintXY(73, 12, tmp);
    sprintf(tmp, "%-3i", right);
    CprintXY(74, 14, tmp);
    tmpOR = 0;
    while ((tmpOR != _Tab_) && (tmpOR != _F10_)) {
        idle_poll_counter++;
        if ((mouse_driver_status == -1) && (idle_poll_counter > 5000)) {
            idle_poll_counter = 0;
            mouse_x += mouse_dx;
            if (mouse_x >= mouse_max_x)
                mouse_dx = -1;
            if (mouse_x <= 0)
                mouse_dx = 1;
            mouse_y += mouse_dy;
            if (mouse_y >= mouse_max_y)
                mouse_dy = -1;
            if (mouse_y <= 0)
                mouse_dy = 1;
            SetPos(mouse_x, mouse_y);
        }
        tmpOR = Getc(&tmp[0], &tmp[1]);
    }
    return (tmpOR);
}

/* show_session_results открывает вторую видеостраницу и обрабатывает клавиши прокрутки протокола. */
void show_session_results(void) {
    int kk;
    char ks[2];

    Npage(2, 1);
    active_screen = 2;
    results_scroll_offset = 0;
    create_results_screen();
    refresh_results_screen();
    kk = 0;
    while (kk != _Esc_) {
        idle_poll_counter++;
        if ((mouse_driver_status == -1) && (idle_poll_counter > 5000)) {
            idle_poll_counter = 0;
            mouse_x += mouse_dx;
            if (mouse_x >= mouse_max_x)
                mouse_dx = -1;
            if (mouse_x <= 0)
                mouse_dx = 1;
            mouse_y += mouse_dy;
            if (mouse_y >= mouse_max_y)
                mouse_dy = -1;
            if (mouse_y <= 0)
                mouse_dy = 1;
            SetPos(mouse_x, mouse_y);
        }
        kk = Getc(&ks[0], &ks[1]);
        if (kk != 0) {
            switch (kk) {
                case _Up_:
                    if (results_scroll_offset != 0) {
                        results_scroll_offset--;
                        refresh_results_screen();
                    }
                    break;
                case _Down_:
                    if ((results_scroll_offset + 11) < session_result.answered_count) {
                        results_scroll_offset++;
                        refresh_results_screen();
                    }
                    break;
                case _PageUp_:
                    if (results_scroll_offset != 0) {
                        results_scroll_offset -= 11;
                        if (results_scroll_offset < 0)
                            results_scroll_offset = 0;
                        refresh_results_screen();
                    }
                    break;
                case _PageDown_:
                    if ((results_scroll_offset + 11) < session_result.answered_count) {
                        results_scroll_offset += 10;
                        if ((results_scroll_offset + 11) > session_result.answered_count)
                            results_scroll_offset = session_result.answered_count - 12;
                        refresh_results_screen();
                    }
                    break;
                case _Home_:
                    if (results_scroll_offset != 0) {
                        results_scroll_offset = 0;
                        refresh_results_screen();
                    }
                    break;
                case _End_:
                    if ((results_scroll_offset + 11) < session_result.answered_count) {
                        results_scroll_offset = session_result.answered_count - 11;
                        refresh_results_screen();
                    }
                    break;
            }
        }
    }
    Npage(1, 1);
    active_screen = 1;
}

/* create_results_screen рисует заголовок и сводную часть экрана результата. */
void create_results_screen(void) {
    char tmp[100];
    int i, j, k, l, m;

    TextColor(YELLOW);
    TextBackGround(BLUE);
    ClrScr();
    TextColor(GREEN);
    print_centered_text(1, "Р Е З У Л Ь Т А Т Ы");
    print_centered_text(2, "===================");
    TextColor(LIGHTCYAN);
    CprintXY(1, 3, "Имя:");
    CprintXY(1, 4, "Действия:");
    CprintXY(1, 5, "Количество:");
    CprintXY(18, 5, "пределы: от");
    CprintXY(36, 5, "до");
    CprintXY(46, 5, "сложность:");
    CprintXY(1, 6, "Верных ответов:");
    CprintXY(22, 6, "оценка:");
    CprintXY(1, 7, "Количество действий:");
    CprintXY(1, 12, "Предложенные действия:");
    TextColor(LIGHTMAGENTA);
    CprintXY(6, 3, session_result.name);
    l = 11;
    j = 0;
    for (i = 0; i < 5; i++) {
        if (session_result.enabled_operations[i] == 1) {
            if (j != 0) {
                CprintXY(l - 1, 4, ",");
                l++;
            }
            CprintXY(l, 4, operation_names[i]);
            l = l + strlen(operation_names[i]) + 1;
            j++;
        }
    }
    sprintf(tmp, "%i", session_result.planned_count);
    CprintXY(13, 5, tmp);
    sprintf(tmp, "%i", session_result.result_limits[0]);
    CprintXY(30, 5, tmp);
    sprintf(tmp, "%i", session_result.result_limits[1]);
    CprintXY(39, 5, tmp);
    sprintf(tmp, "%i", session_result.time_limit_seconds);
    CprintXY(57, 5, tmp);
    sprintf(tmp, "%i", session_result.correct_count);
    CprintXY(17, 6, tmp);
    sprintf(tmp, "%i", session_result.grade);
    CprintXY(30, 6, tmp);
    l = 7;
    for (i = 0; i < 5; i++) {
        if (session_result.enabled_operations[i] == 1) {
            TextColor(LIGHTCYAN);
            sprintf(tmp, "%19s -", operation_names[i]);
            CprintXY(22, l, tmp);
            CprintXY(49, l, "верных ответов -");
            TextColor(LIGHTMAGENTA);
            sprintf(tmp, "%i", session_result.operation_counts[i]);
            CprintXY(45, l, tmp);
            sprintf(tmp, "%i", session_result.correct_operation_counts[i]);
            CprintXY(66, l, tmp);
            l++;
        }
    }
    i = 300;
    j = 0;
    k = 0;
    l = 0;
    m = 0;
    while ((session_result.answers[l].operation_index != -10) || (session_result.answers[l].is_correct != -10)) {
        if (session_result.answers[l].values[3] >= 0) {
            if (session_result.answers[l].elapsed_seconds < i)
                i = session_result.answers[l].elapsed_seconds;
            if (session_result.answers[l].elapsed_seconds > j)
                j = session_result.answers[l].elapsed_seconds;
            k += session_result.answers[l].elapsed_seconds;
            m++;
        }
        l++;
    }
    if (m != 0) {
        k /= m;
        snprintf(tmp, sizeof(tmp), "Время:  лучшее - %i, худшее - %i, среднее - %i", i, j, k);
        m = 80 - strlen(tmp) - 5;
        CprintXY(m, 25, tmp);
    }
}

/* refresh_results_screen перерисовывает видимый фрагмент списка решенных примеров. */
void refresh_results_screen(void) {
    int i, l, m;
    char tmp[80];

    TextBackGround(BLUE);
    for (l = 0; l < 11; l++) {
        for (i = 0; i < 79; i++)
            tmp[i] = ' ';
        tmp[79] = 0x0;
        CprintXY(1, l + 13, tmp);
        if ((session_result.answers[l + results_scroll_offset].operation_index == -10) || (session_result.answers[l + results_scroll_offset].is_correct == -10))
            break;
        TextColor(LIGHTMAGENTA);
        sprintf(tmp, "%3i.", l + results_scroll_offset + 1);
        CprintXY(1, l + 13, tmp);
        if (session_result.answers[l + results_scroll_offset].is_correct == 0)
            CprintXY(7, l + 13, "не");
        CprintXY(10, l + 13, "верно:");
        TextColor(LIGHTCYAN);
        sprintf(tmp, "%i%c%i=%i", session_result.answers[l + results_scroll_offset].values[0],
                operation_symbols[session_result.answers[l + results_scroll_offset].operation_index], session_result.answers[l + results_scroll_offset].values[1],
                session_result.answers[l + results_scroll_offset].values[2]);
        CprintXY(17, l + 13, tmp);
        switch (session_result.answers[l + results_scroll_offset].values[3]) {
            case -7:
                sprintf(tmp, "нажата клавиша <Esc>");
                break;
            case -5:
                sprintf(tmp, "вышло время");
                break;
            case -3:
                sprintf(tmp, "нажата клавиша <F10>");
                break;
            default:
                sprintf(tmp, "введено %i", session_result.answers[l + results_scroll_offset].values[3]);
                break;
        }
        TextColor(LIGHTMAGENTA);
        CprintXY(30, l + 13, tmp);
        m = session_result.answers[l + results_scroll_offset].elapsed_seconds - (session_result.answers[l + results_scroll_offset].elapsed_seconds / 10) * 10;
        if ((session_result.answers[l + results_scroll_offset].elapsed_seconds > 10) && (session_result.answers[l + results_scroll_offset].elapsed_seconds < 20))
            m = 0;
        switch (m) {
            case 0:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
                sprintf(tmp, "  прошло %3i секунд\r\n", session_result.answers[l + results_scroll_offset].elapsed_seconds);
                break;
            case 1:
                sprintf(tmp, "  прошла %3i секунда\r\n", session_result.answers[l + results_scroll_offset].elapsed_seconds);
                break;
            case 2:
            case 3:
            case 4:
                sprintf(tmp, "  прошло %3i секунды\r\n", session_result.answers[l + results_scroll_offset].elapsed_seconds);
                break;
        }
        CprintXY(52, l + 13, tmp);
    }
    TextColor(YELLOW);
    if (results_scroll_offset > 0)
        CprintXY(50, 12, "еще");
    else
        CprintXY(50, 12, "   ");
    if ((results_scroll_offset + 11) < session_result.answered_count)
        CprintXY(50, 24, "еще");
    else
        CprintXY(50, 24, "   ");
}
