/*
 * protocol.h
 *
 * Объявляет сохранение протокола последнего сеанса.
 */

#ifndef ARITHMET_PROTOCOL_H
#define ARITHMET_PROTOCOL_H

/* Сравнивает DOS-время файлов, чтобы выбрать самый старый протокол при переполнении 000-999. */
int is_older_file_time(const struct ftime *new_time, const struct ftime *old_time);

/* Сохраняет session_result в текстовый протокол с DOS-совместимым именем файла. */
void save_session_protocol(void);

#endif
