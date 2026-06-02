#include "dos_compat.h"
#include "asm.h"
#include "conio.h"
#include "keys.h"
#include "terminal_platform.h"

#include <stdio.h>
#include <stdlib.h>

int MakeKey(int code, char *s1, char *s2) {
    *s1 = (char) (code & 0xFF);
    *s2 = (char) ((code >> 8) & 0xFF);

    return code;
}

int MapAsciiKey(int c, char *s1, char *s2) {
    switch (c) {
        case 0x08:
        case 0x7F:
            return MakeKey(_BackSpace_, s1, s2);
        case '\t':
            return MakeKey(_Tab_, s1, s2);
        case '\r':
        case '\n':
            return MakeKey(_Enter_, s1, s2);
        case ' ':
            return MakeKey(_Space_, s1, s2);
        case '+':
            return MakeKey(_SEQUAL_, s1, s2);
        case '-':
            return MakeKey(_MINUS_, s1, s2);
        case '*':
            return MakeKey(_S8_, s1, s2);
        case '/':
            return MakeKey(_SLASH_, s1, s2);
        case ':':
            return MakeKey(_SDOTCOM_, s1, s2);
        case ';':
            return MakeKey(_DOTCOM_, s1, s2);
        case '\\':
            return MakeKey(_LINE_, s1, s2);
        case '|':
            return MakeKey(_SLINE_, s1, s2);
        case '0':
            return MakeKey(_0_, s1, s2);
        case '1':
            return MakeKey(_1_, s1, s2);
        case '2':
            return MakeKey(_2_, s1, s2);
        case '3':
            return MakeKey(_3_, s1, s2);
        case '4':
            return MakeKey(_4_, s1, s2);
        case '5':
            return MakeKey(_5_, s1, s2);
        case '6':
            return MakeKey(_6_, s1, s2);
        case '7':
            return MakeKey(_7_, s1, s2);
        case '8':
            return MakeKey(_8_, s1, s2);
        case '9':
            return MakeKey(_9_, s1, s2);
        default:
            *s1 = (char) c;
            *s2 = 0;
            return (unsigned char) *s1;
    }
}

static int map_csi_sequence(char *s1, char *s2) {
    int c = ReadTerminalByte(10000);

    switch (c) {
        case 'A':
            return MakeKey(_Up_, s1, s2);
        case 'B':
            return MakeKey(_Down_, s1, s2);
        case 'C':
            return MakeKey(_Right_, s1, s2);
        case 'D':
            return MakeKey(_Left_, s1, s2);
        case 'F':
            return MakeKey(_End_, s1, s2);
        case 'H':
            return MakeKey(_Home_, s1, s2);
        case '1':
            c = ReadTerminalByte(10000);
            if (c == '~') {
                return MakeKey(_Home_, s1, s2);
            }
            if (c == '5') {
                ReadTerminalByte(10000);
                return MakeKey(_F5_, s1, s2);
            }
            if (c == '7') {
                ReadTerminalByte(10000);
                return MakeKey(_F6_, s1, s2);
            }
            if (c == '8') {
                ReadTerminalByte(10000);
                return MakeKey(_F7_, s1, s2);
            }
            if (c == '9') {
                ReadTerminalByte(10000);
                return MakeKey(_F8_, s1, s2);
            }
            break;
        case '2':
            c = ReadTerminalByte(10000);
            if (c == '1') {
                ReadTerminalByte(10000);
                return MakeKey(_F10_, s1, s2);
            }
            break;
        case '4':
            ReadTerminalByte(10000);
            return MakeKey(_End_, s1, s2);
        case '5':
            ReadTerminalByte(10000);
            return MakeKey(_PageUp_, s1, s2);
        case '6':
            ReadTerminalByte(10000);
            return MakeKey(_PageDown_, s1, s2);
        default:
            break;
    }

    *s1 = 0;
    *s2 = 0;
    return 0;
}

static int map_escape_sequence(char *s1, char *s2) {
    int c = ReadTerminalByte(20000);

    if (c < 0) {
        return MakeKey(_Esc_, s1, s2);
    }

    if (c == '[') {
        return map_csi_sequence(s1, s2);
    }

    if (c == 'O') {
        c = ReadTerminalByte(10000);
        switch (c) {
            case 'P':
                return MakeKey(_F1_, s1, s2);
            case 'Q':
                return MakeKey(_F2_, s1, s2);
            case 'R':
                return MakeKey(_F3_, s1, s2);
            case 'S':
                return MakeKey(_F4_, s1, s2);
            case 'F':
                return MakeKey(_End_, s1, s2);
            case 'H':
                return MakeKey(_Home_, s1, s2);
            default:
                break;
        }
    }

    *s1 = 0;
    *s2 = 0;
    return 0;
}

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
    int c;
    int c2;
    int key;

    InitTerminal();
    RefreshTerminalSize();

    key = ReadTerminalKey(s1, s2);
    if (key >= 0) {
        return key;
    }

    c = ReadTerminalByte(0);
    if (c < 0) {
        *s1 = 0;
        *s2 = 0;
        return 0;
    }

    if (c == 0x1B) {
        return map_escape_sequence(s1, s2);
    }

    *s1 = (char) c;
    if ((c & 0xE0) == 0xC0) {
        c2 = ReadTerminalByte(10000);
        if (c2 >= 0) {
            *s2 = (char) c2;
            return ((unsigned char) *s1 << 8) | (unsigned char) *s2;
        }
    }

    return MapAsciiKey(c, s1, s2);
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
    dos_getdate(DDD);
}

// Установка адреса прерыванмя
//==========================================
void addrint(char Nint, unsigned int a, unsigned int b) {
    (void) Nint;
    (void) a;
    (void) b;
}
