#ifndef ARITHMET_CONIO_H
#define ARITHMET_CONIO_H

#include "dos_compat.h"

#define BLACK 0
#define BLUE 1
#define GREEN 2
#define CYAN 3
#define RED 4
#define MAGENTA 5
#define BROWN 6
#define LIGHTGRAY 7
#define DARKGRAY 8
#define LIGHTBLUE 9
#define LIGHTGREEN 10
#define LIGHTCYAN 11
#define LIGHTRED 12
#define LIGHTMAGENTA 13
#define YELLOW 14
#define WHITE 15

#define _NOCURSOR 0
#define _NORMALCURSOR 1

void _setcursortype(int);
void RestoreTerminalScreen(void);
void RestoreTerminalScreenFromSignal(void);
void Npage(int, int); // Установка активной видеостраницы
//          A   B
// A:  номер видеостраницы
// B:  0 - делать только текущей;  1 - делать текущей и активной
void TextColor(int); // Установка цвета символов
//    параметр:  цвет
void TextBackGround(int); // Установка цыета фона
//    параметр:  цвет
void ClrScr(void); // Очистка экрана
void GotoXY(int, int); // Позиционирование курсора
//           X   Y
// X,Y:  координаты курсора
void Cprint(char *); // Вывод строки в текущую позицию курсора
//    параметр:  адрес строки
void CprintXY(int, int, char *); // Вывод строки в указанную позицию курсора
//             X   Y    S
// X,Y:  координаты вывода
//   S:  адрес строки
void GetCharXY(int, int, char); // Вывод символа в указанную позицию курсора
//              X   Y    C
// X,Y:  координаты вывода
//   C:  символ
void GetChar(char); // Вывод символа в текущую позицию курсора
//    параметр:  символ
void Window(int, int, int, int); // Установка границ окна вывода
//          X0  X1  Y0  Y1
// X0,X1:  границы по горизонтали
// Y0,Y1:  границы по вертикали
int WhereX(void); // Получение координаты X
//    выход:  текущая координата X
int WhereY(void); // Получение координаты Y
//    выход:  текущая координата Y

extern char *Screen; // Буфер экрана
extern int NP, NPV; // номер текущей и активной видеостраницы
extern char ATTRIBUTE; // текущие атрибуты вывода
extern unsigned int CX, CY; // текущие координаты
extern int LC, TC, RC, DC; // границы текущего окна вывода
extern int lenscr, heiscr; // длина и высота текущего окна вывода

#endif
