#include "conio.h"
#include "terminal_platform.h"

#include <signal.h>
#include <stdlib.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>

static struct termios original_terminal;
static int terminal_configured = 0;
static int terminal_raw_enabled = 0;
static int terminal_signals_configured = 0;

/* restore_terminal возвращает stdin из raw-режима в настройки, которые были до запуска программы. */
static void restore_terminal(void) {
    if (terminal_raw_enabled) {
        tcsetattr(STDIN_FILENO, TCSANOW, &original_terminal);
        terminal_raw_enabled = 0;
    }
}

/* handle_terminal_signal восстанавливает терминал перед аварийным завершением по сигналу. */
static void handle_terminal_signal(int signal_number) {
    restore_terminal();
    RestoreTerminalScreenFromSignal();
    _exit(128 + signal_number);
}

/* init_terminal_signals ставит обработчики только один раз, чтобы повторный InitTerminal был безопасным. */
static void init_terminal_signals(void) {
    if (terminal_signals_configured) {
        return;
    }

    terminal_signals_configured = 1;
    signal(SIGINT, handle_terminal_signal);
    signal(SIGTERM, handle_terminal_signal);
    signal(SIGQUIT, handle_terminal_signal);
}

/* InitTerminal переводит Unix-терминал в неблокирующий raw-режим для посимвольного чтения. */
void InitTerminal(void) {
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

/* ReadTerminalByte читает один байт stdin через select с микросекундным таймаутом. */
int ReadTerminalByte(int timeout_us) {
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

/* ReadTerminalKey на Unix не используется: расширенные клавиши приходят как escape-последовательности. */
int ReadTerminalKey(char *s1, char *s2) {
    (void) s1;
    (void) s2;

    return -1;
}
