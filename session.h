/*
 * session.h
 *
 * Объявляет запуск сеанса и подготовку структуры результата.
 */

#ifndef ARITHMET_SESSION_H
#define ARITHMET_SESSION_H

/* Запускает один тренировочный сеанс от первого примера до итоговой оценки. */
void run_training_session(void);

/* Очищает session_result и копирует туда настройки, с которыми начинается новый сеанс. */
void reset_session_result(void);

#endif
