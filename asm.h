#ifndef ARITHMET_ASM_H
#define ARITHMET_ASM_H

#include "dos_compat.h"

/* Собирает старый двухбайтовый код клавиши и записывает его байты в s1/s2. */
int MakeKey(int code, char *s1, char *s2);

/* Преобразует обычный ASCII-символ в DOS-код клавиши, если для него есть специальная константа. */
int MapAsciiKey(int c, char *s1, char *s2);

/* Переводит BCD-байт в десятичное число, повторяя смысл инструкции AAD. */
int AAD(char);

/* Переводит число 0-99 в BCD-байт, повторяя смысл инструкции AAM. */
char AAM(int);

/* Возвращает беззнаковое числовое значение байта символа. */
int ASCII(char);

/* Возвращает символ по младшему байту целого числа. */
char CHR(int);

/* Сохраняет совместимость с установкой одного регистра DAC-палитры BIOS 10h. */
void SetDACpalette1(int, char, char, char);

/* Сохраняет совместимость с чтением DAC-палитры BIOS 10h. */
void GetDACpalette(int, int, char *);

/* Читает клавиатуру и возвращает старый двухбайтовый DOS-код клавиши. */
int Getc(char *, char *);

/* Сохраняет совместимость с BIOS-имитацией нажатия клавиши. */
void EmulPressKey(char, char);

/* Выводит символ в stdout вместо старого порта принтера BIOS 17h. */
char outchartoprn(char);

/* Заполняет структуру времени через переносимый слой dos_compat. */
void _Time(struct time *);

/* Заполняет структуру даты через переносимый слой dos_compat. */
void _Date(struct date *);

/* Сохраняет совместимость с установкой обработчика прерывания DOS 21h. */
void addrint(char, unsigned int, unsigned int);

#endif
