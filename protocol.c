/*
 * protocol.c
 *
 * Сохраняет текстовый протокол сеанса и формирует DOS-совместимое имя файла результата.
 */

#include <stdio.h>
#include <string.h>

#include "app_state.h"
#include "dos_compat.h"
#include "asm.h"
#include "protocol.h"

static const char *transliterate_utf8_russian_letter(const char *text, int *bytes) {
    int i;
    static const char *upper[] = {
        "А", "Б", "В", "Г", "Д", "Е", "Ж", "З",
        "И", "Й", "К", "Л", "М", "Н", "О", "П",
        "Р", "С", "Т", "У", "Ф", "Х", "Ц", "Ч",
        "Ш", "Щ", "Ъ", "Ы", "Ь", "Э", "Ю", "Я"
    };
    static const char *lower[] = {
        "а", "б", "в", "г", "д", "е", "ж", "з",
        "и", "й", "к", "л", "м", "н", "о", "п",
        "р", "с", "т", "у", "ф", "х", "ц", "ч",
        "ш", "щ", "ъ", "ы", "ь", "э", "ю", "я"
    };
    static const char *latin[] = {
        "A", "B", "W", "G", "D", "E", "G", "Z",
        "I", "J", "K", "L", "M", "N", "O", "P",
        "R", "S", "T", "U", "F", "H", "C", "CH",
        "SH", "SH", "'", "Y", "J", "E", "U", "A"
    };

    for (i = 0; i < 32; i++) {
        if ((text[0] == upper[i][0]) && (text[1] == upper[i][1])) {
            *bytes = 2;
            return latin[i];
        }
        if ((text[0] == lower[i][0]) && (text[1] == lower[i][1])) {
            *bytes = 2;
            return latin[i];
        }
    }

    *bytes = 1;
    return NULL;
}

int is_older_file_time(const struct ftime *FTNew, const struct ftime *FT) {
    if (FTNew->ft_year > FT->ft_year) return 0;
    if (FTNew->ft_year < FT->ft_year) return 1;
    if (FTNew->ft_month > FT->ft_month) return 0;
    if (FTNew->ft_month < FT->ft_month) return 1;
    if (FTNew->ft_day > FT->ft_day) return 0;
    if (FTNew->ft_day < FT->ft_day) return 1;
    if (FTNew->ft_hour > FT->ft_hour) return 0;
    if (FTNew->ft_hour < FT->ft_hour) return 1;
    if (FTNew->ft_min > FT->ft_min) return 0;
    if (FTNew->ft_min < FT->ft_min) return 1;
    if (FTNew->ft_tsec > FT->ft_tsec) return 0;
    if (FTNew->ft_tsec < FT->ft_tsec) return 1;
    return 0;
}

void save_session_protocol(void) {
    int i, j, k, l, m;
    FILE *protocol_file;
    char filename[13] = "________.000";
    struct ftime FT, FTnew;
    struct date D;
    struct time T;
    char tmp[80];

    for (i = 0; i < 8; i++)
        filename[i] = '_';
    filename[8] = '.';
    for (i = 9; i < 12; i++)
        filename[i] = '0';
    filename[12] = 0x0;
    i = 0;
    for (k = 0; (player_name[k] != 0x0) && (i <= 6);) {
        const char *latin;
        int bytes;

        if (player_name[k] == 0x0)
            break;

        latin = transliterate_utf8_russian_letter(&player_name[k], &bytes);
        if (latin != NULL) {
            for (j = 0; (latin[j] != 0x0) && (i <= 6); j++) {
                filename[i] = latin[j];
                i++;
            }
            k += bytes;
            continue;
        }

        if (((unsigned char) player_name[k] & 0x80) == 0) {
            filename[i] = player_name[k];
            i++;
            k++;
        } else {
            k++;
            while ((((unsigned char) player_name[k] & 0xC0) == 0x80) && (player_name[k] != 0x0)) {
                k++;
            }
        }
    }
    filename[8] = '.';
    j = 0;
    k = 0;
    sprintf(&filename[9], "%03i", j);
    protocol_file = NULL;
    protocol_file = fopen(filename, "rb");
    getftime(protocol_file, &FT);
    while (protocol_file != NULL) {
        getftime(protocol_file, &FTnew);
        l = is_older_file_time(&FTnew, &FT);

        if (l == 1) {
            getftime(protocol_file, &FT);
            k = j;
        }
        fclose(protocol_file);
        j++;
        if (j > 999) {
            sprintf(&filename[9], "%03i", k);
            protocol_file = NULL;
        } else {
            sprintf(&filename[9], "%03i", j);
            protocol_file = NULL;
            protocol_file = fopen(filename, "rb");
        }
    }
    protocol_file = fopen(filename, "wb");
    dos_getdate(&D);
    gettime(&T);
    fprintf(protocol_file, "Имя: %s     дата: %2i.%02i.%02i  время: %2i:%02i:%02i\r\n",
            session_result.name, ASCII(D.da_day), ASCII(D.da_mon), D.da_year,
            ASCII(T.ti_hour), ASCII(T.ti_min), ASCII(T.ti_sec));
    fprintf(protocol_file, "Действия: ");
    l = 0;
    for (i = 0; i < 5; i++) {
        if (session_result.enabled_operations[i] == 1) {
            if (l == 1)
                fprintf(protocol_file, ",");
            fprintf(protocol_file, " %s", operation_names[i]);
            l = 1;
        }
    }
    fprintf(protocol_file, "\r\nКоличество: %i,  пределы: от %i до %i,  сложность: %i\r\n",
            session_result.planned_count, session_result.result_limits[0], session_result.result_limits[1], session_result.time_limit_seconds);
    fprintf(protocol_file, "Верных ответов: %i,  оценка: %i\r\n", session_result.correct_count, session_result.grade);
    fprintf(protocol_file, "Количество действий: ");
    l = 0;
    for (i = 0; i < 5; i++) {
        if (session_result.enabled_operations[i] == 1) {
            if (l == 1)
                fprintf(protocol_file, "                     ");
            fprintf(protocol_file, "%19s - %i, верных ответов - %i\r\n", operation_names[i],
                    session_result.operation_counts[i], session_result.correct_operation_counts[i]);
            l = 1;
        }
    }
    fprintf(protocol_file, "Предложенные действия:\r\n");
    l = 0;
    while ((session_result.answers[l].operation_index != -10) || (session_result.answers[l].is_correct != -10)) {
        fprintf(protocol_file, "%3i. ", l + 1);
        if (session_result.answers[l].is_correct == 0)
            fprintf(protocol_file, "не");
        else
            fprintf(protocol_file, "  ");
        snprintf(tmp, sizeof(tmp), "%i%c%i=%i", session_result.answers[l].values[0],
                 operation_symbols[session_result.answers[l].operation_index], session_result.answers[l].values[1],
                 session_result.answers[l].values[2]);
        fprintf(protocol_file, "верно:   %-18s", tmp);
        switch (session_result.answers[l].values[3]) {
            case -7:
                fprintf(protocol_file, "нажата клавиша <Esc>");
                break;
            case -5:
                fprintf(protocol_file, "вышло время         ");
                break;
            case -3:
                fprintf(protocol_file, "нажата клавиша <F10>");
                break;
            default:
                fprintf(protocol_file, "введено %-5i       ", session_result.answers[l].values[3]);
                break;
        }
        m = session_result.answers[l].elapsed_seconds - (session_result.answers[l].elapsed_seconds / 10) * 10;
        if ((session_result.answers[l].elapsed_seconds > 10) && (session_result.answers[l].elapsed_seconds < 20))
            m = 0;
        switch (m) {
            case 0:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
                snprintf(tmp, sizeof(tmp), "  прошло %3i секунд\r\n", session_result.answers[l].elapsed_seconds);
                break;
            case 1:
                snprintf(tmp, sizeof(tmp), "  прошла %3i секунда\r\n", session_result.answers[l].elapsed_seconds);
                break;
            case 2:
            case 3:
            case 4:
                snprintf(tmp, sizeof(tmp), "  прошло %3i секунды\r\n", session_result.answers[l].elapsed_seconds);
                break;
        }
        fprintf(protocol_file, "%s", tmp);
        l++;
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
        fprintf(protocol_file, "                     Время:  лучшее - %i, худшее - %i, среднее - %i\r\n", i, j, k);
    }
    fclose(protocol_file);
}
