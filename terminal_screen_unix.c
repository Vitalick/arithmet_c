#include "conio.h"
#include "terminal_platform.h"

#include <signal.h>
#include <sys/ioctl.h>
#include <unistd.h>

static int terminal_screen_signals_initialized = 0;
static volatile sig_atomic_t *terminal_size_changed_ptr = NULL;

/* ConfigureTerminalConsole на Unix не требует отдельной настройки вывода. */
void ConfigureTerminalConsole(void) {
}

/* RestoreTerminalConsole на Unix не хранит дополнительных настроек вывода. */
void RestoreTerminalConsole(void) {
}

/* WriteTerminalSignalSequence пишет ANSI-последовательность через async-signal-safe write. */
void WriteTerminalSignalSequence(const char *sequence, size_t length) {
    write(STDOUT_FILENO, sequence, length);
}

/* handle_terminal_screen_signal восстанавливает экран при завершении сигналом. */
static void handle_terminal_screen_signal(int signal_number) {
    RestoreTerminalScreenFromSignal();
    _exit(128 + signal_number);
}

/* handle_terminal_resize_signal сообщает conio, что логический экран надо перерисовать. */
static void handle_terminal_resize_signal(int signal_number) {
    (void) signal_number;
    if (terminal_size_changed_ptr != NULL) {
        *terminal_size_changed_ptr = 1;
    }
}

/* InitTerminalScreenSignals ставит обработчики завершения и SIGWINCH для Unix-терминала. */
void InitTerminalScreenSignals(volatile sig_atomic_t *terminal_size_changed) {
    if (terminal_screen_signals_initialized) {
        return;
    }

    terminal_size_changed_ptr = terminal_size_changed;
    terminal_screen_signals_initialized = 1;
    signal(SIGINT, handle_terminal_screen_signal);
    signal(SIGTERM, handle_terminal_screen_signal);
    signal(SIGQUIT, handle_terminal_screen_signal);
    signal(SIGWINCH, handle_terminal_resize_signal);
}

/* UpdateTerminalSize читает размер окна stdout через ioctl(TIOCGWINSZ). */
void UpdateTerminalSize(void) {
    struct winsize size;

    if ((ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) == 0) && (size.ws_col > 0) && (size.ws_row > 0)) {
        TerminalCols = size.ws_col;
        TerminalRows = size.ws_row;
    }
}

/* PollTerminalResize на Unix не нужен, потому что resize приходит через SIGWINCH. */
int PollTerminalResize(void) {
    return 0;
}
