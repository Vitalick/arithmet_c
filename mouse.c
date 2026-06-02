#include "mouse.h"

int FlagViewCursorMouse = 1;

// Инициализация мыши
//==========================================
int Initialize(int *keys) {
    if (keys != 0) {
        *keys = 0;
    }

    return 0;
}

// Спрятать/показать курсор мыши
//==========================================
void Cursor(int flag) {
    FlagViewCursorMouse = flag == 0 ? 1 : 0;
}

// Получение координаты и состяние клавиш мыши
//==========================================
int GetPos(int *x, int *y) {
    if (x != 0) {
        *x = 0;
    }
    if (y != 0) {
        *y = 0;
    }

    return 0;
}

// Позиционирование курсора мыши
//==========================================
void SetPos(int x, int y) {
    (void) x;
    (void) y;
}

// Установка границ перемещения курсора
//==========================================
void SetBorder(int xs, int xe, int ys, int ye) {
    (void) xs;
    (void) xe;
    (void) ys;
    (void) ye;
}

// Установка текстового курсора
//==========================================
void SetTextCursor(int type, int beginline, int endline) {
    (void) type;
    (void) beginline;
    (void) endline;
}
