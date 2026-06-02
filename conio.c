#include "conio.h"
#include "dos_compat.h"
#include "asm.h"

static char ScreenBuffer[4096 * 8];
char *Screen = ScreenBuffer;
int NP = 0, NPV = 0;
char ATTRIBUTE = 0xf;
unsigned int CX = 0, CY = 0;
int LC = 1, TC = 1, RC = 80, DC = 25;
int lenscr = 80, heiscr = 25;

void _setcursortype(int type) {
    (void) type;
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
}

void TextBackGround(int newcolor) {
    ATTRIBUTE &= 0xF;
    ATTRIBUTE = ATTRIBUTE | ((CHR(newcolor) & 0x7) << 4);
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
}

void GotoXY(int x, int y) {
    CX = LC + x - 2;
    CY = TC + y - 2;
}

void Cprint(char *String) {
    int i = 0, j = NP * 4096 + CY * 160 + CX * 2;
    int k;
    while (String[i] != 0x0) {
        unsigned char ch = (unsigned char) String[i];

        if (((CX + 1) >= LC) && ((CX + 1) <= RC) && ((CY + 1) >= TC) && ((CY + 1) <= DC)) {
            if (ch >= 0x20) {
                Screen[j + 1] = ATTRIBUTE;
                Screen[j] = String[i];
                if (CX < 80)
                    CX++;
                else {
                    if (CY < 25) {
                        CX = 0;
                        CY++;
                    } else
                        break;
                }
            } else
                switch (String[i]) {
                    case '\r': {
                        CX = LC - 1;
                        j = NP * 4096 + (CY + TC - 1) * 160 + (CX + LC - 1) * 2;
                    }
                    break;
                    case '\n': {
                        if ((CY + 1) >= DC) {
                            CY--;
                        }
                        CY++;
                        j = NP * 4096 + (CY + TC - 1) * 160 + (CX + LC - 1) * 2;
                    }
                    break;
                    case '\t': {
                        k = 0;
                        while (k < 8) {
                            if ((CX >= LC) && (CX <= RC) && (CY >= TC) && (CY <= DC))
                                k++;
                            CX++;
                            if (CX < 80)
                                CX++;
                            else {
                                if (CY < 25) {
                                    CX = 0;
                                    CY++;
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
                } else
                    break;
            }
        }
        j += 2;
    }
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
}

void GetChar(char s) {
    int j = NP * 4096 + CY * 160 + CX * 2;

    Screen[j + 1] = ATTRIBUTE;
    Screen[j] = s;
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
