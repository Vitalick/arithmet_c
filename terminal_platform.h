#ifndef ARITHMET_TERMINAL_PLATFORM_H
#define ARITHMET_TERMINAL_PLATFORM_H

#include <signal.h>
#include <stddef.h>

void InitTerminal(void);
int ReadTerminalByte(int timeout_us);
int ReadTerminalKey(char *s1, char *s2);

void ConfigureTerminalConsole(void);
void RestoreTerminalConsole(void);
void WriteTerminalSignalSequence(const char *sequence, size_t length);
void InitTerminalScreenSignals(volatile sig_atomic_t *terminal_size_changed);
void UpdateTerminalSize(void);
int PollTerminalResize(void);

int MakeKey(int code, char *s1, char *s2);
int MapAsciiKey(int c, char *s1, char *s2);

#endif
