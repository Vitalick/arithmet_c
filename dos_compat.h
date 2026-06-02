#ifndef DOS_COMPAT_H
#define DOS_COMPAT_H

#include <stdio.h>
#include <time.h>

#ifndef CLK_TCK
#define CLK_TCK CLOCKS_PER_SEC
#endif

/* date повторяет структуру Turbo C для календарной даты. */
struct date {
    int da_year;
    char da_day;
    char da_mon;
};

/* time повторяет структуру Turbo C для времени суток. */
struct time {
    unsigned char ti_min;
    unsigned char ti_hour;
    unsigned char ti_hund;
    unsigned char ti_sec;
};

/* ftime хранит DOS-время файла с секундами, сжатыми до двухсекундных тиков. */
struct ftime {
    unsigned ft_tsec;
    unsigned ft_min;
    unsigned ft_hour;
    unsigned ft_day;
    unsigned ft_month;
    unsigned ft_year;
};

/* Инициализирует генератор случайных чисел аналогично randomize из Turbo C. */
void randomize(void);

/* Возвращает случайное число из диапазона [0, max); при max <= 0 возвращает 0. */
int dos_random(int max);

/* Делает синхронную задержку в миллисекундах. */
void delay(unsigned int ms);

/* Подает короткий звуковой сигнал; частота сохранена только для совместимости API. */
void sound(unsigned int frequency);

/* Завершает звук; в переносимой реализации является no-op. */
void nosound(void);

/* Заполняет DOS-структуру текущей календарной датой. */
void dos_getdate(struct date *date);

/* Заполняет DOS-структуру текущим временем. */
void gettime(struct time *time);

/* Заполняет DOS-структуру времени файла; текущая реализация использует текущее время. */
void getftime(FILE *file, struct ftime *ftime);

#endif
