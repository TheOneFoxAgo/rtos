.intel_syntax noprefix
.text

# rdi - новый стек
# rsi - указатель на указатель на старый стек
# rdx - функция, которую будем вызывать
.global Warp
Warp:
        push rbp       # Сохраняем базу стека
        push rbx
        push r12
        push r13
        push r14
        push r15

        mov [rsi], rsp # Сохраняем старый стек
        and rdi, -16   # Выравниваем стек
        mov rbp, rdi   # Меняем базу стека на новый
        mov rsp, rdi   # Меняем стек на новый
        jmp rdx        # Вызываем функцию

# rdi - старый стек
.global Recall
Recall:
        mov rsp, rdi   # Меняем стек на старый

        pop r15
        pop r14
        pop r13
        pop r12
        pop rbx
        pop rbp        # На вершине сидит база стека
        ret

# rdi - Новый/старый (смотря как смотреть) стек
# rsi - Указатель на указатель на нынешний стек
.global Reenter
Reenter:
        push rbp       # Пушим на старый стек
        push rbx
        push r12
        push r13
        push r14
        push r15

        mov [rsi], rsp # Сохраняем старый стек
        mov rsp, rdi   # Меняем стек на новый

        pop r15
        pop r14
        pop r13
        pop r12
        pop rbx
        pop rbp        # Восстанавливаем базу стека
        ret

