#include "conio.h"
#include "dos_compat.h"
#include "asm.h"

#include <stdio.h>

static char ScreenBuffer[4096 * 8];
char *Screen = ScreenBuffer;
int NP = 0, NPV = 0;
char ATTRIBUTE = 0xf;
unsigned int CX = 0, CY = 0;
int LC = 1, TC = 1, RC = 80, DC = 25;
int lenscr = 80, heiscr = 25;

static int cursor_visible = 1;

static int ansi_color(int color) {
    static const int colors[16] = {
        30, 34, 32, 36, 31, 35, 33, 37,
        90, 94, 92, 96, 91, 95, 93, 97
    };

    return colors[color & 0x0F];
}

static void apply_attribute(void) {
    int fg = ATTRIBUTE & 0x0F;
    int bg = (ATTRIBUTE >> 4) & 0x07;

    printf("\033[%d;%dm", ansi_color(fg), ansi_color(bg) + 10);
}

static void move_terminal_cursor(void) {
    printf("\033[%u;%uH", CY + 1, CX + 1);
}

static void flush_terminal(void) {
    fflush(stdout);
}

void _setcursortype(int type) {
    cursor_visible = type != _NOCURSOR;
    printf(cursor_visible ? "\033[?25h" : "\033[?25l");
    flush_terminal();
}

void Npage(int np, int ind) {
    NP = np;
    if (ind == 1) {
        NPV = np;
    }
}

void TextColor(int newcolor) {
    ATTRIBUTE &= 0xF0;
    ATTRIBUTE = ATTRIBUTE | (CHR(newcolor) & 0xF);
    apply_attribute();
    flush_terminal();
}

void TextBackGround(int newcolor) {
    ATTRIBUTE &= 0xF;
    ATTRIBUTE = ATTRIBUTE | ((CHR(newcolor) & 0x7) << 4);
    apply_attribute();
    flush_terminal();
}

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
    printf("\033[2J");
    move_terminal_cursor();
    apply_attribute();
    flush_terminal();
}

void GotoXY(int x, int y) {
    CX = LC + x - 2;
    CY = TC + y - 2;
    move_terminal_cursor();
    flush_terminal();
}

void Cprint(char *String) {
    int i = 0, j = NP * 4096 + CY * 160 + CX * 2;
    int k;

    move_terminal_cursor();
    apply_attribute();
    while (String[i] != 0x0) {
        unsigned char ch = (unsigned char) String[i];

        if (((CX + 1) >= LC) && ((CX + 1) <= RC) && ((CY + 1) >= TC) && ((CY + 1) <= DC)) {
            if (ch >= 0x20) {
                Screen[j + 1] = ATTRIBUTE;
                Screen[j] = String[i];
                fputc(String[i], stdout);
                if ((ch & 0xC0) != 0x80) {
                    if (CX < 80)
                        CX++;
                    else {
                        if (CY < 25) {
                            CX = 0;
                            CY++;
                            fputc('\n', stdout);
                        } else
                            break;
                    }
                }
            } else
                switch (String[i]) {
                    case '\r': {
                        CX = LC - 1;
                        j = NP * 4096 + (CY + TC - 1) * 160 + (CX + LC - 1) * 2;
                        fputc('\r', stdout);
                    }
                    break;
                    case '\n': {
                        if ((CY + 1) >= DC) {
                            CY--;
                        }
                        CY++;
                        j = NP * 4096 + (CY + TC - 1) * 160 + (CX + LC - 1) * 2;
                        fputc('\n', stdout);
                    }
                    break;
                    case '\t': {
                        k = 0;
                        while (k < 8) {
                            if ((CX >= LC) && (CX <= RC) && (CY >= TC) && (CY <= DC))
                                k++;
                            CX++;
                            fputc(' ', stdout);
                            if (CX < 80)
                                CX++;
                            else {
                                if (CY < 25) {
                                    CX = 0;
                                    CY++;
                                    fputc('\n', stdout);
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
                    fputc('\n', stdout);
                } else
                    break;
            }
        }
        j += 2;
    }
    flush_terminal();
}

void CprintXY(int x, int y, char *String) {
    GotoXY(x, y);
    Cprint(String);
}

void GetCharXY(int x, int y, char s) {
    int j;

    GotoXY(x, y);
    j = NP * 4096 + CY * 160 + CX * 2;
    Screen[j + 1] = ATTRIBUTE;
    Screen[j] = s;
    apply_attribute();
    fputc(s, stdout);
    flush_terminal();
}

void GetChar(char s) {
    int j = NP * 4096 + CY * 160 + CX * 2;

    Screen[j + 1] = ATTRIBUTE;
    Screen[j] = s;
    apply_attribute();
    fputc(s, stdout);
    flush_terminal();
}

void Window(int left, int top, int right, int down) {
    LC = left;
    TC = top;
    RC = right;
    DC = down;
    lenscr = RC - LC + 1;
    heiscr = DC - TC + 1;
    GotoXY(1, 1);
}

int WhereX(void) {
    return (CX - LC + 2);
}

int WhereY(void) {
    return (CY - TC + 2);
}
