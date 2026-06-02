/*
 * conio_state.c
 *
 * Хранит состояние DOS-подобного текстового экрана: видеобуфер, окно вывода, курсор и размеры терминала.
 */

#include "conio.h"
#include "conio_internal.h"

/* ScreenBuffer хранит до восьми DOS-видеостраниц по 4096 байт каждая. */
char ScreenBuffer[4096 * 8];

/* Screen оставлен как публичный указатель для старого кода, который ожидает адрес видеобуфера. */
char *Screen = ScreenBuffer;

/* NP/NPV повторяют модель текущей и видимой видеостраницы Turbo C. */
int NP = 0, NPV = 0;

/* ATTRIBUTE хранит текущую пару цвет текста/цвет фона в DOS-формате. */
char ATTRIBUTE = 0xf;

/* CX/CY - абсолютные координаты курсора логического экрана 80x25. */
unsigned int CX = 0, CY = 0;

/* LC/TC/RC/DC задают активное окно вывода в координатах с единицы. */
int LC = 1, TC = 1, RC = 80, DC = 25;

/* lenscr/heiscr кешируют размер текущего окна для совместимости со старым conio. */
int lenscr = 80, heiscr = 25;

/* TerminalCols/TerminalRows отражают реальное окно терминала. */
int TerminalCols = LOGICAL_SCREEN_WIDTH, TerminalRows = LOGICAL_SCREEN_HEIGHT;

/* ScreenOffsetX/ScreenOffsetY центрируют логический экран в большом терминале. */
int ScreenOffsetX = 0, ScreenOffsetY = 0;
