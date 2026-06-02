/*
 * main.c
 *
 * Содержит точку входа и главный цикл стартового экрана.
 */

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>

#include "asm.h"
#include "app_state.h"
#include "dos_compat.h"
#include "input.h"
#include "keys.h"
#include "mouse.h"
#include "screens.h"
#include "session.h"
#include "ui_helpers.h"

int main(void) {
    int i, flag;

    mouse_max_x = 80 * 8 - 1;
    mouse_max_y = 25 * 8 - 1;
    mouse_cursor_mask_a_bytes = (char *) &mouse_cursor_mask_a;
    mouse_cursor_mask_b_bytes = (char *) &mouse_cursor_mask_b;
    mouse_cursor_mask_a = 0x8FF;
    mouse_cursor_mask_b = 0x2500;
    mouse_driver_status = Initialize(&mouse_key_status);
    randomize();
    settings_file = fopen("a.dat", "rb");
    if (settings_file != NULL) {
        fread(app_settings, sizeof(int), 10, settings_file);
        fread(player_name, 46, 1, settings_file);
        fclose(settings_file);
    }
    if (player_name[0] == 0x0) {
        for (i = 0; i < 46; i++)
            player_name[i] = 0x0;
        i = 0;
        while (default_player_name[i] != 0x0) {
            player_name[i] = default_player_name[i];
            i++;
        }
    }
    for (i = 0; i < 5; i++)
        if ((app_settings[i] != 0) && (app_settings[i] != 1))
            app_settings[i] = 1;
    if (app_settings[5] < 0)
        app_settings[5] = 0;
    if ((app_settings[6] - app_settings[5]) < 50)
        app_settings[6] = app_settings[5] + 50;
    if ((app_settings[7] < 5) || (app_settings[7] > 50))
        app_settings[7] = 20;
    if ((app_settings[9] < 0) || (app_settings[9] > 99))
        app_settings[9] = 30;
    _setcursortype(_NOCURSOR);
    SetTerminalResizeHandler(redraw_active_screen_for_resize);
    Npage(1, 1);
    active_screen = 1;
    create_main_screen();
    pressed_key_code = 0;
    flag = 0;
    if (mouse_driver_status == -1) {
        mouse_dx = 1;
        mouse_dy = 1;
        SetTextCursor(0, mouse_cursor_mask_a, mouse_cursor_mask_b);
        mouse_buttons = GetPos(&mouse_x, &mouse_y);
    }
    idle_poll_counter = 0;
    while (pressed_key_code != _Esc_) {
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
        pressed_key_code = Getc(&pressed_key_bytes[0], &pressed_key_bytes[1]);
        if (pressed_key_code != 0) {
            switch (pressed_key_code) {
                case _F5_:
                    if (mouse_cursor_mask_a_bytes[0] > 0x0) {
                        mouse_cursor_mask_a_bytes[0]--;
                        SetTextCursor(0, mouse_cursor_mask_a, mouse_cursor_mask_b);
                    }
                    break;
                case _F6_:
                    if (mouse_cursor_mask_a_bytes[0] < 0xFF) {
                        mouse_cursor_mask_a_bytes[0]++;
                        SetTextCursor(0, mouse_cursor_mask_a, mouse_cursor_mask_b);
                    }
                    break;
                case _F7_:
                    if (mouse_cursor_mask_b_bytes[0] > 0x0) {
                        mouse_cursor_mask_b_bytes[0]--;
                        SetTextCursor(0, mouse_cursor_mask_a, mouse_cursor_mask_b);
                    }
                    break;
                case _F8_:
                    if (mouse_cursor_mask_b_bytes[0] < 0xFF) {
                        mouse_cursor_mask_b_bytes[0]++;
                        SetTextCursor(0, mouse_cursor_mask_a, mouse_cursor_mask_b);
                    }
                    break;
                case _5_:
                    if (mouse_cursor_mask_a_bytes[1] > 0x0) {
                        mouse_cursor_mask_a_bytes[1]--;
                        SetTextCursor(0, mouse_cursor_mask_a, mouse_cursor_mask_b);
                    }
                    break;
                case _6_:
                    if (mouse_cursor_mask_a_bytes[1] < 0xFF) {
                        mouse_cursor_mask_a_bytes[1]++;
                        SetTextCursor(0, mouse_cursor_mask_a, mouse_cursor_mask_b);
                    }
                    break;
                case _7_:
                    if (mouse_cursor_mask_b_bytes[1] > 0x0) {
                        mouse_cursor_mask_b_bytes[1]--;
                        SetTextCursor(0, mouse_cursor_mask_a, mouse_cursor_mask_b);
                    }
                    break;
                case _8_:
                    if (mouse_cursor_mask_b_bytes[1] < 0xFF) {
                        mouse_cursor_mask_b_bytes[1]++;
                        SetTextCursor(0, mouse_cursor_mask_a, mouse_cursor_mask_b);
                    }
                    break;
                case _SEQUAL_:
                case _RPLUS_:
                    app_settings[0] = 1 - app_settings[0];
                    if ((app_settings[0] == 0) && (app_settings[1] == 0) &&
                        (app_settings[2] == 0) && (app_settings[3] == 0) && (app_settings[4] == 0))
                        app_settings[0] = 1 - app_settings[0];
                    else
                        refresh_operation_status();
                    break;
                case _MINUS_:
                case _RMINUS_:
                    app_settings[1] = 1 - app_settings[1];
                    if ((app_settings[0] == 0) && (app_settings[1] == 0) &&
                        (app_settings[2] == 0) && (app_settings[3] == 0) && (app_settings[4] == 0))
                        app_settings[1] = 1 - app_settings[1];
                    else
                        refresh_operation_status();
                    break;
                case _S8_:
                case _RASTER_:
                    app_settings[2] = 1 - app_settings[2];
                    if ((app_settings[0] == 0) && (app_settings[1] == 0) &&
                        (app_settings[2] == 0) && (app_settings[3] == 0) && (app_settings[4] == 0))
                        app_settings[2] = 1 - app_settings[2];
                    else
                        refresh_operation_status();
                    break;
                case _SLASH_:
                    app_settings[3] = 1 - app_settings[3];
                    if ((app_settings[0] == 0) && (app_settings[1] == 0) &&
                        (app_settings[2] == 0) && (app_settings[3] == 0) && (app_settings[4] == 0))
                        app_settings[3] = 1 - app_settings[3];
                    else
                        refresh_operation_status();
                    break;
                case _DOTCOM_:
                case _SDOTCOM_:
                case _LINE_:
                case _SLINE_:
                case _RDOT_:
                    app_settings[4] = 1 - app_settings[4];
                    if ((app_settings[0] == 0) && (app_settings[1] == 0) &&
                        (app_settings[2] == 0) && (app_settings[3] == 0) && (app_settings[4] == 0))
                        app_settings[4] = 1 - app_settings[4];
                    else
                        refresh_operation_status();
                    break;
                case _Enter_:
                    reset_session_result();
                    run_training_session();
                    active_screen = 1;
                    create_main_screen();
                    flag = 1;
                    break;
                case _F1_:
                    if (flag == 1)
                        show_session_results();
                    break;
                default:
                    if (key_matches_utf8_or_latin(pressed_key_bytes, "и", "И", 'b', 'B')) {
                        edit_input_field(34, 6, -1, 46);
                        if (player_name[0] == 0x0) {
                            for (i = 0; i < 46; i++)
                                player_name[i] = 0x0;
                            i = 0;
                            while (default_player_name[i] != 0x0) {
                                player_name[i] = default_player_name[i];
                                i++;
                            }
                        }
                    } else if (key_matches_utf8_or_latin(pressed_key_bytes, "о", "О", 'j', 'J')) {
                        edit_input_field(54, 7, 5, 5);
                    } else if (key_matches_utf8_or_latin(pressed_key_bytes, "д", "Д", 'l', 'L')) {
                        edit_input_field(64, 7, 6, 5);
                    } else if (key_matches_utf8_or_latin(pressed_key_bytes, "к", "К", 'r', 'R')) {
                        edit_input_field(51, 8, 7, 4);
                    } else if (key_matches_utf8_or_latin(pressed_key_bytes, "с", "С", 'c', 'C')) {
                        edit_input_field(72, 8, 9, 2);
                    }
                    break;
            }
        }
    }
    Npage(0, 1);
    _setcursortype(_NORMALCURSOR);
    settings_file = fopen("a.dat", "wb");
    fwrite(app_settings, sizeof(int), 10, settings_file);
    fwrite(player_name, 46, 1, settings_file);
    fclose(settings_file);

    return 0;
}

