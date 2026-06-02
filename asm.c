/*
 * asm.c
 *
 * Сохраняет интерфейс старого ASM.C: BCD-преобразования, BIOS/DOS-заглушки и обертки даты/времени.
 */

#include "dos_compat.h"
#include "asm.h"

#include <stdio.h>

/* AAD переводит число из BCD-формата в десятичный формат. */
int AAD(char s) {
    unsigned char value = (unsigned char) s;

    return ((value >> 4) & 0x0F) * 10 + (value & 0x0F);
}

/* AAM переводит целое число в BCD-формат. */
char AAM(int i) {
    return (char) (((i / 10) << 4) | (i % 10));
}

/* ASCII возвращает беззнаковое числовое значение байта. */
int ASCII(char c) {
    return (unsigned char) c;
}

/* CHR возвращает младший байт числа как символ. */
char CHR(int i) {
    return (char) i;
}

/* SetDACpalette1 сохранен как no-op совместимость с BIOS-палитрой. */
void SetDACpalette1(int nn, char rr, char gg, char bb) {
    (void) nn;
    (void) rr;
    (void) gg;
    (void) bb;
}

/* GetDACpalette сохранен как no-op совместимость с BIOS-палитрой. */
void GetDACpalette(int sn, int nn, char *buf) {
    (void) sn;
    (void) nn;
    (void) buf;
}

/* EmulPressKey сохранен для совместимости, но переносимая версия не кладет события в очередь клавиатуры. */
void EmulPressKey(char scen, char code) {
    (void) scen;
    (void) code;
}

/* outchartoprn заменяет старый BIOS-принтер обычным stdout. */
char outchartoprn(char c) {
    fputc(c, stdout);
    return 0;
}

/* _Time заполняет старую структуру времени через dos_compat. */
void _Time(struct time *TTT) {
    gettime(TTT);
}

/* _Date заполняет старую структуру даты через dos_compat. */
void _Date(struct date *DDD) {
    dos_getdate(DDD);
}

/* addrint сохранен как no-op совместимость с установкой адреса DOS-прерывания. */
void addrint(char Nint, unsigned int a, unsigned int b) {
    (void) Nint;
    (void) a;
    (void) b;
}
