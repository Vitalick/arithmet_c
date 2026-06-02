#ifndef DOS_COMPAT_H
#define DOS_COMPAT_H

#include <stdio.h>
#include <time.h>

#ifndef CLK_TCK
#define CLK_TCK CLOCKS_PER_SEC
#endif

struct date {
    int da_year;
    char da_day;
    char da_mon;
};

struct time {
    unsigned char ti_min;
    unsigned char ti_hour;
    unsigned char ti_hund;
    unsigned char ti_sec;
};

struct ftime {
    unsigned ft_tsec;
    unsigned ft_min;
    unsigned ft_hour;
    unsigned ft_day;
    unsigned ft_month;
    unsigned ft_year;
};

void randomize(void);
int dos_random(int max);
void delay(unsigned int ms);
void sound(unsigned int frequency);
void nosound(void);
void dos_getdate(struct date *date);
void gettime(struct time *time);
void getftime(FILE *file, struct ftime *ftime);

#endif
