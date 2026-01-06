.globl _sanitizeInAsm

_sanitizeInAsm:
    pushl %ebp
    movl %esp, %ebp


    movl 8(%ebp), %ebx # ebx = array pointer
    movl 12(%ebp), %ecx # ecx = count

    testl %ecx, %ecx
    jle .done_sanitize # if count <= 0, nothing to sanitize, so end

.sanitize_loop:
    movl (%ebx), %edx # edx = array[i] (current element)
    cmpl $0, %edx

    jge .skip_store_zero # if >=0, skip

    movl $0, (%ebx) # else (it was negative) array[i] = 0

.skip_store_zero:
    addl $4, %ebx # next element
    decl %ecx # count--

    jg .sanitize_loop

.done_sanitize:
    popl %ebp
    ret
