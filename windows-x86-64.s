.intel_syntax noprefix
.text

# rcx - новый стек
# rdx - указатель на указатель на старый стек
#  r8 - функция, которую будем вызывать
.global Warp
Warp:
        push rbp       # Сохраняем базу стека
        mov [rdx], rsp # Сохраняем старый стек
        and rcx, -16   # Выравниваем стек
        mov rbp, rcx   # Меняем базу стека на новый
        mov rsp, rcx   # Меняем стек на новый
        sub rsp, 32    # Приколы виндуса
        call r8        # Вызываем функцию

# rcx - старый стек
.global Recall
Recall:
        mov rsp, rcx   # Меняем стек на старый
        pop rbp        # На вершине сидит база стека
        ret

# rcx - Новый/старый (смотря как смотреть) стек
# rdx - Указатель на указатель на нынешний стек
.global Reenter
Reenter:
        push rbp       # Пушим на старый стек
        mov [rdx], rsp # Сохраняем старый стек
        mov rsp, rcx   # Меняем стек на новый
        pop rbp        # Восстанавливаем базу стека
        ret
