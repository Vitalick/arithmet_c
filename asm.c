#include "dos_compat.h"
#include "asm.h"
#include "conio.h"
#include "keys.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#define CY WIN32_API_CY
#include <windows.h>
#undef CY

int _getch(void);
int _kbhit(void);
#else
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#endif

#ifdef _WIN32
static DWORD original_input_mode;
static int terminal_configured = 0;
static int terminal_raw_enabled = 0;
static int terminal_signals_configured = 0;

static void restore_terminal(void) {
    if (terminal_raw_enabled) {
        SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), original_input_mode);
        terminal_raw_enabled = 0;
    }
}

static BOOL WINAPI handle_console_signal(DWORD control_type) {
    switch (control_type) {
        case CTRL_C_EVENT:
        case CTRL_BREAK_EVENT:
        case CTRL_CLOSE_EVENT:
        case CTRL_LOGOFF_EVENT:
        case CTRL_SHUTDOWN_EVENT:
            restore_terminal();
            RestoreTerminalScreenFromSignal();
            ExitProcess(128 + (control_type == CTRL_C_EVENT ? SIGINT : SIGTERM));
            return TRUE;
        default:
            return FALSE;
    }
}

static void init_terminal_signals(void) {
    if (terminal_signals_configured) {
        return;
    }

    terminal_signals_configured = 1;
    SetConsoleCtrlHandler(handle_console_signal, TRUE);
}

static void init_terminal(void) {
    HANDLE input_handle;
    DWORD raw_mode;

    if (terminal_configured) {
        return;
    }

    terminal_configured = 1;
    init_terminal_signals();

    input_handle = GetStdHandle(STD_INPUT_HANDLE);
    if (input_handle == INVALID_HANDLE_VALUE) {
        return;
    }

    if (!GetConsoleMode(input_handle, &original_input_mode)) {
        return;
    }

    raw_mode = original_input_mode;
    raw_mode &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT);

    if (SetConsoleMode(input_handle, raw_mode)) {
        terminal_raw_enabled = 1;
        atexit(restore_terminal);
    }
}

static int read_byte(int timeout_us) {
    DWORD start_tick = GetTickCount();
    DWORD timeout_ms = timeout_us < 0 ? INFINITE : (DWORD) ((timeout_us + 999) / 1000);

    while (!_kbhit()) {
        if (timeout_ms != INFINITE && (GetTickCount() - start_tick) >= timeout_ms) {
            return -1;
        }
        Sleep(1);
    }

    return _getch() & 0xFF;
}
#else
static struct termios original_terminal;
static int terminal_configured = 0;
static int terminal_raw_enabled = 0;
static int terminal_signals_configured = 0;

static void restore_terminal(void) {
    if (terminal_raw_enabled) {
        tcsetattr(STDIN_FILENO, TCSANOW, &original_terminal);
        terminal_raw_enabled = 0;
    }
}

static void handle_terminal_signal(int signal_number) {
    restore_terminal();
    RestoreTerminalScreenFromSignal();
    _exit(128 + signal_number);
}

static void init_terminal_signals(void) {
    if (terminal_signals_configured) {
        return;
    }

    terminal_signals_configured = 1;
    signal(SIGINT, handle_terminal_signal);
    signal(SIGTERM, handle_terminal_signal);
    signal(SIGQUIT, handle_terminal_signal);
}

static void init_terminal(void) {
    struct termios raw;

    if (terminal_configured) {
        return;
    }

    terminal_configured = 1;
    init_terminal_signals();
    if (!isatty(STDIN_FILENO)) {
        return;
    }

    if (tcgetattr(STDIN_FILENO, &original_terminal) != 0) {
        return;
    }

    raw = original_terminal;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_iflag &= ~(IXON | ICRNL);
    raw.c_oflag &= ~(OPOST);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) == 0) {
        terminal_raw_enabled = 1;
        atexit(restore_terminal);
    }
}

static int read_byte(int timeout_us) {
    unsigned char c;
    fd_set input;
    struct timeval timeout;
    struct timeval *timeout_ptr = NULL;

    FD_ZERO(&input);
    FD_SET(STDIN_FILENO, &input);

    if (timeout_us >= 0) {
        timeout.tv_sec = timeout_us / 1000000;
        timeout.tv_usec = timeout_us % 1000000;
        timeout_ptr = &timeout;
    }

    if (select(STDIN_FILENO + 1, &input, NULL, NULL, timeout_ptr) <= 0) {
        return -1;
    }

    if (read(STDIN_FILENO, &c, 1) != 1) {
        return -1;
    }

    return c;
}
#endif

static int make_key(int code, char *s1, char *s2) {
    *s1 = (char) (code & 0xFF);
    *s2 = (char) ((code >> 8) & 0xFF);

    return code;
}

static int map_ascii_key(int c, char *s1, char *s2) {
    switch (c) {
        case 0x08:
        case 0x7F:
            return make_key(_BackSpace_, s1, s2);
        case '\t':
            return make_key(_Tab_, s1, s2);
        case '\r':
        case '\n':
            return make_key(_Enter_, s1, s2);
        case ' ':
            return make_key(_Space_, s1, s2);
        case '+':
            return make_key(_SEQUAL_, s1, s2);
        case '-':
            return make_key(_MINUS_, s1, s2);
        case '*':
            return make_key(_S8_, s1, s2);
        case '/':
            return make_key(_SLASH_, s1, s2);
        case ':':
            return make_key(_SDOTCOM_, s1, s2);
        case ';':
            return make_key(_DOTCOM_, s1, s2);
        case '\\':
            return make_key(_LINE_, s1, s2);
        case '|':
            return make_key(_SLINE_, s1, s2);
        case '0':
            return make_key(_0_, s1, s2);
        case '1':
            return make_key(_1_, s1, s2);
        case '2':
            return make_key(_2_, s1, s2);
        case '3':
            return make_key(_3_, s1, s2);
        case '4':
            return make_key(_4_, s1, s2);
        case '5':
            return make_key(_5_, s1, s2);
        case '6':
            return make_key(_6_, s1, s2);
        case '7':
            return make_key(_7_, s1, s2);
        case '8':
            return make_key(_8_, s1, s2);
        case '9':
            return make_key(_9_, s1, s2);
        default:
            *s1 = (char) c;
            *s2 = 0;
            return (unsigned char) *s1;
    }
}

#ifdef _WIN32
static int map_windows_extended_key(int c, char *s1, char *s2) {
    switch (c) {
        case 0x47:
            return make_key(_Home_, s1, s2);
        case 0x48:
            return make_key(_Up_, s1, s2);
        case 0x49:
            return make_key(_PageUp_, s1, s2);
        case 0x4B:
            return make_key(_Left_, s1, s2);
        case 0x4D:
            return make_key(_Right_, s1, s2);
        case 0x4F:
            return make_key(_End_, s1, s2);
        case 0x50:
            return make_key(_Down_, s1, s2);
        case 0x51:
            return make_key(_PageDown_, s1, s2);
        case 0x52:
            return make_key(_Insert_, s1, s2);
        case 0x53:
            return make_key(_Delete_, s1, s2);
        case 0x3B:
            return make_key(_F1_, s1, s2);
        case 0x3C:
            return make_key(_F2_, s1, s2);
        case 0x3D:
            return make_key(_F3_, s1, s2);
        case 0x3E:
            return make_key(_F4_, s1, s2);
        case 0x3F:
            return make_key(_F5_, s1, s2);
        case 0x40:
            return make_key(_F6_, s1, s2);
        case 0x41:
            return make_key(_F7_, s1, s2);
        case 0x42:
            return make_key(_F8_, s1, s2);
        case 0x43:
            return make_key(_F9_, s1, s2);
        case 0x44:
            return make_key(_F10_, s1, s2);
        default:
            *s1 = 0;
            *s2 = 0;
            return 0;
    }
}
#endif

static int map_csi_sequence(char *s1, char *s2) {
    int c = read_byte(10000);

    switch (c) {
        case 'A':
            return make_key(_Up_, s1, s2);
        case 'B':
            return make_key(_Down_, s1, s2);
        case 'C':
            return make_key(_Right_, s1, s2);
        case 'D':
            return make_key(_Left_, s1, s2);
        case 'F':
            return make_key(_End_, s1, s2);
        case 'H':
            return make_key(_Home_, s1, s2);
        case '1':
            c = read_byte(10000);
            if (c == '~') {
                return make_key(_Home_, s1, s2);
            }
            if (c == '5') {
                read_byte(10000);
                return make_key(_F5_, s1, s2);
            }
            if (c == '7') {
                read_byte(10000);
                return make_key(_F6_, s1, s2);
            }
            if (c == '8') {
                read_byte(10000);
                return make_key(_F7_, s1, s2);
            }
            if (c == '9') {
                read_byte(10000);
                return make_key(_F8_, s1, s2);
            }
            break;
        case '2':
            c = read_byte(10000);
            if (c == '1') {
                read_byte(10000);
                return make_key(_F10_, s1, s2);
            }
            break;
        case '4':
            read_byte(10000);
            return make_key(_End_, s1, s2);
        case '5':
            read_byte(10000);
            return make_key(_PageUp_, s1, s2);
        case '6':
            read_byte(10000);
            return make_key(_PageDown_, s1, s2);
        default:
            break;
    }

    *s1 = 0;
    *s2 = 0;
    return 0;
}

static int map_escape_sequence(char *s1, char *s2) {
    int c = read_byte(20000);

    if (c < 0) {
        return make_key(_Esc_, s1, s2);
    }

    if (c == '[') {
        return map_csi_sequence(s1, s2);
    }

    if (c == 'O') {
        c = read_byte(10000);
        switch (c) {
            case 'P':
                return make_key(_F1_, s1, s2);
            case 'Q':
                return make_key(_F2_, s1, s2);
            case 'R':
                return make_key(_F3_, s1, s2);
            case 'S':
                return make_key(_F4_, s1, s2);
            case 'F':
                return make_key(_End_, s1, s2);
            case 'H':
                return make_key(_Home_, s1, s2);
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

    init_terminal();
    RefreshTerminalSize();

    c = read_byte(0);
    if (c < 0) {
        *s1 = 0;
        *s2 = 0;
        return 0;
    }

#ifdef _WIN32
    if (c == 0 || c == 0xE0) {
        c2 = read_byte(10000);
        if (c2 >= 0) {
            return map_windows_extended_key(c2, s1, s2);
        }
        *s1 = 0;
        *s2 = 0;
        return 0;
    }
#endif

    if (c == 0x1B) {
        return map_escape_sequence(s1, s2);
    }

    *s1 = (char) c;
    if ((c & 0xE0) == 0xC0) {
        c2 = read_byte(10000);
        if (c2 >= 0) {
            *s2 = (char) c2;
            return ((unsigned char) *s1 << 8) | (unsigned char) *s2;
        }
    }

    return map_ascii_key(c, s1, s2);
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
