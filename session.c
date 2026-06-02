/*
 * session.c
 *
 * Проводит тренировочный сеанс: выбирает операции, генерирует примеры, проверяет ответы и считает оценку.
 */

#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "asm.h"
#include "app_state.h"
#include "input.h"
#include "keys.h"
#include "mouse.h"
#include "protocol.h"
#include "screens.h"

/* run_training_session сохраняет старый крупный алгоритм сеанса без дробления на новые подфункции. */
void run_training_session(void) {
    int ii, ll, kk, mm, cc, number, OR;
    char tmp[16];
    char tmps[80]; // возможно при делении с остатком нужен будет больший размер массива
    int arg1, arg2, result, right, ball, n;
    double percent;

    TextColor(RED);
    CprintXY(3, 25, footer_help[2]);
    n = 0;
    right = 0;
    for (ii = 0; ii < 5; ii++)
        if (app_settings[ii] == 1)
            n++;
    number = 0;
    while (app_settings[number] == 0)
        number++;
    for (ii = 0; ii < app_settings[7]; ii++) {
        memset(tmps, 0, sizeof(tmps));
        clear_exercise_area();
        TextColor(LIGHTMAGENTA);
        snprintf(tmp, sizeof(tmp), "%-3i", right);
        CprintXY(74, 14, tmp);
        if (n != 1) {
            number = dos_random(n);
            ll = -1;
            kk = -1;
            while (ll != number) {
                kk++;
                if (app_settings[kk] == 1)
                    ll++;
            }
            number = kk;
        }
        switch (number) {
            case 0: // operation +
            newadd:
                result = dos_random(app_settings[6] - app_settings[5]) + app_settings[5];
                arg1 = dos_random(result);
                arg2 = result - arg1;
                if ((arg1 == 0) || (arg2 == 0) || (result == 0))
                    goto newadd;
                break;
            case 1: // operation -
            newsub:
                arg1 = dos_random(app_settings[6] - app_settings[5]) + app_settings[5];
                arg2 = dos_random(arg1);
                result = arg1 - arg2;
                if ((arg1 == 0) || (arg2 == 0) || (result == 0))
                    goto newsub;
                break;
            case 2: // operation *
            newresult:
                result = dos_random(app_settings[6] - app_settings[5]) + app_settings[5];
                arg1 = dos_random(result);
                if (arg1 == 0)
                    arg1 = 1;
                arg2 = result / arg1;
                cc = arg1;
                ll = 1;
                kk = arg1 * arg2;
                mm = 1;
                while (kk != result) {
                    if (((arg1 + ll) > result) && ((arg1 - ll) < 0))
                        goto newresult;
                    cc = arg1 + ll * mm;
                    if (cc == 0)
                        cc = 1;
                    arg2 = result / cc;
                    kk = cc * arg2;
                    if (mm == 1)
                        mm = -1;
                    else {
                        mm = 1;
                        ll++;
                    }
                }
                arg1 = cc;
                if ((arg1 == 1) || (arg2 == 1) || (result == 1))
                    goto newresult;
                break;
            case 3: // operation /
            case 4: // operation :
            newarg1:
                arg1 = dos_random(app_settings[6] - app_settings[5]) + app_settings[5];
                arg2 = dos_random(arg1 / 2) + 1;
                if (arg2 == 0)
                    arg2 = 1;
                result = arg1 / arg2;
                cc = arg2;
                ll = 1;
                kk = result * arg2;
                mm = 1;
                while (kk != arg1) {
                    if (((arg2 + ll) > arg1) && ((arg2 - ll) < 0))
                        goto newarg1;
                    cc = arg2 + ll * mm;
                    if (cc == 0)
                        cc = 1;
                    result = arg1 / cc;
                    kk = result * cc;
                    if (mm == 1)
                        mm = -1;
                    else {
                        mm = 1;
                        ll++;
                    }
                }
                arg2 = cc;
                if ((arg1 == 1) || (arg2 == 1) || (result == 1))
                    goto newarg1;
                if (number == 4) {
                    kk = dos_random(arg2 - 1) + 1;
                    arg1 += kk;
                }
                break;
        }
        snprintf(tmps, sizeof(tmps), "%i %c %i = ", arg1, operation_symbols[number], arg2);
        ll = strlen(tmps);
        if (number == 4)
            TextColor(WHITE);
        else
            TextColor(LIGHTMAGENTA);
        CprintXY(27, 12, tmps);
        TextColor(LIGHTMAGENTA);
        sprintf(tmp, "%3i", ii + 1);
        CprintXY(17, 12, tmp);
        app_settings[8] = -1;
        edit_input_field(27 + ll, 12, 8, 5);
        TextColor(RED);
        CprintXY(3, 25, footer_help[2]);
        session_result.answered_count++;
        session_result.operation_counts[number]++;
        session_result.answers[ii].values[0] = arg1;
        session_result.answers[ii].values[1] = arg2;
        session_result.answers[ii].values[2] = result;
        session_result.answers[ii].values[3] = app_settings[8];
        session_result.answers[ii].operation_index = number;
        session_result.answers[ii].is_correct = 0;
        session_result.answers[ii].elapsed_seconds = (int) ((current_time - answer_started_at) / CLK_TCK);
        if (app_settings[8] == result) {
            right++;
            session_result.correct_operation_counts[number]++;
            session_result.answers[ii].is_correct = 1;
        }
        percent = (double) right * 100.0 / (double) (ii + 1);
        if (percent >= 98.0)
            ball = 5;
        else if (percent >= 75.0)
            ball = 4;
        else if (percent >= 50.0)
            ball = 3;
        else if (percent >= 20.0)
            ball = 2;
        else
            ball = 1;
        TextColor(LIGHTMAGENTA);
        sprintf(tmp, "%1i", ball);
        CprintXY(45, 10, tmp);
        if (app_settings[8] > 0) {
            TextColor(YELLOW);
            switch (number) {
                case 0: // operation +
                    snprintf(tmps, sizeof(tmps), "%i - %i = %i", app_settings[8], arg1, app_settings[8] - arg1);
                    CprintXY(27, 14, tmps);
                    snprintf(tmps, sizeof(tmps), "%i - %i = %i", app_settings[8], arg2, app_settings[8] - arg2);
                    CprintXY(27, 15, tmps);
                    break;
                case 1: // operation -
                    snprintf(tmps, sizeof(tmps), "%i - %i = %i", arg1, app_settings[8], arg1 - app_settings[8]);
                    CprintXY(27, 14, tmps);
                    snprintf(tmps, sizeof(tmps), "%i + %i = %i", arg2, app_settings[8], arg2 + app_settings[8]);
                    CprintXY(27, 15, tmps);
                    break;
                case 2: // operation *
                    ll = app_settings[8] / arg1;
                    kk = ll * arg1;
                    mm = app_settings[8] - kk;
                    if (mm == 0)
                        snprintf(tmps, sizeof(tmps), "%i / %i = %i", app_settings[8], arg1, ll);
                    else
                        snprintf(tmps, sizeof(tmps), "%i / %i = %i (остаток %i)", app_settings[8], arg1, ll, mm);
                    CprintXY(27, 14, tmps);
                    ll = app_settings[8] / arg2;
                    kk = ll * arg2;
                    mm = app_settings[8] - kk;
                    if (mm == 0)
                        snprintf(tmps, sizeof(tmps), "%i / %i = %i", app_settings[8], arg2, ll);
                    else
                        snprintf(tmps, sizeof(tmps), "%i / %i = %i (остаток %i)", app_settings[8], arg2, ll, mm);
                    CprintXY(27, 15, tmps);
                    break;
                case 3: // operation /
                    ll = arg1 / app_settings[8];
                    kk = ll * app_settings[8];
                    mm = arg1 - kk;
                    if (mm == 0)
                        snprintf(tmps, sizeof(tmps), "%i / %i = %i", arg1, app_settings[8], ll);
                    else
                        snprintf(tmps, sizeof(tmps), "%i / %i = %i (остаток %i)", arg1, app_settings[8], ll, mm);
                    CprintXY(27, 14, tmps);
                    snprintf(tmps, sizeof(tmps), "%i * %i = %i", arg2, app_settings[8], arg2 * app_settings[8]);
                    CprintXY(27, 15, tmps);
                    break;
                case 4: // operation :
                    kk = arg2 * app_settings[8];
                    mm = arg1 - kk;
                    if (mm == 0)
                        snprintf(tmps, sizeof(tmps), "%i / %i = %i", arg1, arg2, app_settings[8]);
                    else
                        snprintf(tmps, sizeof(tmps), "%i / %i = %i (остаток %i)", arg1, arg2, app_settings[8], mm);
                    CprintXY(27, 14, tmps);
                    if (mm > arg2)
                        snprintf(tmps, sizeof(tmps), "%i > %i", mm, arg2);
                    else
                        snprintf(tmps, sizeof(tmps), "%i < %i", mm, arg2);
                    CprintXY(27, 15, tmps);
                    break;
            }
            if (app_settings[8] == result) {
                TextColor(GREEN);
                for (kk = 0; kk < 6; kk++)
                    CprintXY(14, 17 + kk, banner_success[kk]);
            } else {
                TextColor(RED);
                for (kk = 0; kk < 5; kk++)
                    CprintXY(11, 17 + kk, banner_failure[kk]);
            }
            OR = wait_after_answer(result, right);
            if (OR == _F10_)
                goto end;
        } else {
            if (app_settings[8] == -3) {
                goto end;
            } else {
                if ((app_settings[8] == -5) || (app_settings[8] == -5)) {
                    for (kk = 0; kk < 5; kk++)
                        CprintXY(12, 17 + kk, banner_time_elapsed[kk]);
                    OR = wait_after_answer(result, right);
                    if (OR == _F10_)
                        goto end;
                }
            }
        }
    }
end:
    percent = (double) right * 100.0 / (double) app_settings[7];
    if (percent >= 98.0)
        ball = 5;
    else if (percent >= 75.0)
        ball = 4;
    else if (percent >= 50.0)
        ball = 3;
    else if (percent >= 20.0)
        ball = 2;
    else
        ball = 1;
    clear_exercise_area();
    for (kk = 0; kk < 6; kk++)
        CprintXY(10, 17 + kk, banner_final_grade[kk]);
    switch (ball) {
        case 1:
            for (kk = 0; kk < 5; kk++)
                CprintXY(66, 17 + kk, banner_grade_1[kk]);
            break;
        case 2:
            for (kk = 0; kk < 5; kk++)
                CprintXY(66, 17 + kk, banner_grade_2[kk]);
            break;
        case 3:
            for (kk = 0; kk < 5; kk++)
                CprintXY(66, 17 + kk, banner_grade_3[kk]);
            break;
        case 4:
            for (kk = 0; kk < 5; kk++)
                CprintXY(66, 17 + kk, banner_grade_4[kk]);
            break;
        case 5:
            for (kk = 0; kk < 5; kk++)
                CprintXY(66, 17 + kk, banner_grade_5[kk]);
            break;
    }
    session_result.grade = ball;
    session_result.correct_count = right;
    CprintXY(23, 23, continue_prompt);
    ii = Getc(&tmp[0], &tmp[1]);
    while (ii != _Tab_) {
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
        ii = Getc(&tmp[0], &tmp[1]);
    }
    save_session_protocol();
    TextColor(RED);
    CprintXY(3, 25, footer_help[0]);
}

void reset_session_result(void) {
    int i, j;

    for (i = 0; i < 46; i++)
        session_result.name[i] = player_name[i];
    for (i = 0; i < 5; i++) {
        session_result.enabled_operations[i] = app_settings[i];
        session_result.operation_counts[i] = 0;
        session_result.correct_operation_counts[i] = 0;
    }
    session_result.planned_count = app_settings[7];
    session_result.answered_count = 0;
    session_result.result_limits[0] = app_settings[5];
    session_result.result_limits[1] = app_settings[6];
    session_result.time_limit_seconds = app_settings[9];
    session_result.grade = 0;
    session_result.correct_count = 0;
    for (i = 0; i < 200; i++) {
        for (j = 0; j < 4; j++)
            session_result.answers[i].values[j] = 0;
        session_result.answers[i].operation_index = -10;
        session_result.answers[i].is_correct = -10;
        session_result.answers[i].elapsed_seconds = 100;
    }
}
