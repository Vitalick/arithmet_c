#include "conio.h"
#include "asm.h"
#include "keys.h"
#include "terminal_platform.h"

#include <signal.h>
#include <stdlib.h>

#define CY WIN32_API_CY
#include <windows.h>
#undef CY

static DWORD original_input_mode;
static int terminal_configured = 0;
static int terminal_raw_enabled = 0;
static int terminal_signals_configured = 0;
static unsigned char input_buffer[8];
static int input_buffer_pos = 0;
static int input_buffer_len = 0;

/* restore_terminal возвращает режим Windows Console Input к состоянию до запуска программы. */
static void restore_terminal(void) {
    if (terminal_raw_enabled) {
        SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), original_input_mode);
        terminal_raw_enabled = 0;
    }
}

/* handle_console_signal восстанавливает терминал перед закрытием консоли или Ctrl+C. */
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

/* init_terminal_signals регистрирует Windows console control handler один раз. */
static void init_terminal_signals(void) {
    if (terminal_signals_configured) {
        return;
    }

    terminal_signals_configured = 1;
    SetConsoleCtrlHandler(handle_console_signal, TRUE);
}

/* InitTerminal отключает line/echo/processed input, чтобы читать клавиши напрямую. */
void InitTerminal(void) {
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
    raw_mode |= ENABLE_EXTENDED_FLAGS;

    if (SetConsoleMode(input_handle, raw_mode)) {
        terminal_raw_enabled = 1;
        atexit(restore_terminal);
    }
}

/* push_input_byte добавляет байт в небольшой буфер UTF-8 символа. */
static void push_input_byte(unsigned char c) {
    if (input_buffer_len < (int) sizeof(input_buffer)) {
        input_buffer[input_buffer_len] = c;
        input_buffer_len++;
    }
}

/* push_utf8_char кодирует Windows WCHAR в UTF-8 байты для общего кода ввода имени. */
static void push_utf8_char(WCHAR ch) {
    if (ch < 0x80) {
        push_input_byte((unsigned char) ch);
    } else if (ch < 0x800) {
        push_input_byte((unsigned char) (0xC0 | (ch >> 6)));
        push_input_byte((unsigned char) (0x80 | (ch & 0x3F)));
    } else {
        push_input_byte((unsigned char) (0xE0 | (ch >> 12)));
        push_input_byte((unsigned char) (0x80 | ((ch >> 6) & 0x3F)));
        push_input_byte((unsigned char) (0x80 | (ch & 0x3F)));
    }
}

/* map_windows_extended_key переводит scan code Windows Console в старую DOS-константу клавиши. */
static int map_windows_extended_key(int c, char *s1, char *s2) {
    switch (c) {
        case 0x47:
            return MakeKey(_Home_, s1, s2);
        case 0x48:
            return MakeKey(_Up_, s1, s2);
        case 0x49:
            return MakeKey(_PageUp_, s1, s2);
        case 0x4B:
            return MakeKey(_Left_, s1, s2);
        case 0x4D:
            return MakeKey(_Right_, s1, s2);
        case 0x4F:
            return MakeKey(_End_, s1, s2);
        case 0x50:
            return MakeKey(_Down_, s1, s2);
        case 0x51:
            return MakeKey(_PageDown_, s1, s2);
        case 0x52:
            return MakeKey(_Insert_, s1, s2);
        case 0x53:
            return MakeKey(_Delete_, s1, s2);
        case 0x3B:
            return MakeKey(_F1_, s1, s2);
        case 0x3C:
            return MakeKey(_F2_, s1, s2);
        case 0x3D:
            return MakeKey(_F3_, s1, s2);
        case 0x3E:
            return MakeKey(_F4_, s1, s2);
        case 0x3F:
            return MakeKey(_F5_, s1, s2);
        case 0x40:
            return MakeKey(_F6_, s1, s2);
        case 0x41:
            return MakeKey(_F7_, s1, s2);
        case 0x42:
            return MakeKey(_F8_, s1, s2);
        case 0x43:
            return MakeKey(_F9_, s1, s2);
        case 0x44:
            return MakeKey(_F10_, s1, s2);
        default:
            *s1 = 0;
            *s2 = 0;
            return 0;
    }
}

/* ReadTerminalByte отдает байты, подготовленные ReadTerminalKey для многобайтового UTF-8 символа. */
int ReadTerminalByte(int timeout_us) {
    (void) timeout_us;

    if (input_buffer_pos < input_buffer_len) {
        return input_buffer[input_buffer_pos++];
    }

    return -1;
}

/* ReadTerminalKey читает INPUT_RECORD и возвращает готовый DOS-код клавиши или UTF-8 байты символа. */
int ReadTerminalKey(char *s1, char *s2) {
    HANDLE input_handle;
    int ascii_key;

    if (input_buffer_pos < input_buffer_len) {
        *s1 = (char) input_buffer[input_buffer_pos++];
        *s2 = 0;
        if (input_buffer_pos < input_buffer_len) {
            *s2 = (char) input_buffer[input_buffer_pos++];
        }
        if (input_buffer_pos >= input_buffer_len) {
            input_buffer_pos = 0;
            input_buffer_len = 0;
        }
        return ((unsigned char) *s1 << 8) | (unsigned char) *s2;
    }

    input_buffer_pos = 0;
    input_buffer_len = 0;

    input_handle = GetStdHandle(STD_INPUT_HANDLE);
    if (input_handle == INVALID_HANDLE_VALUE) {
        return 0;
    }

    while (1) {
        INPUT_RECORD record;
        DWORD events_available;
        DWORD records_read;

        if (!GetNumberOfConsoleInputEvents(input_handle, &events_available) || events_available == 0) {
            return 0;
        }

        if (!ReadConsoleInputW(input_handle, &record, 1, &records_read) || records_read == 0) {
            return 0;
        }

        if (record.EventType == WINDOW_BUFFER_SIZE_EVENT) {
            return 0;
        }

        if (record.EventType != KEY_EVENT || !record.Event.KeyEvent.bKeyDown) {
            continue;
        }

        if (record.Event.KeyEvent.uChar.UnicodeChar == 0) {
            return map_windows_extended_key(record.Event.KeyEvent.wVirtualScanCode, s1, s2);
        }

        if (record.Event.KeyEvent.uChar.UnicodeChar < 0x80) {
            return MapAsciiKey(record.Event.KeyEvent.uChar.UnicodeChar, s1, s2);
        }

        push_utf8_char(record.Event.KeyEvent.uChar.UnicodeChar);
        break;
    }

    *s1 = (char) input_buffer[0];
    *s2 = input_buffer_len > 1 ? (char) input_buffer[1] : 0;
    input_buffer_pos = input_buffer_len > 2 ? 2 : 0;
    if (input_buffer_len <= 2) {
        input_buffer_len = 0;
    }

    if (*s2 != 0) {
        return ((unsigned char) *s1 << 8) | (unsigned char) *s2;
    }

    ascii_key = MapAsciiKey((unsigned char) *s1, s1, s2);
    return ascii_key;
}
