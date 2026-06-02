/*
 * conio.c
 *
 * Реализует DOS-подобные операции текстового вывода поверх логического видеобуфера.
 */

#include "conio.h"
#include "asm.h"
#include "conio_internal.h"

#include <stdio.h>

/* Npage переключает логическую видеостраницу, как это делала DOS-видеопамять. */
void Npage(int np, int ind) {
    NP = np;
    if (ind == 1) {
        NPV = np;
    }
}

/* TextColor меняет младшую половину ATTRIBUTE и сразу применяет цвет к терминалу. */
void TextColor(int newcolor) {
    ATTRIBUTE &= 0xF0;
    ATTRIBUTE = ATTRIBUTE | (CHR(newcolor) & 0xF);
    apply_terminal_attribute();
    flush_terminal_output();
}

/* TextBackGround меняет фоновую половину ATTRIBUTE; ANSI-вывод пока использует только цвет текста. */
void TextBackGround(int newcolor) {
    ATTRIBUTE &= 0xF;
    ATTRIBUTE = ATTRIBUTE | ((CHR(newcolor) & 0x7) << 4);
    apply_terminal_attribute();
    flush_terminal_output();
}

/* ClrScr очищает текущее окно в видеобуфере и весь реальный терминальный экран. */
void ClrScr(void) {
    unsigned int i, k = NP * 4096;
    unsigned int x = 1, y = 1;

    for (i = 0; i < 4000; i += 2) {
        if ((x >= LC) && (x <= RC) && (y >= TC) && (y <= DC)) {
            Screen[k + i + 1] = ATTRIBUTE;
            Screen[k + i] = 0x20;
        }
        if (x < 80)
            x++;
        else {
            x = 1;
            y++;
        }
    }
    GotoXY(1, 1);
    printf("\033[0m");
    printf("\033[2J");
    move_terminal_cursor();
    apply_terminal_attribute();
    flush_terminal_output();
}

/* GotoXY переводит координаты окна в абсолютные координаты логического экрана. */
void GotoXY(int x, int y) {
    CX = LC + x - 2;
    CY = TC + y - 2;
    move_terminal_cursor();
    flush_terminal_output();
}

/* Cprint печатает строку с учетом окна вывода, UTF-8 continuation bytes и логического видеобуфера. */
void Cprint(const char *String) {
    int i = 0, j = NP * 4096 + CY * 160 + CX * 2;
    int k;
    int utf8_visible = 0;

    move_terminal_cursor();
    apply_terminal_attribute();
    while (String[i] != 0x0) {
        unsigned char ch = (unsigned char) String[i];
        int is_continuation = (ch & 0xC0) == 0x80;

        if (((CX + 1) >= LC) && ((CX + 1) <= RC) && ((CY + 1) >= TC) && ((CY + 1) <= DC)) {
            if (ch >= 0x20) {
                Screen[j + 1] = ATTRIBUTE;
                Screen[j] = String[i];
                if (terminal_cell_visible() || (is_continuation && utf8_visible)) {
                    fputc(String[i], stdout);
                }
                if (!is_continuation) {
                    utf8_visible = terminal_cell_visible();
                    if (CX < 80)
                        CX++;
                    else {
                        if (CY < 25) {
                            CX = 0;
                            CY++;
                            if (terminal_cell_visible()) {
                                fputc('\n', stdout);
                            }
                        } else
                            break;
                    }
                }
            } else
                switch (String[i]) {
                    case '\r': {
                        CX = LC - 1;
                        j = NP * 4096 + (CY + TC - 1) * 160 + (CX + LC - 1) * 2;
                        if (terminal_cell_visible()) {
                            fputc('\r', stdout);
                        }
                    }
                    break;
                    case '\n': {
                        if ((CY + 1) >= DC) {
                            CY--;
                        }
                        CY++;
                        j = NP * 4096 + (CY + TC - 1) * 160 + (CX + LC - 1) * 2;
                        if (terminal_cell_visible()) {
                            fputc('\n', stdout);
                        }
                    }
                    break;
                    case '\t': {
                        k = 0;
                        while (k < 8) {
                            if ((CX >= LC) && (CX <= RC) && (CY >= TC) && (CY <= DC))
                                k++;
                            CX++;
                            if (terminal_cell_visible()) {
                                fputc(' ', stdout);
                            }
                            if (CX < 80)
                                CX++;
                            else {
                                if (CY < 25) {
                                    CX = 0;
                                    CY++;
                                    if (terminal_cell_visible()) {
                                        fputc('\n', stdout);
                                    }
                                } else
                                    break;
                            }
                        }
                    }
                    break;
                }
            i++;
        } else {
            if (CX < 80)
                CX++;
            else {
                if (CY < 25) {
                    CX = 0;
                    CY++;
                    if (terminal_cell_visible()) {
                        fputc('\n', stdout);
                    }
                } else
                    break;
            }
        }
        j += 2;
    }
    flush_terminal_output();
}

/* CprintXY объединяет позиционирование и печать строки в одну DOS-подобную операцию. */
void CprintXY(int x, int y, const char *String) {
    GotoXY(x, y);
    Cprint(String);
}

/* GetCharXY записывает один символ в буфер и терминал без продвижения курсора. */
void GetCharXY(int x, int y, char s) {
    int j;

    GotoXY(x, y);
    j = NP * 4096 + CY * 160 + CX * 2;
    Screen[j + 1] = ATTRIBUTE;
    Screen[j] = s;
    apply_terminal_attribute();
    if (terminal_cell_visible()) {
        fputc(s, stdout);
    }
    flush_terminal_output();
}

/* GetChar записывает один символ в текущую ячейку логического экрана. */
void GetChar(char s) {
    int j = NP * 4096 + CY * 160 + CX * 2;

    Screen[j + 1] = ATTRIBUTE;
    Screen[j] = s;
    apply_terminal_attribute();
    if (terminal_cell_visible()) {
        fputc(s, stdout);
    }
    flush_terminal_output();
}

/* Window задает область вывода, внутри которой работают координаты 1..width и 1..height. */
void Window(int left, int top, int right, int down) {
    LC = left;
    TC = top;
    RC = right;
    DC = down;
    lenscr = RC - LC + 1;
    heiscr = DC - TC + 1;
    GotoXY(1, 1);
}

/* WhereX возвращает позицию курсора относительно текущего окна вывода. */
int WhereX(void) {
    return (CX - LC + 2);
}

/* WhereY возвращает позицию курсора относительно текущего окна вывода. */
int WhereY(void) {
    return (CY - TC + 2);
}
