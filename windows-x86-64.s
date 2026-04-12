.intel_syntax noprefix
.text

# rcx - новый стек
# rdx - указатель на указатель на старый стек
#  r8 - функция, которую будем вызывать
.global Warp
Warp:
        push rbp       # Сохраняем базу стека
        push rsi
        push rdi
        push rbx
        push r12
        push r13
        push r14
        push r15

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

        
        pop r15
        pop r14
        pop r13
        pop r12
        pop rbx
        pop rdi
        pop rsi
        pop rbp        # На вершине сидит база стека
        ret

# rcx - Новый/старый (смотря как смотреть) стек
# rdx - Указатель на указатель на нынешний стек
.global Reenter
Reenter:
        push rbp       # Пушим на старый стек
        push rsi
        push rdi
        push rbx
        push r12
        push r13
        push r14
        push r15

        mov [rdx], rsp # Сохраняем старый стек
        mov rsp, rcx   # Меняем стек на новый

        pop r15
        pop r14
        pop r13
        pop r12
        pop rbx
        pop rdi
        pop rsi
        pop rbp        # Восстанавливаем базу стека
        ret
