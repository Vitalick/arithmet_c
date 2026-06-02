#include "conio.h"
#include "terminal_platform.h"

#include <signal.h>

#define CY WIN32_API_CY
#include <windows.h>
#undef CY

static DWORD original_output_mode;
static UINT original_output_code_page;
static UINT original_input_code_page;
static int terminal_console_configured = 0;
static int terminal_screen_signals_initialized = 0;

void ConfigureTerminalConsole(void) {
    HANDLE output_handle;
    DWORD output_mode;

    if (terminal_console_configured) {
        return;
    }

    terminal_console_configured = 1;
    original_output_code_page = GetConsoleOutputCP();
    original_input_code_page = GetConsoleCP();
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    output_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (output_handle == INVALID_HANDLE_VALUE) {
        return;
    }

    if (GetConsoleMode(output_handle, &output_mode)) {
        original_output_mode = output_mode;
        SetConsoleMode(output_handle, output_mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
}

void RestoreTerminalConsole(void) {
    HANDLE output_handle;

    if (!terminal_console_configured) {
        return;
    }

    output_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (output_handle != INVALID_HANDLE_VALUE) {
        SetConsoleMode(output_handle, original_output_mode);
    }
    SetConsoleOutputCP(original_output_code_page);
    SetConsoleCP(original_input_code_page);
    terminal_console_configured = 0;
}

void WriteTerminalSignalSequence(const char *sequence, size_t length) {
    DWORD written;
    HANDLE output_handle = GetStdHandle(STD_OUTPUT_HANDLE);

    if (output_handle != INVALID_HANDLE_VALUE) {
        WriteConsoleA(output_handle, sequence, (DWORD) length, &written, NULL);
    }
}

static BOOL WINAPI handle_terminal_screen_control(DWORD control_type) {
    switch (control_type) {
        case CTRL_C_EVENT:
        case CTRL_BREAK_EVENT:
        case CTRL_CLOSE_EVENT:
        case CTRL_LOGOFF_EVENT:
        case CTRL_SHUTDOWN_EVENT:
            RestoreTerminalScreenFromSignal();
            ExitProcess(128 + (control_type == CTRL_C_EVENT ? SIGINT : SIGTERM));
            return TRUE;
        default:
            return FALSE;
    }
}

void InitTerminalScreenSignals(volatile sig_atomic_t *terminal_size_changed) {
    (void) terminal_size_changed;

    if (terminal_screen_signals_initialized) {
        return;
    }

    terminal_screen_signals_initialized = 1;
    SetConsoleCtrlHandler(handle_terminal_screen_control, TRUE);
}

void UpdateTerminalSize(void) {
    CONSOLE_SCREEN_BUFFER_INFO info;
    HANDLE output_handle = GetStdHandle(STD_OUTPUT_HANDLE);

    if ((output_handle != INVALID_HANDLE_VALUE) && GetConsoleScreenBufferInfo(output_handle, &info)) {
        TerminalCols = info.srWindow.Right - info.srWindow.Left + 1;
        TerminalRows = info.srWindow.Bottom - info.srWindow.Top + 1;
    }
}

int PollTerminalResize(void) {
    int old_cols = TerminalCols;
    int old_rows = TerminalRows;

    UpdateTerminalSize();
    return (old_cols != TerminalCols) || (old_rows != TerminalRows);
}
