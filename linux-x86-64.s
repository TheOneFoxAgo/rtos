.intel_syntax noprefix
.text

# rdi - новый стек
# rsi - функция, которую будем вызывать
# rdx - указатель на указатель на старый стек
.global Warp
Warp:
        push rbp       # Сохраняем базу стека
        mov [rsi], rsp # Сохраняем старый стек
        and rdi, -16   # Выравниваем стек
        mov rbp, rdi   # Меняем базу стека на новый
        mov rsp, rdi   # Меняем стек на новый
        jmp rdx        # Вызываем функцию

# rdi - старый стек
.global Recall
Recall:
        mov rsp, rdi   # Меняем стек на старый
        pop rbp        # На вершине сидит база стека
        ret

# rdi - Новый/старый (смотря как смотреть) стек
# rsi - Указатель на указатель на нынешний стек
.global Reenter
Reenter:
        push rbp       # Пушим на старый стек
        mov [rsi], rsp # Сохраняем старый стек
        mov rsp, rdi   # Меняем стек на новый
        pop rbp        # Восстанавливаем базу стека
        ret

