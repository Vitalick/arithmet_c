#include "conio.h"
#include "asm.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define LOGICAL_SCREEN_WIDTH 80
#define LOGICAL_SCREEN_HEIGHT 25

static char ScreenBuffer[4096 * 8];
char *Screen = ScreenBuffer;
int NP = 0, NPV = 0;
char ATTRIBUTE = 0xf;
unsigned int CX = 0, CY = 0;
int LC = 1, TC = 1, RC = 80, DC = 25;
int lenscr = 80, heiscr = 25;
int TerminalCols = LOGICAL_SCREEN_WIDTH, TerminalRows = LOGICAL_SCREEN_HEIGHT;
int ScreenOffsetX = 0, ScreenOffsetY = 0;

static int cursor_visible = 1;
static int terminal_screen_initialized = 0;
static int terminal_screen_signals_initialized = 0;
static volatile sig_atomic_t terminal_size_changed = 1;
static void (*terminal_resize_handler)(void) = NULL;

static void apply_attribute(void);
static void move_terminal_cursor(void);

static int terminal_cell_visible(void) {
    unsigned int x = ScreenOffsetX + CX + 1;
    unsigned int y = ScreenOffsetY + CY + 1;

    return (x >= 1) && (x <= (unsigned int) TerminalCols) &&
           (y >= 1) && (y <= (unsigned int) TerminalRows);
}

static void update_terminal_size(void) {
    struct winsize size;

    if ((ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) == 0) && (size.ws_col > 0) && (size.ws_row > 0)) {
        TerminalCols = size.ws_col;
        TerminalRows = size.ws_row;
    }

    ScreenOffsetX = (TerminalCols > LOGICAL_SCREEN_WIDTH) ? ((TerminalCols - LOGICAL_SCREEN_WIDTH) / 2) : 0;
    ScreenOffsetY = (TerminalRows > LOGICAL_SCREEN_HEIGHT) ? ((TerminalRows - LOGICAL_SCREEN_HEIGHT) / 2) : 0;
}

void RestoreTerminalScreen(void) {
    printf("\033[0m\033[?25h\033[?1049l");
    fflush(stdout);
}

void RestoreTerminalScreenFromSignal(void) {
    static const char sequence[] = "\033[0m\033[?25h\033[?1049l";

    write(STDOUT_FILENO, sequence, sizeof(sequence) - 1);
}

static void handle_terminal_screen_signal(int signal_number) {
    RestoreTerminalScreenFromSignal();
    _exit(128 + signal_number);
}

static void handle_terminal_resize_signal(int signal_number) {
    (void) signal_number;
    terminal_size_changed = 1;
}

static void init_terminal_screen_signals(void) {
    if (terminal_screen_signals_initialized) {
        return;
    }

    terminal_screen_signals_initialized = 1;
    signal(SIGINT, handle_terminal_screen_signal);
    signal(SIGTERM, handle_terminal_screen_signal);
    signal(SIGQUIT, handle_terminal_screen_signal);
    signal(SIGWINCH, handle_terminal_resize_signal);
}

static void init_terminal_screen(void) {
    if (terminal_screen_initialized) {
        return;
    }

    terminal_screen_initialized = 1;
    init_terminal_screen_signals();
    update_terminal_size();
    terminal_size_changed = 0;
    printf("\033[?1049h");
    printf("\033[2J");
    printf("\033[H");
    atexit(RestoreTerminalScreen);
    fflush(stdout);
}

static int ansi_color(int color) {
    static const int colors[16] = {
        30, 34, 32, 36, 31, 35, 33, 37,
        90, 94, 92, 96, 91, 95, 93, 97
    };

    return colors[color & 0x0F];
}

int RefreshTerminalSize(void) {
    init_terminal_screen();
    if (!terminal_size_changed) {
        return 0;
    }

    terminal_size_changed = 0;
    update_terminal_size();
    printf("\033[0m");
    printf("\033[2J");
    printf("\033[H");
    if (terminal_resize_handler != NULL) {
        terminal_resize_handler();
    } else {
        apply_attribute();
        move_terminal_cursor();
    }
    fflush(stdout);
    return 1;
}

void SetTerminalResizeHandler(void (*handler)(void)) {
    terminal_resize_handler = handler;
}

static void apply_attribute(void) {
    int fg = ATTRIBUTE & 0x0F;

    init_terminal_screen();
    printf("\033[%dm", ansi_color(fg));
}

static void move_terminal_cursor(void) {
    unsigned int x;
    unsigned int y;

    init_terminal_screen();
    x = ScreenOffsetX + CX + 1;
    y = ScreenOffsetY + CY + 1;

    if (x < 1) {
        x = 1;
    }
    if (y < 1) {
        y = 1;
    }
    if (x > (unsigned int) TerminalCols) {
        x = TerminalCols;
    }
    if (y > (unsigned int) TerminalRows) {
        y = TerminalRows;
    }

    printf("\033[%u;%uH", y, x);
}

static void flush_terminal(void) {
    fflush(stdout);
}

void _setcursortype(int type) {
    init_terminal_screen();
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
    printf("\033[0m");
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
    int utf8_visible = 0;

    move_terminal_cursor();
    apply_attribute();
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
    if (terminal_cell_visible()) {
        fputc(s, stdout);
    }
    flush_terminal();
}

void GetChar(char s) {
    int j = NP * 4096 + CY * 160 + CX * 2;

    Screen[j + 1] = ATTRIBUTE;
    Screen[j] = s;
    apply_attribute();
    if (terminal_cell_visible()) {
        fputc(s, stdout);
    }
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
