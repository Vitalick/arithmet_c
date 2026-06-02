int FlagViewCursorMouse = 1;

// Инициализация мыши
//==========================================
int Initialize(int *keys) {
    int xx, yy;

    xx = 0;
    yy = 0;
    asm
    {
        mov ax,
        0000h
    int
        33h
        mov xx, ax
        mov yy, bx
    };
    *keys = yy;
    return (xx);
}

// Спрятать/показать курсор мыши
//==========================================
void Cursor(int flag) {
    if ((flag == 0) && (FlagViewCursorMouse == 0)) {
        asm
        {
            mov ax,
            0002h
      int
            33h
        };
        FlagViewCursorMouse = 1;
    }
    if ((flag == 1) && (FlagViewCursorMouse == 1)) {
        asm
        {
            mov ax,
            0001h
      int
            33h
        };
        FlagViewCursorMouse = 0;
    }
}

// Получение координаты и состяние клавиш мыши
//==========================================
int GetPos(int *x, int *y) {
    int xx, yy, mm;

    xx = 0;
    yy = 0;
    mm = 0;
    asm
    {
        mov ax,
        0003h
    int
        33h
        mov xx, cx
        mov yy, dx
        mov mm, bx
    };
    *x = xx;
    *y = yy;
    return (mm);
}

// Позиционирование курсора мыши
//==========================================
void SetPos(int x, int y) {
    int xx, yy;

    xx = x;
    yy = y;
    asm
    {
        mov ax,
        0004h
        mov cx, xx
        mov dx, yy
    int
        33h
    }
}

// Установка границ перемещения курсора
//==========================================
void SetBorder(int xs, int xe, int ys, int ye) {
    int xxs, xxe, yys, yye;

    xxs = xs;
    xxe = xe;
    yys = ys;
    yye = ye;
    asm
    {
        mov ax,
        0007h
        mov cx, xxs
        mov dx, xxe
    int
        33h
        mov ax,
        0008h
        mov cx, yys
        mov dx, yye
    int
        33h
    }
}

// Установка текстового курсора
//==========================================
void SetTextCursor(int type, int beginline, int endline) {
    int t, begl, endl;

    t = type;
    begl = beginline;
    endl = endline;
    asm
    {
        mov ax,
        000Ah
        mov bx, t
        mov cx, begl
        mov dx, endl
    int
        33h
    }
}
