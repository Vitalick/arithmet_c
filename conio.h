#ifndef ARITHMET_CONIO_H
#define ARITHMET_CONIO_H

/* Цвета повторяют палитру Turbo C textmode и используются как числовые атрибуты видеобуфера. */
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

/* Меняет видимость текстового курсора в активном терминале. */
void _setcursortype(int);

/* Восстанавливает экран терминала после штатного завершения программы. */
void RestoreTerminalScreen(void);

/* Восстанавливает экран из обработчика сигнала, где нельзя использовать обычный printf. */
void RestoreTerminalScreenFromSignal(void);

/* Проверяет изменение размера терминала и перерисовывает активный логический экран. */
int RefreshTerminalSize(void);

/* Регистрирует callback, который знает, какой экран нужно нарисовать после resize. */
void SetTerminalResizeHandler(void (*handler)(void));

/* Выбирает DOS-видеостраницу; ind=1 делает страницу также активной для вывода. */
void Npage(int, int);

/* Устанавливает цвет символов в текущем текстовом атрибуте. */
void TextColor(int);

/* Устанавливает цвет фона в текущем текстовом атрибуте. */
void TextBackGround(int);

/* Очищает текущее окно вывода на логическом экране 80x25. */
void ClrScr(void);

/* Перемещает курсор внутри текущего окна вывода, а не всего терминала. */
void GotoXY(int, int);

/* Печатает строку в текущей позиции, обновляя и видеобуфер, и реальный терминал. */
void Cprint(const char *);

/* Перемещает курсор и печатает строку в указанной позиции текущего окна. */
void CprintXY(int, int, const char *);

/* Печатает один символ в указанной позиции текущего окна. */
void GetCharXY(int, int, char);

/* Печатает один символ в текущей позиции курсора. */
void GetChar(char);

/* Задает окно вывода, относительно которого работают GotoXY и WhereX/WhereY. */
void Window(int, int, int, int);

/* Возвращает X-координату курсора внутри текущего окна. */
int WhereX(void);

/* Возвращает Y-координату курсора внутри текущего окна. */
int WhereY(void);

/* Screen указывает на логический текстовый видеобуфер, совместимый с DOS-адресацией символ/атрибут. */
extern char *Screen;

/* NP - текущая видеостраница, NPV - активная видимая видеостраница. */
extern int NP, NPV;

/* ATTRIBUTE хранит текущий DOS-атрибут: младшие 4 бита - текст, старшие - фон. */
extern char ATTRIBUTE;

/* CX/CY - абсолютная позиция курсора в логическом экране 80x25, отсчет с нуля. */
extern unsigned int CX, CY;

/* LC/TC/RC/DC задают границы текущего окна вывода в DOS-координатах, отсчет с единицы. */
extern int LC, TC, RC, DC;

/* lenscr/heiscr хранят размер текущего окна вывода. */
extern int lenscr, heiscr;

/* TerminalCols/TerminalRows - фактический размер окна терминала. */
extern int TerminalCols, TerminalRows;

/* ScreenOffsetX/ScreenOffsetY центрируют логический экран 80x25 внутри большего терминала. */
extern int ScreenOffsetX, ScreenOffsetY;

#endif
