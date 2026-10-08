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

LegacyMaximum5923D0 QWORD 07FEFFFFFFFFFFFFFh
LegacyStatusHuge5925BC BYTE 00h, 00h, 00h, 00h, 00h, 00h, 00h, 080h, 010h, 044h
LegacyStatusTiny5925C8 BYTE 01h, 00h, 00h, 00h, 00h, 00h, 00h, 080h, 00h, 030h

.code
EXTERN Cg2dClassifyLegacySpecialFloat:PROC
PUBLIC Cg2dClassifyLegacyFloat
PUBLIC Cg2dClassifyLegacyInteger
PUBLIC Cg2dProcessLegacyPowerSpecial
PUBLIC Cg2dLoadLegacyPowerControlWord
PUBLIC Cg2dReadLegacyRawX87Status
PUBLIC Cg2dReadAndClearLegacyRawX87Status
PUBLIC Cg2dControlLegacyRawX87
PUBLIC Cg2dHandleLegacyMathException
PUBLIC Cg2dSetLegacyRawX87Status
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

; 原5433F0；私有ASM间ST(0)输入/输出，保全部十条x87操作与当前控制字。
; 不将连续扩展精度结果落成F64，不通过CPP double返回ABI调用。
Cg2dLegacyTwoToTOSX87 PROC
    fld st(0)
    frndint
    fsubr st(1), st(0)
    fxch st(1)
    fchs
    f2xm1
    fld1
    faddp st(1), st(0)
    fscale
    fstp st(1)
    ret
Cg2dLegacyTwoToTOSX87 ENDP

; 原543405；Win64 CX承载保存的W16控制字，只有原PC两位参与计算。
; 只借用调用暂存槽加载原临时CW，原载体的额外W16不复制为业务值。
Cg2dLoadLegacyPowerControlWord PROC
    movzx edx, cx
    and edx, 0300h
    or edx, 07Fh
    mov word ptr [rsp+8], dx
    fldcw word ptr [rsp+8]
    ret
Cg2dLoadLegacyPowerControlWord ENDP

; 原543435；私有ASM间RDX借用F64输入，ST(0)/EAX/FLAGS返回。
; 普通分支保指数CMP标志；特殊分支原三个重叠W32写出10B F80后TEST EAX,0。
Cg2dLoadLegacyPowerOperandX87 PROC
    mov eax, dword ptr [rdx+4]
    and eax, 07FF00000h
    cmp eax, 07FF00000h
    je LegacyPowerOperandSpecial
    fld qword ptr [rdx]
    ret
LegacyPowerOperandSpecial:
    mov eax, dword ptr [rdx+4]
    or eax, 07FFF0000h
    mov dword ptr [rsp+14], eax
    mov eax, dword ptr [rdx+4]
    mov ecx, dword ptr [rdx]
    shld eax, ecx, 0Bh
    shl ecx, 0Bh
    mov dword ptr [rsp+12], eax
    mov dword ptr [rsp+8], ecx
    fld tbyte ptr [rsp+8]
    test eax, 0
    mov eax, dword ptr [rdx+4]
    ret
Cg2dLoadLegacyPowerOperandX87 ENDP

; 原5478DB __statfp；等待式FSTSW后按原S16符号扩展返回S32。
Cg2dReadLegacyRawX87Status PROC
    fstsw word ptr [rsp+8]
    movsx eax, word ptr [rsp+8]
    ret
Cg2dReadLegacyRawX87Status ENDP

; 原5478E9 __clrfp；返回清除前状态，FNSTSW/FNCLEX顺序保持。
Cg2dReadAndClearLegacyRawX87Status PROC
    fnstsw word ptr [rsp+8]
    fnclex
    movsx eax, word ptr [rsp+8]
    ret
Cg2dReadAndClearLegacyRawX87Status ENDP

; 原5478F8 __ctrlfp；保存W16旧CW，原W32合并后只加载低W16并返回旧CW。
; 原额外高W16不填值/不当业务输入；两个槽借Win64调用暂存区。
Cg2dControlLegacyRawX87 PROC
    fstcw word ptr [rsp+8]
    mov eax, edx
    mov r8d, eax
    and eax, ecx
    not r8d
    and r8d, dword ptr [rsp+8]
    or r8d, eax
    mov dword ptr [rsp+16], r8d
    fldcw word ptr [rsp+16]
    movsx eax, word ptr [rsp+8]
    ret
Cg2dControlLegacyRawX87 ENDP

; 原5461BB；RCX借用F64/EDX为S32指数，ST(0)返回；原F64落存与低W16改写保留。
; 高端W32额外W16不填值/业务消费，AND CX而非AND ECX后只写AX。
Cg2dSetLegacyFloatExponentX87 PROC
    mov r8, qword ptr [rcx]
    mov qword ptr [rsp+8], r8
    mov eax, edx
    mov ecx, dword ptr [rsp+14]
    fld qword ptr [rsp+8]
    add eax, 03FEh
    and cx, 0800Fh
    fstp qword ptr [rsp+16]
    shl eax, 4
    or eax, ecx
    mov word ptr [rsp+22], ax
    fld qword ptr [rsp+16]
    ret
Cg2dSetLegacyFloatExponentX87 ENDP

; 原54623E；RCX借用F64输入/RDX借用S32指数输出，ST(0)返回。
; 72B正常展开帧保32B调用区、输入/结果/输出指针/指数与真实落存参数槽。
Cg2dDecomposeLegacyFloatX87 PROC FRAME
    sub rsp, 72
    .allocstack 72
    .endprolog
    mov r8, qword ptr [rcx]
    mov qword ptr [rsp+32], r8
    mov qword ptr [rsp+48], rdx
    fld qword ptr [rsp+32]
    fcomp qword ptr [LegacyZero54D5D0]
    fnstsw ax
    sahf
    jne LegacyDecomposeNonzero
    fldz
    mov dword ptr [rsp+56], 0
    fstp qword ptr [rsp+40]
    jmp LegacyDecomposePublish
LegacyDecomposeNonzero:
    xor ecx, ecx
    test word ptr [rsp+38], 07FF0h
    jnz LegacyDecomposeNormal
    test dword ptr [rsp+36], 0FFFFFh
    jnz LegacyDecomposeDenormal
    cmp dword ptr [rsp+32], ecx
    je LegacyDecomposeNormal
LegacyDecomposeDenormal:
    fld qword ptr [rsp+32]
    fcomp qword ptr [LegacyZero54D5D0]
    mov dword ptr [rsp+56], 0FFFFFC03h
    fnstsw ax
    sahf
    jnb LegacyDecomposeNonnegative
    mov eax, 1
    jmp LegacyDecomposeShiftCheck
LegacyDecomposeNonnegative:
    xor eax, eax
LegacyDecomposeShiftCheck:
    test byte ptr [rsp+38], 010h
    jnz LegacyDecomposeShiftDone
    shl dword ptr [rsp+36], 1
    test byte ptr [rsp+35], 080h
    jz LegacyDecomposeShiftLow
    or dword ptr [rsp+36], 1
LegacyDecomposeShiftLow:
    shl dword ptr [rsp+32], 1
    dec dword ptr [rsp+56]
    jmp LegacyDecomposeShiftCheck
LegacyDecomposeShiftDone:
    and word ptr [rsp+38], 0FFEFh
    cmp eax, ecx
    je LegacyDecomposeSignReady
    or byte ptr [rsp+39], 080h
LegacyDecomposeSignReady:
    fld qword ptr [rsp+32]
    fstp qword ptr [rsp+64]
    lea rcx, [rsp+64]
    xor edx, edx
    call Cg2dSetLegacyFloatExponentX87
    fstp qword ptr [rsp+40]
    jmp LegacyDecomposePublish
LegacyDecomposeNormal:
    fld qword ptr [rsp+32]
    fstp qword ptr [rsp+64]
    lea rcx, [rsp+64]
    xor edx, edx
    call Cg2dSetLegacyFloatExponentX87
    mov eax, dword ptr [rsp+38]
    fstp qword ptr [rsp+40]
    shr eax, 4
    and ax, 07FFh
    movsx eax, ax
    sub eax, 03FEh
    mov dword ptr [rsp+56], eax
LegacyDecomposePublish:
    mov rcx, qword ptr [rsp+48]
    fld qword ptr [rsp+40]
    mov eax, dword ptr [rsp+56]
    mov dword ptr [rcx], eax
    add rsp, 72
    ret
Cg2dDecomposeLegacyFloatX87 ENDP

; 原54791B；保原低BYTE样本及1/8/10h/4/20h的x87状态生产顺序。
; 仅原FISTP/FSTP/FDIVRP/FLDPI与等待操作，不增额外注入/清除或MXCSR动作。
Cg2dSetLegacyRawX87Status PROC
    mov r9b, cl
    test r9b, 1
    jz LegacySetStatusOverflow
    fld tbyte ptr [LegacyStatusHuge5925BC]
    fistp dword ptr [rsp+8]
    fwait
LegacySetStatusOverflow:
    test r9b, 8
    jz LegacySetStatusUnderflow
    fstsw ax
    fld tbyte ptr [LegacyStatusHuge5925BC]
    fstp qword ptr [rsp+16]
    fwait
    fstsw ax
LegacySetStatusUnderflow:
    test r9b, 010h
    jz LegacySetStatusDivide
    fld tbyte ptr [LegacyStatusTiny5925C8]
    fstp qword ptr [rsp+16]
    fwait
LegacySetStatusDivide:
    test r9b, 4
    jz LegacySetStatusInexact
    fldz
    fld1
    fdivrp st(1), st(0)
    fstp st(0)
    fwait
LegacySetStatusInexact:
    test r9b, 020h
    jz LegacySetStatusDone
    fldpi
    fstp qword ptr [rsp+16]
    fwait
LegacySetStatusDone:
    ret
Cg2dSetLegacyRawX87Status ENDP

; 原54769B；88B正常展开帧承载32B调用区、原条件/剩余位/CW/借用指针与F64/S32槽。
; 依原优先级处理8/4/1/2后再16，保原状态生产与真实分解/指数依赖。
Cg2dHandleLegacyMathException PROC FRAME
    sub rsp, 88
    .allocstack 88
    .endprolog
    mov dword ptr [rsp+32], ecx
    mov eax, ecx
    and eax, 01Fh
    mov dword ptr [rsp+36], eax
    mov qword ptr [rsp+40], rdx
    mov dword ptr [rsp+48], r8d
    test cl, 8
    jz LegacyHandleCheckDivide
    test byte ptr [rsp+48], 1
    jz LegacyHandleCheckDivide
    mov ecx, 1
    call Cg2dSetLegacyRawX87Status
    and dword ptr [rsp+36], 0FFFFFFF7h
    jmp LegacyHandleInexact
LegacyHandleCheckDivide:
    test byte ptr [rsp+32], 4
    jz LegacyHandleCheckOverflow
    test byte ptr [rsp+48], 4
    jz LegacyHandleCheckOverflow
    mov ecx, 4
    call Cg2dSetLegacyRawX87Status
    and dword ptr [rsp+36], 0FFFFFFFBh
    jmp LegacyHandleInexact
LegacyHandleCheckOverflow:
    test byte ptr [rsp+32], 1
    jz LegacyHandleCheckUnderflow
    test byte ptr [rsp+48], 8
    jz LegacyHandleCheckUnderflow
    mov ecx, 8
    call Cg2dSetLegacyRawX87Status
    mov eax, 0C00h
    mov ecx, dword ptr [rsp+48]
    and ecx, eax
    jz LegacyHandleOverflowNearest
    cmp ecx, 0400h
    je LegacyHandleOverflowDown
    cmp ecx, 0800h
    je LegacyHandleOverflowUp
    cmp ecx, eax
    jne LegacyHandleOverflowDone
    mov rcx, qword ptr [rsp+40]
    fld qword ptr [rcx]
    fcomp qword ptr [LegacyZero54D5D0]
    fld qword ptr [LegacyMaximum5923D0]
    fnstsw ax
    sahf
    ja LegacyHandleOverflowTruncateReady
    fchs
LegacyHandleOverflowTruncateReady:
    fstp qword ptr [rsp+56]
    fld qword ptr [rsp+56]
    jmp LegacyHandleOverflowPublish
LegacyHandleOverflowUp:
    mov rcx, qword ptr [rsp+40]
    fld qword ptr [rcx]
    fcomp qword ptr [LegacyZero54D5D0]
    fnstsw ax
    sahf
    jbe LegacyHandleOverflowUpNegative
    fld qword ptr [LegacyPowerInfinity5923C0]
    jmp LegacyHandleOverflowUpReady
LegacyHandleOverflowUpNegative:
    fld qword ptr [LegacyMaximum5923D0]
    fchs
LegacyHandleOverflowUpReady:
    fstp qword ptr [rsp+56]
    fld qword ptr [rsp+56]
    jmp LegacyHandleOverflowPublish
LegacyHandleOverflowDown:
    mov rcx, qword ptr [rsp+40]
    fld qword ptr [rcx]
    fcomp qword ptr [LegacyZero54D5D0]
    fnstsw ax
    sahf
    jbe LegacyHandleOverflowDownNegative
    fld qword ptr [LegacyMaximum5923D0]
    jmp LegacyHandleOverflowDownReady
LegacyHandleOverflowDownNegative:
    fld qword ptr [LegacyPowerInfinity5923C0]
    fchs
LegacyHandleOverflowDownReady:
    fstp qword ptr [rsp+56]
    fld qword ptr [rsp+56]
    jmp LegacyHandleOverflowPublish
LegacyHandleOverflowNearest:
    mov rcx, qword ptr [rsp+40]
    fld qword ptr [rcx]
    fcomp qword ptr [LegacyZero54D5D0]
    fld qword ptr [LegacyPowerInfinity5923C0]
    fnstsw ax
    sahf
    ja LegacyHandleOverflowNearestReady
    fchs
LegacyHandleOverflowNearestReady:
    fstp qword ptr [rsp+56]
    fld qword ptr [rsp+56]
LegacyHandleOverflowPublish:
    fstp qword ptr [rcx]
LegacyHandleOverflowDone:
    and dword ptr [rsp+36], 0FFFFFFFEh
    jmp LegacyHandleInexact
LegacyHandleCheckUnderflow:
    test byte ptr [rsp+32], 2
    jz LegacyHandleInexact
    test byte ptr [rsp+48], 010h
    jz LegacyHandleInexact
    mov dword ptr [rsp+52], 0
    test byte ptr [rsp+32], 010h
    jz LegacyHandleUnderflowSample
    mov dword ptr [rsp+52], 1
LegacyHandleUnderflowSample:
    mov rax, qword ptr [rsp+40]
    fld qword ptr [rax]
    fstp qword ptr [rsp+56]
    fld qword ptr [rsp+56]
    fcomp qword ptr [LegacyZero54D5D0]
    fnstsw ax
    sahf
    je LegacyHandleUnderflowZero
    fld qword ptr [rsp+56]
    fstp qword ptr [rsp+72]
    lea rcx, [rsp+72]
    lea rdx, [rsp+64]
    call Cg2dDecomposeLegacyFloatX87
    mov eax, dword ptr [rsp+64]
    fstp qword ptr [rsp+56]
    lea ecx, [eax-0600h]
    cmp ecx, 0FFFFFBCEh
    jge LegacyHandleUnderflowNormalize
    fldz
    mov dword ptr [rsp+52], 1
    fstp qword ptr [rsp+56]
    jmp LegacyHandleUnderflowPublish
LegacyHandleUnderflowNormalize:
    fld qword ptr [rsp+56]
    fcomp qword ptr [LegacyZero54D5D0]
    fnstsw ax
    sahf
    jnb LegacyHandleUnderflowNonnegative
    mov edx, 1
    jmp LegacyHandleUnderflowSignReady
LegacyHandleUnderflowNonnegative:
    xor edx, edx
LegacyHandleUnderflowSignReady:
    mov al, byte ptr [rsp+62]
    and eax, 0Fh
    or al, 010h
    mov word ptr [rsp+62], ax
    mov eax, 0FFFFFC03h
    cmp ecx, eax
    jge LegacyHandleUnderflowShiftDone
    sub eax, ecx
LegacyHandleUnderflowShift:
    test byte ptr [rsp+56], 1
    jz LegacyHandleUnderflowShiftLow
    cmp dword ptr [rsp+52], 0
    jne LegacyHandleUnderflowShiftLow
    mov dword ptr [rsp+52], 1
LegacyHandleUnderflowShiftLow:
    shr dword ptr [rsp+56], 1
    test byte ptr [rsp+60], 1
    jz LegacyHandleUnderflowShiftHigh
    or byte ptr [rsp+59], 080h
LegacyHandleUnderflowShiftHigh:
    shr dword ptr [rsp+60], 1
    dec eax
    jne LegacyHandleUnderflowShift
LegacyHandleUnderflowShiftDone:
    test edx, edx
    jz LegacyHandleUnderflowPublish
    fld qword ptr [rsp+56]
    fchs
    fstp qword ptr [rsp+56]
LegacyHandleUnderflowPublish:
    fld qword ptr [rsp+56]
    mov rax, qword ptr [rsp+40]
    fstp qword ptr [rax]
    jmp LegacyHandleUnderflowStatus
LegacyHandleUnderflowZero:
    mov dword ptr [rsp+52], 1
LegacyHandleUnderflowStatus:
    cmp dword ptr [rsp+52], 0
    je LegacyHandleUnderflowDone
    mov ecx, 010h
    call Cg2dSetLegacyRawX87Status
LegacyHandleUnderflowDone:
    and dword ptr [rsp+36], 0FFFFFFFDh
LegacyHandleInexact:
    test byte ptr [rsp+32], 010h
    jz LegacyHandleDone
    test byte ptr [rsp+48], 020h
    jz LegacyHandleDone
    mov ecx, 020h
    call Cg2dSetLegacyRawX87Status
    and dword ptr [rsp+36], 0FFFFFFEFh
LegacyHandleDone:
    xor eax, eax
    cmp dword ptr [rsp+36], 0
    sete al
    add rsp, 88
    ret
Cg2dHandleLegacyMathException ENDP

END
