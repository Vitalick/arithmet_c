/*
 * input.c
 *
 * Обрабатывает ввод полей настроек и ответа с учетом русских UTF-8 символов.
 */

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "asm.h"
#include "app_state.h"
#include "input.h"
#include "keys.h"
#include "mouse.h"
#include "ui_helpers.h"

int key_matches_utf8_or_latin(const char pressed_key_code[2], const char *lower, const char *upper,
                              char latin_lower, char latin_upper) {
    if ((pressed_key_code[1] == 0) && ((pressed_key_code[0] == latin_lower) || (pressed_key_code[0] == latin_upper))) {
        return 1;
    }

    return ((pressed_key_code[0] == lower[0]) && (pressed_key_code[1] == lower[1])) ||
           ((pressed_key_code[0] == upper[0]) && (pressed_key_code[1] == upper[1]));
}

/* utf8_is_continuation_byte отличает продолжение UTF-8 символа от начала экранной ячейки. */
static int utf8_is_continuation_byte(char c) {
    return (((unsigned char) c & 0xC0) == 0x80);
}

/* utf8_display_width считает экранную ширину подстроки в знакоместах, а не в байтах. */
static int utf8_display_width(const char *text, int bytes) {
    int i;
    int len = 0;

    for (i = 0; (i < bytes) && (text[i] != 0x0); i++) {
        if (!utf8_is_continuation_byte(text[i])) {
            len++;
        }
    }

    return len;
}

/* utf8_previous_char_pos возвращает позицию начала предыдущего UTF-8 символа. */
static int utf8_previous_char_pos(const char *text, int pos) {
    if (pos <= 0) {
        return 0;
    }

    pos--;
    while ((pos > 0) && utf8_is_continuation_byte(text[pos])) {
        pos--;
    }

    return pos;
}

/* utf8_next_char_pos возвращает позицию начала следующего UTF-8 символа в пределах буфера. */
static int utf8_next_char_pos(const char *text, int pos, int limit) {
    if ((pos >= limit) || (text[pos] == 0x0)) {
        return pos;
    }

    pos++;
    while ((pos < limit) && utf8_is_continuation_byte(text[pos])) {
        pos++;
    }

    return pos;
}

/* input_cursor_screen_x переводит байтовую позицию поля в экранную X-координату курсора. */
static int input_cursor_screen_x(int sx, const char *text, int pos, int is_utf8) {
    if (is_utf8) {
        return sx + utf8_display_width(text, pos);
    }

    return sx + pos;
}

void edit_input_field(int sx, int sy, int n, int nn) {
    int ll, kk, cc, ee, flag;
    char tmp[56], ks[2];
    char cursor[4] = "-\\|/";
    double clck, clck_;
    int is_utf8_input = n == -1;

    flag = 0;
    answer_time_limit_ticks = (clock_t) (app_settings[9] + 5) * CLK_TCK;
    clck_ = clck = (double) answer_time_limit_ticks / (double) 80;
    cursor_blink_interval = CLK_TCK / (clock_t) cursor_blink_speed;
    cc = 0;
    time_bar_position = 0;
    if (n == 8) {
        for (ll = 0; ll < nn; ll++)
            tmp[ll] = ' ';
        tmp[nn] = 0x0;
        ll = 0;
    } else {
        if (n == -1) {
            for (ll = 0; ll < nn; ll++)
                tmp[ll] = ' ';
            tmp[nn] = 0x0;
            ll = 0;
            while (player_name[ll] != 0x0) {
                tmp[ll] = player_name[ll];
                ll++;
            }
        } else {
            sprintf(tmp, "%-5i", app_settings[n]);
            tmp[nn] = 0x0;
            ll = 0;
            while ((tmp[ll] != ' ') && (tmp[ll] != 0x0))
                ll++;
        }
    }
    TextColor(RED);
    if (n == 8)
        CprintXY(3, 25, footer_help[3]);
    else
        CprintXY(3, 25, footer_help[1]);
    if (n != -1) {
        tmp[nn] = ' ';
        tmp[nn + 1] = 0x0;
    }
    TextColor(LIGHTMAGENTA);
    ee = ll;
    GotoXY(input_cursor_screen_x(sx, tmp, ll, is_utf8_input), sy);
    kk = 0;
    cursor_updated_at = answer_started_at = current_time = clock();
    while (kk != _Esc_) {
        current_time = clock();
        if ((n == 8) && ((current_time - answer_started_at) > (clock_t) clck)) {
            time_bar_position++;
            CprintXY(time_bar_position, 24, "█");
            if (time_bar_position >= 80) {
                app_settings[8] = -5;
                return;
            }
            clck += clck_;
        }
        if ((current_time - cursor_updated_at) > cursor_blink_interval) {
            cursor_updated_at = current_time;
            GetCharXY(input_cursor_screen_x(sx, tmp, ll, is_utf8_input), sy, cursor[cc]);
            cc++;
            if (cc > 3)
                cc = 0;
        }
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
                case _RPLUS_:
                    if ((clock_t) cursor_blink_speed < CLK_TCK) {
                        cursor_blink_speed++;
                        cursor_blink_interval = CLK_TCK / (clock_t) cursor_blink_speed;
                    } else
                        play_error_sound();
                    break;
                case _RMINUS_:
                    if (cursor_blink_speed > 1) {
                        cursor_blink_speed--;
                        cursor_blink_interval = CLK_TCK / (clock_t) cursor_blink_speed;
                    } else
                        play_error_sound();
                    break;
                case _0_:
                case _1_:
                case _2_:
                case _3_:
                case _4_:
                case _5_:
                case _6_:
                case _7_:
                case _8_:
                case _9_:
                case _R0_:
                case _R1_:
                case _R2_:
                case _R3_:
                case _R4_:
                case _R5_:
                case _R6_:
                case _R7_:
                case _R8_:
                case _R9_:
                    if (flag == 0) {
                        for (ll = 0; ll <= nn; ll++)
                            tmp[ll] = ' ';
                        tmp[nn + 1] = 0x0;
                        ll = 0;
                        CprintXY(sx, sy, tmp);
                    }
                    if (ll < nn) {
                        tmp[ll] = ks[0];
                        CprintXY(sx, sy, tmp);
                        ll++;
                        if (ll > ee)
                            ee = ll;
                    }
                    flag = 1;
                    break;
                case _BackSpace_:
                    if (ll > 0) {
                        int prev = is_utf8_input ? utf8_previous_char_pos(tmp, ll) : ll - 1;
                        int old_ll = ll;
                        int erase;

                        if (ll == ee)
                            ee = prev;
                        ll = prev;
                        for (erase = prev; erase < old_ll; erase++) {
                            tmp[erase] = ' ';
                        }
                        CprintXY(sx, sy, tmp);
                        while ((ee > 0) && (tmp[ee - 1] == ' '))
                            ee--;
                    }
                    flag = 1;
                    break;
                case _Space_:
                    if (n == -1) {
                        if (flag == 0) {
                            for (ll = 0; ll <= nn; ll++)
                                tmp[ll] = ' ';
                            tmp[nn + 1] = 0x0;
                            ll = 0;
                            CprintXY(sx, sy, tmp);
                        }
                        if (ll < nn) {
                            tmp[ll] = ks[0];
                            CprintXY(sx, sy, tmp);
                            ll++;
                            if (ll > ee)
                                ee = ll;
                        }
                        flag = 1;
                        break;
                    }
                case _Enter_:
                    if (n == -1) {
                        for (ll = 0; ll < 46; ll++)
                            player_name[ll] = tmp[ll];
                        player_name[45] = 0x0;
                        ll = 45;
                        while ((player_name[ll] == ' ') || (player_name[ll] == 0x0)) {
                            player_name[ll] = 0x0;
                            ll--;
                        }
                    } else
                        app_settings[n] = atoi(tmp);
                    kk = _Esc_;
                    break;
                case _Left_:
                    if (ll > 0) {
                        ll = is_utf8_input ? utf8_previous_char_pos(tmp, ll) : ll - 1;
                        CprintXY(sx, sy, tmp);
                    }
                    flag = 1;
                    break;
                case _Right_:
                    if (ll < nn) {
                        ll = is_utf8_input ? utf8_next_char_pos(tmp, ll, nn) : ll + 1;
                        CprintXY(sx, sy, tmp);
                    }
                    flag = 1;
                    break;
                case _Home_:
                    if (ll != 0) {
                        ll = 0;
                        CprintXY(sx, sy, tmp);
                    }
                    flag = 1;
                    break;
                case _End_:
                    if (ll != ee) {
                        ll = ee;
                        CprintXY(sx, sy, tmp);
                    }
                    flag = 1;
                    break;
                case _Esc_:
                    if (n == 8) {
                        app_settings[8] = -7;
                        return;
                    }
                    break;
                case _F10_:
                    if (n == 8) {
                        app_settings[8] = -3;
                        return;
                    }
                    break;
                default:
                    if (n == -1) {
                        if (flag == 0) {
                            for (ll = 0; ll <= nn; ll++)
                                tmp[ll] = ' ';
                            tmp[nn + 1] = 0x0;
                            ll = 0;
                            CprintXY(sx, sy, tmp);
                        }
                        if (ll < nn) {
                            int bytes = ks[1] != 0 ? 2 : 1;

                            if ((ll + bytes) > nn) {
                                play_error_sound();
                                flag = 1;
                                break;
                            }
                            tmp[ll] = ks[0];
                            ll++;
                            if ((ks[1] != 0) && (ll < nn)) {
                                tmp[ll] = ks[1];
                                ll++;
                            }
                            CprintXY(sx, sy, tmp);
                            if (ll > ee)
                                ee = ll;
                        }
                        flag = 1;
                    }
                    break;
            }
        }
    }
    if (n == -1) {
        CprintXY(sx, sy, player_name);
        CprintXY(80, sy, " ");
        ll = 45;
        while (player_name[ll] == ' ') {
            player_name[ll] = 0x0;
            ll--;
        }
        for (ll = 0; ll < 46; ll++)
            tmp[ll] = ' ';
        tmp[46] = 0x0;
        CprintXY(sx, sy, tmp);
        CprintXY(sx, sy, player_name);
    } else {
        switch (n) {
            case 5:
                if ((app_settings[6] - app_settings[5]) < 50)
                    app_settings[5] = app_settings[6] - 50;
                break;
            case 6:
                if ((app_settings[6] - app_settings[5]) < 50)
                    app_settings[6] = app_settings[5] + 50;
                break;
            case 7:
                if (app_settings[7] < 5)
                    app_settings[7] = 5;
                if (app_settings[7] > 200)
                    app_settings[7] = 200;
                break;
        }
        if (nn == 5) {
            sprintf(tmp, "%-6i", app_settings[n]);
            CprintXY(sx, sy, tmp);
        } else {
            sprintf(tmp, "%-5i", app_settings[n]);
            CprintXY(sx, sy, tmp);
        }
        if (n != 8) {
            TextColor(RED);
            CprintXY(3, 25, footer_help[0]);
        }
    }
}
