#include <dos.h>
#include "asm.h"

// Перевод числа из BCD-формата в десятичный формат
//==========================================
int AAD(char s) {
    int i;

    asm
    {
        mov al, s
        mov ah, s
        mov cl,
        4
        ror ah, cl
        or ax,
        0f0f0h
        xor ax,
        0f0f0h
        aad
    mov i, ax
    };
    return i;
}

// Перевод целого числа в BCD-формат
//==========================================
char AAM(int i) {
    char c;

    asm
    {
        mov ax, i
        aam
    mov cl,
        4
        rol ah, cl
        or al,
        0f0h
        or ah,
        0fh
        and al, ah
        mov c, al
    };
    return c;
}

// Перевод символа в целое число
//==========================================
int ASCII(char c) {
    int i = 0;

    asm
    {
        mov ax,
        0
        mov al, c
        mov i, ax
    };
    return i;
}

// Перевод целого числа в символ
//==========================================
char CHR(int i) {
    char c = 0x0;

    asm
    {
        mov ax, i
        mov c, al
    };
    return c;
}

// Установка DAC палитры для одного цвета
//==========================================
void SetDACpalette1(int nn, char rr, char gg, char bb) {
    asm
    {
        mov ax,
        1010h
        mov bx, nn
        mov ch, gg
        mov cl, bb
        mov dh, rr
    int
        10h
    }
}

// Получение DAC палитры
//==========================================
void GetDACpalette(int sn, int nn, char *buf) {
    int eess, ddxx;

    eess = FP_SEG(buf);
    ddxx = FP_OFF(buf);
    asm
    {
        mov ax,
        1017h
        mov bx, sn
        mov cx, nn
        mov es, eess
        mov dx, ddxx
    int
        10h
    }
}

// Прием символа с клавиатуры
//==========================================
int Getc(char *s1, char *s2) {
    char c1 = 0, c2 = 0;
    int c3 = 0;

    asm
    {
        mov al,
        0
        mov ah,
        1
    int
        16h
        jz quit
        mov al,
        0
        mov ah,
        0
    int
        16h
        mov c1, al
        mov c2, ah
        mov c3, ax
    }
quit:
    *s1 = c1;
    *s2 = c2;
    return (c3);
}

// Имитация нажатия клавиши
//==========================================
void EmulPressKey(char scen, char code) {
    asm
    {
        mov al,
        00h
        mov ah,
        05h
        mov ch, scen
        mov cl, code
    int
        16h
    }
}

// Вывод символа на принтер
//==========================================
char outchartoprn(char c) {
    char d;

    asm
    {
        mov ah,
        0
        mov al, c
        mov dx,
        0
    int
        17h
        mov d, ah
    }
    return (d);
}

// Получение реального времени
//==========================================
void _Time(struct time *TTT) {
    char hhh, mmm, sss;

    asm
    {
        mov ah,
        2
    int
        1ah
        mov hhh, ch
        mov mmm, cl
        mov sss, dh
    };
    TTT->ti_hour = CHR(AAD(hhh));
    TTT->ti_min = CHR(AAD(mmm));
    TTT->ti_sec = CHR(AAD(sss));
}

// Получение реальной даты
//==========================================
void _Date(struct date *DDD) {
    char vvv, yyy, mmm, ddd;

    asm
    {
        mov ah,
        4
    int
        1ah
        mov vvv, ch
        mov yyy, cl
        mov mmm, dh
        mov ddd, dl
    };
    DDD->da_year = AAD(vvv) * 100 + AAD(yyy);
    DDD->da_mon = CHR(AAD(mmm));
    DDD->da_day = CHR(AAD(ddd));
}

// Установка адреса прерыванмя
//==========================================
void addrint(char Nint, unsigned int a, unsigned int b) {
    asm
    {
        mov ah,
        25h
        mov al, Nint;
        push ds
        mov ds, a
        mov dx, b
    int
        21h
        pop ds
    }
}
