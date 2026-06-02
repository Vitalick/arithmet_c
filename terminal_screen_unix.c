#include "conio.h"
#include "terminal_platform.h"

#include <signal.h>
#include <sys/ioctl.h>
#include <unistd.h>

static int terminal_screen_signals_initialized = 0;
static volatile sig_atomic_t *terminal_size_changed_ptr = NULL;

void ConfigureTerminalConsole(void) {
}

void RestoreTerminalConsole(void) {
}

void WriteTerminalSignalSequence(const char *sequence, size_t length) {
    write(STDOUT_FILENO, sequence, length);
}

static void handle_terminal_screen_signal(int signal_number) {
    RestoreTerminalScreenFromSignal();
    _exit(128 + signal_number);
}

static void handle_terminal_resize_signal(int signal_number) {
    (void) signal_number;
    if (terminal_size_changed_ptr != NULL) {
        *terminal_size_changed_ptr = 1;
    }
}

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

void UpdateTerminalSize(void) {
    struct winsize size;

    if ((ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) == 0) && (size.ws_col > 0) && (size.ws_row > 0)) {
        TerminalCols = size.ws_col;
        TerminalRows = size.ws_row;
    }
}

int PollTerminalResize(void) {
    return 0;
}
