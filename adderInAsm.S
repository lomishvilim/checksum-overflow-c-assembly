    .globl _adderInAsm
_adderInAsm:

    pushl %ebp
    movl %esp, %ebp
    subl $12, %esp # reserve 12 bytes for local vars: none in fast, sum_lo, sum_hi

    # normal (fast) loop for 32-bit checksum
    movl 8(%ebp), %ebx #ebx = pointer to array (numbers) (first argument)
    movl 12(%ebp), %edx # edx = count (second argument)

    #in the case of overflow in 32-bit sum calculation, we will use 64-bit sum calculation
    #it is split into high and low 32 bits
    #anything causing overflow in 64-bits is hard to reproduce, thus not handled in this code

    movl 16(%ebp), %esi # esi = pointer to sum_lo_out
    movl 20(%ebp), %edi # edi = pointer to sum_hi_out

    xorl %eax, %eax # eax = 0 (sum_lo)
    testl %edx, %edx
    je .store_and_return # if count==0, skip to store & return


.fast_loop:
    addl (%ebx), %eax # sum_lo += *numbers (current element)
    jo .wide_mode # if we catch overflow, switch to wide (64-bit) loop

    addl $4, %ebx # numbers++ (next element)
    decl %edx # count--
    jnz .fast_loop # if count>0, continue

    # no overflow: sum_lo in eax, sum_hi = 0
    xorl %edx, %edx # edx = 0 (sum_hi)
    movl $0, %ecx # status = 0 (0=fast, 1=slow/wide so 64-bit, 2=error,  not handled)

    jmp .store_and_return


# 2) Wide (64-bit) loop, which comes in when 32-bits is not enough
.wide_mode:
    #setup
    movl 8(%ebp), %ebx # reload pointer (so anything that happened that led to overflow doesn't distort this sum)
    movl 12(%ebp), %edx # reload count
    movl $0, -4(%ebp) # local sum_lo  = 0
    movl $0, -8(%ebp) # local sum_hi  = 0

    xorl %ecx, %ecx # status = 0 (initialized)

.wide_loop:

    movl (%ebx), %ecx # ecx = *numbers
    addl %ecx, -4(%ebp) # sum_lo += ecx
    adcl $0, -8(%ebp) # sum_hi += carry flag from addl we did   (so the result stays in sum_lo, whatever got carried gets added to sum_hi)

    jo .double_overflow # if high-part overflows, return error (status = 2 )

    addl $4, %ebx # numbers++ (next element)
    decl %edx # count--

    jnz .wide_loop # continue wide loop if count>0


    # wide succeeded, load sum_lo, sum_hi into registers
    movl -4(%ebp), %eax # low half in eax
    movl -8(%ebp), %edx # high half in edx
    movl $1, %ecx # status = 1 (wide done)
    jmp .store_and_return

#3) error handling, if even 64-bits overflow
.double_overflow:
    xorl %eax, %eax # sum_lo = 0
    xorl %edx, %edx # sum_hi = 0
    movl $2, %ecx # status = 2 (irrecoverable)

.store_and_return:
    # store results into caller's pointers
    movl %eax, (%esi) # *sum_lo_out = sum_lo
    movl %edx, (%edi) # *sum_hi_out = sum_hi

    # return status in eax: 0=fast(32), 1=wide(64), 2=error
    movl %ecx, %eax


    movl %ebp, %esp # restore stack pointer
    popl %ebp
    ret
