#ifndef ARITHMET_TERMINAL_PLATFORM_H
#define ARITHMET_TERMINAL_PLATFORM_H

#include <signal.h>
#include <stddef.h>

/* Инициализирует платформенный ввод терминала и переводит его в неблокирующий raw-режим. */
void InitTerminal(void);

/* Читает один байт терминального ввода; timeout_us < 0 означает ожидание без таймаута. */
int ReadTerminalByte(int timeout_us);

/* Читает платформенное событие клавиши, если платформа умеет отдавать его без escape-парсинга. */
int ReadTerminalKey(char *s1, char *s2);

/* Настраивает вывод терминала: кодировку, альтернативный экран и поддержку ANSI-последовательностей. */
void ConfigureTerminalConsole(void);

/* Возвращает параметры вывода терминала в состояние до запуска программы. */
void RestoreTerminalConsole(void);

/* Пишет escape-последовательность способом, допустимым из обработчика сигнала/console control event. */
void WriteTerminalSignalSequence(const char *sequence, size_t length);

/* Настраивает обработчики завершения и изменения размера экрана для платформенного вывода. */
void InitTerminalScreenSignals(volatile sig_atomic_t *terminal_size_changed);

/* Обновляет глобальные TerminalCols/TerminalRows по фактическому размеру терминала. */
void UpdateTerminalSize(void);

/* Проверяет resize на платформах без SIGWINCH; возвращает 1, если размер изменился. */
int PollTerminalResize(void);

#endif
