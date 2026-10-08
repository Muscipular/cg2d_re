OPTION CASEMAP:NONE

.const
ALIGN 8
LegacyDivisor54F9E8 QWORD 04147FFFF80000000h
LegacyDividend54F9E0 QWORD 04150017EC0000000h
LegacyThreshold54D5C0 QWORD 03FF0000000000000h
LegacyZero54D5D0 QWORD 0
LegacyIntegerDivisor54FB58 QWORD 04000000000000000h
LegacyPowerInfinity5923C0 QWORD 07FF0000000000000h
LegacyPowerUndefined5923C8 QWORD 0FFF8000000000000h
LegacyPowerNegativeZero5923E0 QWORD 08000000000000000h

.code
EXTERN Cg2dClassifyLegacySpecialFloat:PROC
PUBLIC Cg2dClassifyLegacyFloat
PUBLIC Cg2dClassifyLegacyInteger
PUBLIC Cg2dProcessLegacyPowerSpecial
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

; 原5462FF；私有ASM合同为RCX借用F64、ST(0)返回，不能由CPP按Win64 F64返回调用。
; FRNDINT沿当前x87舍入模式；原F64落存/重载保留，借用调用方8B暂存槽。
Cg2dRoundLegacyFloatX87 PROC
    fld qword ptr [rcx]
    frndint
    fstp qword ptr [rsp+8]
    fld qword ptr [rsp+8]
    ret
Cg2dRoundLegacyFloatX87 ENDP

; 原5436AC；56B展开帧保留32B调用区及原值、传参、舍入落存三个F64槽。
; 三个CALL与原FLD/FSTP、FDIV、FCOMP/FNSTSW/SAHF顺序保持，不设额外有限值门禁。
Cg2dClassifyLegacyInteger PROC FRAME
    sub rsp, 56
    .allocstack 56
    .endprolog
    mov r8, qword ptr [rcx]
    mov qword ptr [rsp+32], r8
    fld qword ptr [rsp+32]
    fstp qword ptr [rsp+40]
    lea rcx, [rsp+40]
    call Cg2dClassifyLegacyFloat
    test al, 090h
    jnz LegacyIntegerClassNoninteger
    fld qword ptr [rsp+32]
    fstp qword ptr [rsp+40]
    lea rcx, [rsp+40]
    call Cg2dRoundLegacyFloatX87
    fstp qword ptr [rsp+48]
    fld qword ptr [rsp+48]
    fcomp qword ptr [rsp+32]
    fnstsw ax
    sahf
    jne LegacyIntegerClassNoninteger
    fld qword ptr [rsp+32]
    fdiv qword ptr [LegacyIntegerDivisor54FB58]
    fstp qword ptr [rsp+32]
    fld qword ptr [rsp+32]
    fstp qword ptr [rsp+40]
    lea rcx, [rsp+40]
    call Cg2dRoundLegacyFloatX87
    fcomp qword ptr [rsp+32]
    fnstsw ax
    sahf
    jne LegacyIntegerClassOdd
    mov eax, 2
    jmp LegacyIntegerClassDone
LegacyIntegerClassOdd:
    mov eax, 1
    jmp LegacyIntegerClassDone
LegacyIntegerClassNoninteger:
    xor eax, eax
LegacyIntegerClassDone:
    add rsp, 56
    ret
Cg2dClassifyLegacyInteger ENDP

; 原54357C；72B正常展开帧承载32B调用区、四F64槽与一个x64结果指针。
; 原0返回同时包含已写出与未写出分支，不增默认结果或有限值保护。
Cg2dProcessLegacyPowerSpecial PROC FRAME
    sub rsp, 72
    .allocstack 72
    .endprolog
    mov r9, qword ptr [rcx]
    mov qword ptr [rsp+32], r9
    mov r9, qword ptr [rdx]
    mov qword ptr [rsp+40], r9
    mov qword ptr [rsp+64], r8
    fld qword ptr [rsp+32]
    fcomp qword ptr [LegacyZero54D5D0]
    fld qword ptr [rsp+32]
    xor edx, edx
    fnstsw ax
    sahf
    jnb LegacyPowerSpecialMagnitudeReady
    fchs
LegacyPowerSpecialMagnitudeReady:
    fstp qword ptr [rsp+48]
    mov eax, 07FF00000h
    mov ecx, 0FFF00000h
    cmp dword ptr [rsp+44], eax
    jne LegacyPowerSpecialNegativeExponentInfinity
    cmp dword ptr [rsp+40], edx
    jne LegacyPowerSpecialPositiveBaseInfinity
    fld qword ptr [rsp+48]
    fcomp qword ptr [LegacyThreshold54D5C0]
    fnstsw ax
    sahf
    ja LegacyPowerSpecialPositiveInfinity
    fld qword ptr [rsp+48]
    fcomp qword ptr [LegacyThreshold54D5C0]
    fnstsw ax
    sahf
    jb LegacyPowerSpecialPositiveZero
LegacyPowerSpecialUndefined:
    fld qword ptr [LegacyPowerUndefined5923C8]
    mov rax, qword ptr [rsp+64]
    fstp qword ptr [rax]
    mov eax, 1
    jmp LegacyPowerSpecialDone
LegacyPowerSpecialNegativeExponentInfinity:
    cmp dword ptr [rsp+44], ecx
    jne LegacyPowerSpecialPositiveBaseInfinity
    cmp dword ptr [rsp+40], edx
    jne LegacyPowerSpecialPositiveBaseInfinity
    fld qword ptr [rsp+48]
    fcomp qword ptr [LegacyThreshold54D5C0]
    fnstsw ax
    sahf
    ja LegacyPowerSpecialPositiveZero
    fld qword ptr [rsp+48]
    fcomp qword ptr [LegacyThreshold54D5C0]
    fnstsw ax
    sahf
    jnb LegacyPowerSpecialUndefined
    jmp LegacyPowerSpecialPositiveInfinity
LegacyPowerSpecialPositiveBaseInfinity:
    cmp dword ptr [rsp+36], eax
    jne LegacyPowerSpecialNegativeBaseInfinity
    cmp dword ptr [rsp+32], edx
    jne LegacyPowerSpecialNoOutput
    fld qword ptr [rsp+40]
    fcomp qword ptr [LegacyZero54D5D0]
    fnstsw ax
    sahf
    jbe LegacyPowerSpecialPositiveBaseNonpositiveExponent
LegacyPowerSpecialPositiveInfinity:
    fld qword ptr [LegacyPowerInfinity5923C0]
    jmp LegacyPowerSpecialWriteResult
LegacyPowerSpecialPositiveBaseNonpositiveExponent:
    fld qword ptr [rsp+40]
    fcomp qword ptr [LegacyZero54D5D0]
    fnstsw ax
    sahf
    jnb LegacyPowerSpecialOne
LegacyPowerSpecialPositiveZero:
    fldz
    jmp LegacyPowerSpecialWriteResult
LegacyPowerSpecialNegativeBaseInfinity:
    cmp dword ptr [rsp+36], ecx
    jne LegacyPowerSpecialNoOutput
    cmp dword ptr [rsp+32], edx
    jne LegacyPowerSpecialNoOutput
    fld qword ptr [rsp+40]
    fstp qword ptr [rsp+56]
    lea rcx, [rsp+56]
    call Cg2dClassifyLegacyInteger
    fld qword ptr [rsp+40]
    fcomp qword ptr [LegacyZero54D5D0]
    mov ecx, eax
    fnstsw ax
    sahf
    jbe LegacyPowerSpecialNegativeBaseNonpositiveExponent
    fld qword ptr [LegacyPowerInfinity5923C0]
    cmp ecx, 1
    jne LegacyPowerSpecialSignedInfinityReady
    fchs
LegacyPowerSpecialSignedInfinityReady:
    fstp qword ptr [rsp+40]
    fld qword ptr [rsp+40]
    jmp LegacyPowerSpecialWriteResult
LegacyPowerSpecialNegativeBaseNonpositiveExponent:
    fld qword ptr [rsp+40]
    fcomp qword ptr [LegacyZero54D5D0]
    fnstsw ax
    sahf
    jnb LegacyPowerSpecialOne
    cmp ecx, 1
    jne LegacyPowerSpecialNegativeBasePositiveZero
    fld qword ptr [LegacyPowerNegativeZero5923E0]
    jmp LegacyPowerSpecialSignedZeroReady
LegacyPowerSpecialNegativeBasePositiveZero:
    fldz
LegacyPowerSpecialSignedZeroReady:
    fstp qword ptr [rsp+40]
    fld qword ptr [rsp+40]
    jmp LegacyPowerSpecialWriteResult
LegacyPowerSpecialOne:
    fld1
LegacyPowerSpecialWriteResult:
    mov rax, qword ptr [rsp+64]
    fstp qword ptr [rax]
LegacyPowerSpecialNoOutput:
    xor eax, eax
LegacyPowerSpecialDone:
    add rsp, 72
    ret
Cg2dProcessLegacyPowerSpecial ENDP

END
