OPTION CASEMAP:NONE

.const
ALIGN 8
LegacyDivisor54F9E8 QWORD 04147FFFF80000000h
LegacyDividend54F9E0 QWORD 04150017EC0000000h
LegacyThreshold54D5C0 QWORD 03FF0000000000000h
LegacyZero54D5D0 QWORD 0

.code
EXTERN Cg2dClassifyLegacySpecialFloat:PROC
PUBLIC Cg2dClassifyLegacyFloat
PUBLIC Cg2dReadLegacyX87Control
PUBLIC Cg2dWriteLegacyX87Control
PUBLIC Cg2dClearLegacyX87Exceptions
PUBLIC Cg2dTestLegacyFdiv
PUBLIC Cg2dLegacyFloatNonnegative

; 叶函数只借用Win64参数暂存区，不改变RSP或非易失通用寄存器。
Cg2dReadLegacyX87Control PROC
    fstcw word ptr [rsp+8]
    movzx eax, word ptr [rsp+8]
    ret
Cg2dReadLegacyX87Control ENDP

Cg2dWriteLegacyX87Control PROC
    mov word ptr [rsp+8], cx
    fldcw word ptr [rsp+8]
    ret
Cg2dWriteLegacyX87Control ENDP

Cg2dClearLegacyX87Exceptions PROC
    fnclex
    ret
Cg2dClearLegacyX87Exceptions ENDP

; 原53FD6E的三处F64落栈边界和比较；探测前不调整控制字。
Cg2dTestLegacyFdiv PROC
    fld qword ptr [LegacyDivisor54F9E8]
    fstp qword ptr [rsp+8]
    fld qword ptr [LegacyDividend54F9E0]
    fstp qword ptr [rsp+16]
    fld qword ptr [rsp+16]
    fdiv qword ptr [rsp+8]
    fmul qword ptr [rsp+8]
    fsubr qword ptr [rsp+16]
    fstp qword ptr [rsp+24]
    fld qword ptr [rsp+24]
    fcomp qword ptr [LegacyThreshold54D5C0]
    fnstsw ax
    test ah, 041h
    jnz LegacyFdivNoBug
    mov eax, 1
    ret
LegacyFdivNoBug:
    xor eax, eax
    ret
Cg2dTestLegacyFdiv ENDP

; 原53FE7D；C0置位表示小于或无序，原JB分支均返回0。
Cg2dLegacyFloatNonnegative PROC
    fld qword ptr [rcx]
    fcomp qword ptr [LegacyZero54D5D0]
    fnstsw ax
    test ah, 1
    jnz LegacyFloatNegativeOrUnordered
    mov eax, 1
    ret
LegacyFloatNegativeOrUnordered:
    xor eax, eax
    ret
Cg2dLegacyFloatNonnegative ENDP

; 原546311；原值槽与真实x87落存槽分离，高端W32仅消费低W16。
; Win64保留32B调用暂存区与两个F64槽；不写控制字/MXCSR或清异常。
Cg2dClassifyLegacyFloat PROC FRAME
    sub rsp, 56
    .allocstack 56
    .endprolog
    mov r8, qword ptr [rcx]
    mov qword ptr [rsp+32], r8
    mov eax, dword ptr [rsp+38]
    mov ecx, 07FF0h
    mov edx, eax
    and edx, ecx
    cmp dx, cx
    jne LegacyFloatClassOrdinary
    fld qword ptr [rsp+32]
    fstp qword ptr [rsp+40]
    lea rcx, [rsp+40]
    call Cg2dClassifyLegacySpecialFloat
    dec eax
    jz LegacyFloatClassPositiveInfinity
    dec eax
    jz LegacyFloatClassNegativeInfinity
    dec eax
    jz LegacyFloatClassQuietNaN
    mov eax, 1
    jmp LegacyFloatClassDone
LegacyFloatClassQuietNaN:
    mov eax, 2
    jmp LegacyFloatClassDone
LegacyFloatClassNegativeInfinity:
    mov eax, 4
    jmp LegacyFloatClassDone
LegacyFloatClassPositiveInfinity:
    mov eax, 0200h
    jmp LegacyFloatClassDone
LegacyFloatClassOrdinary:
    and eax, 08000h
    test dx, dx
    mov ecx, eax
    jnz LegacyFloatClassCompareZero
    test dword ptr [rsp+36], 0FFFFFh
    jnz LegacyFloatClassDenormal
    cmp dword ptr [rsp+32], 0
    je LegacyFloatClassCompareZero
LegacyFloatClassDenormal:
    mov eax, ecx
    neg eax
    sbb eax, eax
    and al, 090h
    add eax, 080h
    jmp LegacyFloatClassDone
LegacyFloatClassCompareZero:
    fld qword ptr [rsp+32]
    fcomp qword ptr [LegacyZero54D5D0]
    fnstsw ax
    sahf
    mov eax, ecx
    jne LegacyFloatClassNormal
    neg eax
    sbb eax, eax
    and al, 0E0h
    add eax, 040h
    jmp LegacyFloatClassDone
LegacyFloatClassNormal:
    neg eax
    sbb eax, eax
    and al, 08h
    add eax, 0100h
LegacyFloatClassDone:
    add rsp, 56
    ret
Cg2dClassifyLegacyFloat ENDP

END
