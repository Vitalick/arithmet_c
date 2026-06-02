#include "dos_compat.h"
#include "asm.h"

#include <stdio.h>

// Перевод числа из BCD-формата в десятичный формат
//==========================================
int AAD(char s) {
    unsigned char value = (unsigned char) s;

    return ((value >> 4) & 0x0F) * 10 + (value & 0x0F);
}

// Перевод целого числа в BCD-формат
//==========================================
char AAM(int i) {
    return (char) (((i / 10) << 4) | (i % 10));
}

// Перевод символа в целое число
//==========================================
int ASCII(char c) {
    return (unsigned char) c;
}

// Перевод целого числа в символ
//==========================================
char CHR(int i) {
    return (char) i;
}

// Установка DAC палитры для одного цвета
//==========================================
void SetDACpalette1(int nn, char rr, char gg, char bb) {
    (void) nn;
    (void) rr;
    (void) gg;
    (void) bb;
}

// Получение DAC палитры
//==========================================
void GetDACpalette(int sn, int nn, char *buf) {
    (void) sn;
    (void) nn;
    (void) buf;
}

// Прием символа с клавиатуры
//==========================================
int Getc(char *s1, char *s2) {
    int c = getchar();
    int c2;

    if (c == EOF) {
        *s1 = 0;
        *s2 = 0;
        return 0;
    }

    *s1 = (char) c;
    if ((c & 0xE0) == 0xC0) {
        c2 = getchar();
        if (c2 != EOF) {
            *s2 = (char) c2;
            return ((unsigned char) *s1 << 8) | (unsigned char) *s2;
        }
    }

    *s2 = 0;
    return (unsigned char) *s1;
}

// Имитация нажатия клавиши
//==========================================
void EmulPressKey(char scen, char code) {
    (void) scen;
    (void) code;
}

// Вывод символа на принтер
//==========================================
char outchartoprn(char c) {
    fputc(c, stdout);
    return 0;
}

// Получение реального времени
//==========================================
void _Time(struct time *TTT) {
    gettime(TTT);
}

// Получение реальной даты
//==========================================
void _Date(struct date *DDD) {
    getdate(DDD);
}

// Установка адреса прерыванмя
//==========================================
void addrint(char Nint, unsigned int a, unsigned int b) {
    (void) Nint;
    (void) a;
    (void) b;
}
