#include "dos_compat.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void randomize(void) {
    srand((unsigned int) time(NULL));
}

int dos_random(int max) {
    if (max <= 0) {
        return 0;
    }

    return rand() % max;
}

void delay(unsigned int ms) {
    const clock_t start = clock();
    const clock_t duration = (clock_t) ((double) ms * CLOCKS_PER_SEC / 1000.0);

    while ((clock() - start) < duration) {
    }
}

void sound(unsigned int frequency) {
    (void) frequency;
    fputc('\a', stdout);
    fflush(stdout);
}

void nosound(void) {
}

void dos_getdate(struct date *date) {
    time_t now;
    struct tm *local;

    if (date == NULL) {
        return;
    }

    now = time(NULL);
    local = localtime(&now);
    if (local == NULL) {
        return;
    }

    date->da_year = local->tm_year + 1900;
    date->da_mon = (char) (local->tm_mon + 1);
    date->da_day = (char) local->tm_mday;
}

void gettime(struct time *current_time) {
    time_t now;
    struct tm *local;

    if (current_time == NULL) {
        return;
    }

    now = time(NULL);
    local = localtime(&now);
    if (local == NULL) {
        return;
    }

    current_time->ti_hour = (unsigned char) local->tm_hour;
    current_time->ti_min = (unsigned char) local->tm_min;
    current_time->ti_sec = (unsigned char) local->tm_sec;
    current_time->ti_hund = 0;
}

void getftime(FILE *file, struct ftime *ftime) {
    time_t now;
    struct tm *local;

    (void) file;

    if (ftime == NULL) {
        return;
    }

    now = time(NULL);
    local = localtime(&now);
    if (local == NULL) {
        return;
    }

    ftime->ft_tsec = (unsigned) (local->tm_sec / 2);
    ftime->ft_min = (unsigned) local->tm_min;
    ftime->ft_hour = (unsigned) local->tm_hour;
    ftime->ft_day = (unsigned) local->tm_mday;
    ftime->ft_month = (unsigned) (local->tm_mon + 1);
    ftime->ft_year = (unsigned) local->tm_year;
}
