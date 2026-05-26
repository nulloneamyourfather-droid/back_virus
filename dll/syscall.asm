PUBLIC WorkCallback
PUBLIC Work2Callback
PUBLIC Work3Callback
PUBLIC Work4Callback
PUBLIC NtWaitForSingleObject

EXTERN wNtWaitForSingleObject:DWORD
EXTERN sysAddrNtWaitForSingleObject:QWORD

.CODE

WorkCallback PROC
    mov     rbx, rdx
    mov     rax, [rbx]
    mov     rcx, [rbx + 8h]
    mov     rdx, [rbx + 10h]
    xor     r8, r8
    mov     r9, [rbx + 18h]
    mov     r10, [rbx + 20h]
    mov     [rsp + 30h], r10
    mov     r10, 3000h
    mov     [rsp + 28h], r10
    jmp     rax
WorkCallback ENDP

Work2Callback PROC
    mov     rbx, rdx
    mov     rax, [rbx]
    mov     rcx, [rbx + 8h]
    mov     rdx, [rbx + 10h]
    mov     r8,  [rbx + 18h]
    mov     r9,  [rbx + 20h]
    mov     r10, [rbx + 28h]
    mov     [rsp + 28h], r10
    jmp     rax
Work2Callback ENDP

Work3Callback PROC
    mov     rbx, rdx
    mov     rax, [rbx]
    mov     rcx, [rbx + 8h]
    mov     rdx, [rbx + 10h]
    mov     r8,  [rbx + 18h]
    mov     r9,  [rbx + 20h]
    mov     r10, [rbx + 28h]
    mov     [rsp + 28h], r10
    mov     r10, [rbx + 30h]
    mov     [rsp + 30h], r10
    mov     r10, [rbx + 38h]
    mov     [rsp + 38h], r10
    mov     r10, [rbx + 40h]
    mov     [rsp + 40h], r10
    mov     r10, [rbx + 48h]
    mov     [rsp + 48h], r10
    mov     r10, [rbx + 50h]
    mov     [rsp + 50h], r10
    mov     r10, [rbx + 58h]
    mov     [rsp + 58h], r10
    jmp     rax
Work3Callback ENDP

Work4Callback PROC
    mov     rbx, rdx
    mov     rax, [rbx]
    mov     rcx, [rbx + 8h]
    mov     rdx, [rbx + 10h]
    mov     r8,  [rbx + 18h]
    mov     r9,  [rbx + 20h]
    mov     r10, [rbx + 28h]
    mov     [rsp + 28h], r10
    jmp     rax
Work4Callback ENDP

NtWaitForSingleObject PROC
    mov     r10, rcx
    mov     eax, wNtWaitForSingleObject
    jmp     QWORD PTR [sysAddrNtWaitForSingleObject]
NtWaitForSingleObject ENDP

END
