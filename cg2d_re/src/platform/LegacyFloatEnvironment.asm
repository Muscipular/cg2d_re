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

LegacyRangeMinimum54FB2C QWORD 0010000000000000h
LegacyRangeScaleDown54FB34 QWORD 0C098000000000000h
LegacyRangeScaleUp54FB3C QWORD 04098000000000000h

LegacyGenericPowerHalf58F890 QWORD 03FE0000000000000h
LegacyGenericPowerInfinity591FF0 BYTE 00h, 00h, 00h, 00h, 00h, 00h, 00h, 080h, 0FFh, 07Fh
LegacyGenericPowerUndefined592060 BYTE 00h, 00h, 00h, 00h, 00h, 00h, 00h, 0C0h, 0FFh, 0FFh
LegacyGenericPowerName58F898 BYTE "pow", 0

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
PUBLIC Cg2dRaiseLegacyFloatingPointException
PUBLIC Cg2dProcessLegacyX87Exception
PUBLIC Cg2dStartLegacyTwoArgumentErrorX87
PUBLIC Cg2dStartLegacyOneArgumentErrorX87
PUBLIC Cg2dCheckLegacyRangeExitX87
PUBLIC Cg2dComputeLegacyPower
PUBLIC Cg2dLegacyCIPowerX87
PUBLIC Cg2dLegacyPowerCoreX87
PUBLIC Cg2dClassifyLegacyPowerExponentX87
EXTERN Cg2dLegacyPowerFastExitGate1085FBC:DWORD
EXTERN Cg2dRaiseLegacyFloatExceptionHost:PROC
EXTERN Cg2dDefaultLegacyMathError:PROC
EXTERN Cg2dSetLegacyMathErrno:PROC
EXTERN Cg2dLegacyMathErrorCallbackGate5925B8:DWORD
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

; 原5473E8；88B原数值记录不扩宽，参数指针槽及六参调用见数学异常分派SPEC。
; 三保存寄存器/80B正常帧承接原W32字段、借用指针与原读取/改写顺序。
Cg2dRaiseLegacyFloatingPointException PROC FRAME
    push rbx
    .pushreg rbx
    push rsi
    .pushreg rsi
    push rdi
    .pushreg rdi
    sub rsp, 80
    .allocstack 80
    .endprolog
    mov qword ptr [rsp+32], rcx
    mov qword ptr [rsp+40], rdx
    mov dword ptr [rsp+72], r8d
    mov dword ptr [rsp+52], r9d
    mov rax, qword ptr [rsp+144]
    mov qword ptr [rsp+56], rax
    mov rax, qword ptr [rsp+152]
    mov qword ptr [rsp+64], rax
    mov rcx, qword ptr [rsp+32]
    xor eax, eax
    mov [rcx+4], eax
    mov rcx, qword ptr [rsp+32]
    mov ebx, 1
    mov [rcx+8], eax
    mov rcx, qword ptr [rsp+32]
    mov [rcx+0Ch], eax
    mov cl, byte ptr [rsp+72]
    test cl, 10h
    jz LegacyRaise_54741A
    mov rax, qword ptr [rsp+32]
    mov dword ptr [rsp+72], 0C000008Fh
    or [rax+4], ebx
LegacyRaise_54741A:
    test cl, 2
    jz LegacyRaise_54742D
    mov rax, qword ptr [rsp+32]
    mov dword ptr [rsp+72], 0C0000093h
    or dword ptr [rax+4], 2
LegacyRaise_54742D:
    test bl, cl
    jz LegacyRaise_54743F
    mov rax, qword ptr [rsp+32]
    mov dword ptr [rsp+72], 0C0000091h
    or dword ptr [rax+4], 4
LegacyRaise_54743F:
    test cl, 4
    jz LegacyRaise_547452
    mov rax, qword ptr [rsp+32]
    mov dword ptr [rsp+72], 0C000008Eh
    or dword ptr [rax+4], 8
LegacyRaise_547452:
    test cl, 8
    jz LegacyRaise_547465
    mov rax, qword ptr [rsp+32]
    mov dword ptr [rsp+72], 0C0000090h
    or dword ptr [rax+4], 10h
LegacyRaise_547465:
    mov rsi, qword ptr [rsp+40]
    mov rax, qword ptr [rsp+32]
    mov ecx, [rsi]
    mov edx, [rax+8]
    not ecx
    and ecx, ebx
    and edx, 0FFFFFFEFh
    shl ecx, 4
    or ecx, edx
    mov edi, 2
    mov dword ptr [rax+8], ecx
    mov ecx, [rsi]
    mov rax, qword ptr [rsp+32]
    not ecx
    mov edx, [rax+8]
    and ecx, 4
    shl ecx, 1
    and edx, 0FFFFFFF7h
    or ecx, edx
    mov [rax+8], ecx
    mov ecx, [rsi]
    mov rax, qword ptr [rsp+32]
    not ecx
    mov edx, [rax+8]
    shr ecx, 1
    and ecx, 4
    and edx, 0FFFFFFFBh
    or ecx, edx
    mov [rax+8], ecx
    mov ecx, [rsi]
    mov rax, qword ptr [rsp+32]
    not ecx
    mov edx, [rax+8]
    shr ecx, 3
    and ecx, edi
    and edx, 0FFFFFFFDh
    or ecx, edx
    mov [rax+8], ecx
    mov ecx, [rsi]
    mov rax, qword ptr [rsp+32]
    not ecx
    mov edx, [rax+8]
    shr ecx, 5
    and ecx, ebx
    and edx, 0FFFFFFFEh
    or ecx, edx
    mov [rax+8], ecx
    call Cg2dReadLegacyRawX87Status
    test bl, al
    jz LegacyRaise_5474EE
    mov rcx, qword ptr [rsp+32]
    or dword ptr [rcx+0Ch], 10h
LegacyRaise_5474EE:
    test al, 4
    jz LegacyRaise_5474F9
    mov rcx, qword ptr [rsp+32]
    or dword ptr [rcx+0Ch], 8
LegacyRaise_5474F9:
    test al, 8
    jz LegacyRaise_547504
    mov rcx, qword ptr [rsp+32]
    or dword ptr [rcx+0Ch], 4
LegacyRaise_547504:
    test al, 10h
    jz LegacyRaise_54750E
    mov rcx, qword ptr [rsp+32]
    or [rcx+0Ch], edi
LegacyRaise_54750E:
    test al, 20h
    jz LegacyRaise_547518
    mov rax, qword ptr [rsp+32]
    or [rax+0Ch], ebx
LegacyRaise_547518:
    mov eax, [rsi]
    mov ecx, 0C00h
    and eax, ecx
    jz LegacyRaise_547557
    cmp eax, 400h
    jz LegacyRaise_547549
    cmp eax, 800h
    jz LegacyRaise_54753D
    cmp eax, ecx
    jnz LegacyRaise_54755D
    mov rax, qword ptr [rsp+32]
    or dword ptr [rax], 3
    jmp LegacyRaise_54755D
LegacyRaise_54753D:
    mov rax, qword ptr [rsp+32]
    mov ecx, [rax]
    and ecx, 0FFFFFFFEh
    or ecx, edi
    jmp LegacyRaise_547553
LegacyRaise_547549:
    mov rax, qword ptr [rsp+32]
    mov ecx, [rax]
    and ecx, 0FFFFFFFDh
    or ecx, ebx
LegacyRaise_547553:
    mov [rax], ecx
    jmp LegacyRaise_54755D
LegacyRaise_547557:
    mov rax, qword ptr [rsp+32]
    and dword ptr [rax], 0FFFFFFFCh
LegacyRaise_54755D:
    mov eax, [rsi]
    mov ecx, 300h
    and eax, ecx
    jz LegacyRaise_547588
    cmp eax, 200h
    jz LegacyRaise_54757B
    cmp eax, ecx
    jnz LegacyRaise_547595
    mov rax, qword ptr [rsp+32]
    and dword ptr [rax], 0FFFFFFE3h
    jmp LegacyRaise_547595
LegacyRaise_54757B:
    mov rax, qword ptr [rsp+32]
    mov ecx, [rax]
    and ecx, 0FFFFFFE7h
    or ecx, 4
    jmp LegacyRaise_547593
LegacyRaise_547588:
    mov rax, qword ptr [rsp+32]
    mov ecx, [rax]
    and ecx, 0FFFFFFEBh
    or ecx, 8
LegacyRaise_547593:
    mov [rax], ecx
LegacyRaise_547595:
    mov rax, qword ptr [rsp+32]
    mov ecx, [rsp+52]
    and ecx, 0FFFh
    mov edx, [rax]
    shl ecx, 5
    and edx, 0FFFE001Fh
    or ecx, edx
    mov [rax], ecx
    mov rax, qword ptr [rsp+32]
    or [rax+20h], ebx
    mov rax, qword ptr [rsp+32]
    mov ecx, [rax+20h]
    and ecx, 0FFFFFFE3h
    or ecx, edi
    mov [rax+20h], ecx
    mov rax, qword ptr [rsp+56]
    fld qword ptr [rax]
    mov rax, qword ptr [rsp+32]
    fstp qword ptr [rax+10h]
    mov rax, qword ptr [rsp+32]
    or [rax+50h], ebx
    mov rax, qword ptr [rsp+32]
    mov ecx, [rax+50h]
    and ecx, 0FFFFFFE3h
    or ecx, edi
    mov rdi, qword ptr [rsp+64]
    mov [rax+50h], ecx
    mov rax, qword ptr [rsp+32]
    fld qword ptr [rdi]
    fstp qword ptr [rax+40h]
    call Cg2dReadAndClearLegacyRawX87Status
    lea rdx, [rsp+32]
    mov ecx, dword ptr [rsp+72]
    call Cg2dRaiseLegacyFloatExceptionHost
    mov rax, qword ptr [rsp+32]
    test byte ptr [rax+8], 10h
    jz LegacyRaise_54760F
    and dword ptr [rsi], 0FFFFFFFEh
LegacyRaise_54760F:
    test byte ptr [rax+8], 8
    jz LegacyRaise_547618
    and dword ptr [rsi], 0FFFFFFFBh
LegacyRaise_547618:
    test byte ptr [rax+8], 4
    jz LegacyRaise_547621
    and dword ptr [rsi], 0FFFFFFF7h
LegacyRaise_547621:
    test byte ptr [rax+8], 2
    jz LegacyRaise_54762A
    and dword ptr [rsi], 0FFFFFFEFh
LegacyRaise_54762A:
    test byte ptr [rax+8], bl
    jz LegacyRaise_547632
    and dword ptr [rsi], 0FFFFFFDFh
LegacyRaise_547632:
    mov ecx, [rax]
    mov edx, 0FFFFF3FFh
    and ecx, 3
    sub ecx, 0
    jz LegacyRaise_547666
    dec ecx
    jz LegacyRaise_54765A
    dec ecx
    jz LegacyRaise_547650
    dec ecx
    jnz LegacyRaise_547668
    or byte ptr [rsi+1], 0Ch
    jmp LegacyRaise_547668
LegacyRaise_547650:
    mov ecx, [rsi]
    and ch, 0FBh
    or ch, 8
    jmp LegacyRaise_547662
LegacyRaise_54765A:
    mov ecx, [rsi]
    and ch, 0F7h
    or ch, 4
LegacyRaise_547662:
    mov [rsi], ecx
    jmp LegacyRaise_547668
LegacyRaise_547666:
    and [rsi], edx
LegacyRaise_547668:
    mov ecx, [rax]
    shr ecx, 2
    and ecx, 7
    sub ecx, 0
    jz LegacyRaise_547688
    dec ecx
    jz LegacyRaise_54767F
    dec ecx
    jnz LegacyRaise_547691
    and [rsi], edx
    jmp LegacyRaise_547691
LegacyRaise_54767F:
    mov ecx, [rsi]
    and ecx, edx
    or ch, 2
    jmp LegacyRaise_54768F
LegacyRaise_547688:
    mov ecx, [rsi]
    and ecx, edx
    or ch, 3
LegacyRaise_54768F:
    mov [rsi], ecx
LegacyRaise_547691:
    fld qword ptr [rax+40h]
    fstp qword ptr [rdi]
    add rsp, 80
    pop rdi
    pop rsi
    pop rbx
    ret
Cg2dRaiseLegacyFloatingPointException ENDP

; 原5460EB；三保存寄存器/176B正常帧承接88B原参数记录与六参调用区。
; 原保存CW仅读W16后零扩展到本调用W32，原记录只扩既有名称指针。
Cg2dProcessLegacyX87Exception PROC FRAME
    push rbx
    .pushreg rbx
    push rsi
    .pushreg rsi
    push rdi
    .pushreg rdi
    sub rsp, 176
    .allocstack 176
    .endprolog
    mov dword ptr [rsp+48], ecx
    mov rsi, rdx
    movzx ecx, word ptr [r8]
    mov dword ptr [rsp+52], ecx
    mov eax, [rsi]
    dec eax
    jz Legacy87_546130
    dec eax
    jz Legacy87_54612C
    dec eax
    jz Legacy87_546128
    dec eax
    jz Legacy87_546124
    dec eax
    jz Legacy87_546130
    dec eax
    dec eax
    jz Legacy87_54611C
    dec eax
    jnz Legacy87_546186
    mov ebx, 010h
    jmp Legacy87_546132
Legacy87_54611C:
    mov dword ptr [rsi], 1
    jmp Legacy87_546186
Legacy87_546124:
    mov ebx, 012h
    jmp Legacy87_546132
Legacy87_546128:
    mov ebx, 011h
    jmp Legacy87_546132
Legacy87_54612C:
    mov ebx, 4
    jmp Legacy87_546132
Legacy87_546130:
    mov ebx, 8
Legacy87_546132:
    lea rdi, [rsi+32]
    mov r8d, dword ptr [rsp+52]
    mov rdx, rdi
    mov ecx, ebx
    call Cg2dHandleLegacyMathException
    test eax, eax
    jnz Legacy87_546186
    mov eax, dword ptr [rsp+48]
    cmp eax, 10h
    jz Legacy87_54615D
    cmp eax, 16h
    jz Legacy87_54615D
    cmp eax, 1Dh
    jz Legacy87_54615D
    and dword ptr [rsp+136], 0FFFFFFFEh
    jmp Legacy87_54616F
Legacy87_54615D:
    mov ecx, dword ptr [rsp+136]
    fld qword ptr [rsi+24]
    and ecx, 0FFFFFFE3h
    fstp qword ptr [rsp+120]
    or ecx, 3
    mov dword ptr [rsp+136], ecx
Legacy87_54616F:
    lea r10, [rsi+16]
    mov qword ptr [rsp+40], rdi
    mov qword ptr [rsp+32], r10
    mov r9d, eax
    lea rdx, [rsp+52]
    mov r8d, ebx
    lea rcx, [rsp+80]
    call Cg2dRaiseLegacyFloatingPointException
Legacy87_546186:
    mov edx, 0FFFFh
    mov ecx, dword ptr [rsp+52]
    call Cg2dControlLegacyRawX87
    cmp dword ptr [rsi], 8
    jz Legacy87_5461AE
    cmp dword ptr [Cg2dLegacyMathErrorCallbackGate5925B8], 0
    jnz Legacy87_5461AE
    mov rcx, rsi
    call Cg2dDefaultLegacyMathError
    test eax, eax
    jnz Legacy87_5461B6
Legacy87_5461AE:
    mov ecx, dword ptr [rsi]
    call Cg2dSetLegacyMathErrno
Legacy87_5461B6:
    add rsp, 176
    pop rdi
    pop rsi
    pop rbx
    ret
Cg2dProcessLegacyX87Exception ENDP

; 原543390；RCX借用输入/EDX为S32类型/ST(0)为结果；双参低后高两个W32原样复制。
; 与单参入口有同一88B展开帧，尾转真实共享段，不另复制共享体或补默认第二参数。
Cg2dStartLegacyTwoArgumentErrorX87 PROC FRAME
    sub rsp, 88
    .allocstack 88
    .endprolog
    mov qword ptr [rsp+72], rcx
    mov dword ptr [rsp+32], edx
    mov eax, dword ptr [rcx+24]
    mov dword ptr [rsp+56], eax
    mov eax, dword ptr [rcx+28]
    mov dword ptr [rsp+60], eax
    jmp LegacyMathErrorContinue
Cg2dStartLegacyTwoArgumentErrorX87 ENDP

; 原5433A7及5433B0共享段；两入口同一88B正常帧/40B原数学记录/借用上下文槽。
; 单参仅生产类型后落结果，不写第二F64；共享段真调原87except，重载结果后条件恢复CW。
Cg2dStartLegacyOneArgumentErrorX87 PROC FRAME
    sub rsp, 88
    .allocstack 88
    .endprolog
    mov qword ptr [rsp+72], rcx
    mov dword ptr [rsp+32], edx
LegacyMathErrorContinue::
    fstp qword ptr [rsp+64]
    mov rax, qword ptr [rcx+8]
    mov qword ptr [rsp+40], rax
    mov eax, dword ptr [rcx+16]
    mov r9d, dword ptr [rcx+20]
    mov dword ptr [rsp+48], eax
    mov dword ptr [rsp+52], r9d
    lea r8, [rcx]
    lea rdx, [rsp+32]
    mov ecx, dword ptr [rcx+4]
    call Cg2dProcessLegacyX87Exception
    fld qword ptr [rsp+64]
    mov rcx, qword ptr [rsp+72]
    cmp word ptr [rcx], 027Fh
    jz LegacyMathErrorControlRestored
    fldcw word ptr [rcx]
LegacyMathErrorControlRestored:
    add rsp, 88
    ret
Cg2dStartLegacyOneArgumentErrorX87 ENDP

; 原5434D9；RCX借用输入/ST(0)连续结果，56B正常帧保32B调用区/输入指针与原FST临时值。
; 保原FST不弹栈、CW/状态读取、缩放比较/乘法及三个返回路径；没有新增异常保护。
Cg2dCheckLegacyRangeExitX87 PROC FRAME
    sub rsp, 56
    .allocstack 56
    .endprolog
    mov qword ptr [rsp+32], rcx
    fst qword ptr [rsp+40]
    mov eax, dword ptr [rsp+44]
    and eax, 07FF00000h
    jz LegacyRangeUnderflow
    cmp eax, 07FF00000h
    jz LegacyRangeOverflow
    mov ax, word ptr [rcx]
    cmp ax, 027Fh
    jz LegacyRangeDone
    and ax, 020h
    jnz LegacyRangeRestoreControl
    fstsw ax
    and ax, 020h
    jz LegacyRangeRestoreControl
    mov eax, 8
LegacyRangeHaveError:
    cmp dword ptr [rcx+4], 01Dh
    jz LegacyRangeTwoArguments
    mov edx, eax
    call Cg2dStartLegacyOneArgumentErrorX87
    mov rcx, qword ptr [rsp+32]
    mov edx, dword ptr [rcx]
    add rsp, 56
    ret
LegacyRangeTwoArguments:
    mov edx, eax
    call Cg2dStartLegacyTwoArgumentErrorX87
    mov rcx, qword ptr [rsp+32]
    mov edx, dword ptr [rcx]
    add rsp, 56
    ret
LegacyRangeRestoreControl:
    fldcw word ptr [rcx]
LegacyRangeDone:
    mov rcx, qword ptr [rsp+32]
    mov edx, dword ptr [rcx]
    add rsp, 56
    ret
LegacyRangeUnderflow:
    fld qword ptr [LegacyRangeScaleUp54FB3C]
    fxch st(1)
    fscale
    fstp st(1)
    fld st(0)
    fabs
    fcomp qword ptr [LegacyRangeMinimum54FB2C]
    fstsw ax
    sahf
    mov eax, 4
    jnb LegacyRangeHaveError
    fmul qword ptr [LegacyZero54D5D0]
    jmp LegacyRangeHaveError
LegacyRangeOverflow:
    fld qword ptr [LegacyRangeScaleDown54FB34]
    fxch st(1)
    fscale
    fstp st(1)
    fld st(0)
    fabs
    fcomp qword ptr [LegacyMaximum5923D0]
    fstsw ax
    sahf
    mov eax, 3
    jbe LegacyRangeHaveError
    fmul qword ptr [LegacyPowerInfinity5923C0]
    jmp LegacyRangeHaveError
Cg2dCheckLegacyRangeExitX87 ENDP

; 原53D1F5；ST0指数被原比较/半乘法/弹栈消费，CL保原0/1/2，其他ECX位不改。
Cg2dClassifyLegacyPowerExponentX87 PROC
    fld st(0)
    frndint
    fcomp st(1)
    mov cl, 0
    fstsw ax
    sahf
    jnz LegacyPowerExponentNotInteger
    fmul qword ptr [LegacyGenericPowerHalf58F890]
    inc cl
    fld st(0)
    frndint
    fcompp
    fstsw ax
    sahf
    jnz LegacyPowerExponentDone
    inc cl
LegacyPowerExponentDone:
    ret
LegacyPowerExponentNotInteger:
    fstp st(0)
    ret
Cg2dClassifyLegacyPowerExponentX87 ENDP

; 原53D022完整130条及54348E共同出口；RCX借用两个F64，EAX为已加载Y高W32，ST0为Y。
; 232B正常帧保32B调用区、32B原错误输入、原样本与两实参/结果及108B不解释FPU保存区。
Cg2dLegacyPowerCoreX87 PROC FRAME
    sub rsp, 232
    .allocstack 232
    .endprolog
    mov r10, qword ptr [rcx]
    mov r11, qword ptr [rcx+8]
    mov qword ptr [rsp+48], r10
    mov qword ptr [rsp+56], r11
    mov ecx, eax
    mov dword ptr [rsp+64], ecx
    mov dword ptr [rsp+32], eax
    fstcw word ptr [rsp+32]
    cmp word ptr [rsp+32], 027Fh
    jz LegacyGenericPower_53D036
    movzx ecx, word ptr [rsp+32]
    call Cg2dLoadLegacyPowerControlWord
    mov ecx, dword ptr [rsp+64]
LegacyGenericPower_53D036:
    and ecx, 7FF00000h
    lea rdx, [rsp+48]
    cmp ecx, 7FF00000h
    jz LegacyGenericPower_53D0E9
    call Cg2dLoadLegacyPowerOperandX87
    jz LegacyGenericPower_53D0E5
    test eax, 7FF00000h
    jz LegacyGenericPower_53D158
LegacyGenericPower_53D062:
    mov cl, byte ptr [rsp+55]
    and cl, 80h
    jnz LegacyGenericPower_53D1D0
LegacyGenericPower_53D06F:
    fyl2x
    call Cg2dLegacyTwoToTOSX87
    cmp cl, 1
    jnz LegacyGenericPower_53D07D
    fchs
LegacyGenericPower_53D07D:
    cmp dword ptr [Cg2dLegacyPowerFastExitGate1085FBC], 0
    jnz LegacyGenericPower_54348E
    lea r10, LegacyGenericPowerName58F898
    mov qword ptr [rsp+40], r10
    mov dword ptr [rsp+36], 01Dh
    lea rcx, [rsp+32]
    call Cg2dCheckLegacyRangeExitX87
    add rsp, 232
    ret
LegacyGenericPower_53D09A:
    cmp dword ptr [Cg2dLegacyPowerFastExitGate1085FBC], 0
    jnz LegacyGenericPower_54348E
    lea r10, LegacyGenericPowerName58F898
    mov qword ptr [rsp+40], r10
    mov dword ptr [rsp+36], 01Dh
    mov edx, eax
    lea rcx, [rsp+32]
    call Cg2dStartLegacyTwoArgumentErrorX87
    mov edx, dword ptr [rsp+32]
    add rsp, 232
    ret
LegacyGenericPower_53D0B9:
    lea rdx, [rsp+48]
    call Cg2dLoadLegacyPowerOperandX87
    test byte ptr [rsp+62], 8
    jnz LegacyGenericPower_53D0CC
    inc ecx
    jmp LegacyGenericPower_53D0FF
LegacyGenericPower_53D0CC:
    faddp st(1), st(0)
    mov eax, 1
    jmp LegacyGenericPower_53D09A
LegacyGenericPower_53D0D5:
    test byte ptr [rsp+54], 8
    jnz LegacyGenericPower_53D0CC
LegacyGenericPower_53D0DC:
    faddp st(1), st(0)
    mov eax, 7
    jmp LegacyGenericPower_53D09A
LegacyGenericPower_53D0E5:
    xor ecx, ecx
    jmp LegacyGenericPower_53D0FF
LegacyGenericPower_53D0E9:
    xor ecx, ecx
    and eax, 0FFFFFh
    or eax, [rsp+56]
    jnz LegacyGenericPower_53D0B9
    lea rdx, [rsp+48]
    call Cg2dLoadLegacyPowerOperandX87
LegacyGenericPower_53D0FF:
    mov eax, [rsp+52]
    mov edx, eax
    and eax, 7FF00000h
    and edx, 0FFFFFh
    cmp eax, 7FF00000h
    jnz LegacyGenericPower_53D11D
    or edx, [rsp+48]
    jnz LegacyGenericPower_53D0D5
LegacyGenericPower_53D11D:
    test ecx, ecx
    jnz LegacyGenericPower_53D0DC
    fstp qword ptr [rsp+80]
    fstp qword ptr [rsp+88]
    fsave [rsp+104]
    lea rcx, [rsp+80]
    lea rdx, [rsp+88]
    lea r8, [rsp+96]
    call Cg2dProcessLegacyPowerSpecial
    frstor [rsp+104]
    fld qword ptr [rsp+96]
    test eax, eax
    jz LegacyGenericPower_54348E
    mov eax, 1
    jmp LegacyGenericPower_53D09A
LegacyGenericPower_53D158:
    mov eax, [rsp+52]
    and eax, 0FFFFFh
    or eax, [rsp+48]
    jnz LegacyGenericPower_53D062
    fstp st(0)
    mov eax, [rsp+60]
    test eax, 7FF00000h
    jz LegacyGenericPower_53D1B5
    call Cg2dClassifyLegacyPowerExponentX87
    mov ch, byte ptr [rsp+55]
    shr ch, 7
    test dword ptr [rsp+63], 80h
    jz LegacyGenericPower_53D1A4
    fld tbyte ptr [LegacyGenericPowerInfinity591FF0]
    test ch, cl
    jz LegacyGenericPower_53D19A
    fchs
LegacyGenericPower_53D19A:
    mov eax, 2
    jmp LegacyGenericPower_53D09A
LegacyGenericPower_53D1A4:
    fldz
    test ch, cl
    jz LegacyGenericPower_54348E
    fchs
    jmp LegacyGenericPower_54348E
LegacyGenericPower_53D1B5:
    fstp st(0)
    and eax, 0FFFFFh
    or eax, [rsp+56]
    jnz LegacyGenericPower_53D1C9
    fld1
    jmp LegacyGenericPower_54348E
LegacyGenericPower_53D1C9:
    fldz
    jmp LegacyGenericPower_54348E
LegacyGenericPower_53D1D0:
    fld st(1)
    call Cg2dClassifyLegacyPowerExponentX87
    fchs
    test cl, cl
    jnz LegacyGenericPower_53D06F
    fstp st(0)
    fstp st(0)
    fld tbyte ptr [LegacyGenericPowerUndefined592060]
    mov eax, 1
    jmp LegacyGenericPower_53D09A
LegacyGenericPower_54348E:
    cmp word ptr [rsp+32], 027Fh
    jz LegacyGenericPower_543499
    fldcw word ptr [rsp+32]
LegacyGenericPower_543499:
    mov edx, dword ptr [rsp+32]
    add rsp, 232
    ret
Cg2dLegacyPowerCoreX87 ENDP

; 原53D000；ST0=Y/ST1=X，保原FXCH/FSTP X/FST Y与高W32，再真调完整core。
Cg2dLegacyCIPowerX87 PROC FRAME
    sub rsp, 56
    .allocstack 56
    .endprolog
    fxch st(1)
    fstp qword ptr [rsp+32]
    fst qword ptr [rsp+40]
    mov eax, dword ptr [rsp+44]
    lea rcx, [rsp+32]
    call Cg2dLegacyPowerCoreX87
    add rsp, 56
    ret
Cg2dLegacyCIPowerX87 ENDP

; 原53D019普通入口的Y加载/至core顺流承接；RCX借两个F64/RDX借F64输出。
; 只有外部Win64值桥在core返回后FSTP F64；CI/core/帮助体连续ST0不经CPP。
Cg2dComputeLegacyPower PROC FRAME
    sub rsp, 56
    .allocstack 56
    .endprolog
    mov qword ptr [rsp+32], rcx
    mov qword ptr [rsp+40], rdx
    lea rdx, [rcx+8]
    call Cg2dLoadLegacyPowerOperandX87
    mov rcx, qword ptr [rsp+32]
    call Cg2dLegacyPowerCoreX87
    mov rdx, qword ptr [rsp+40]
    fstp qword ptr [rdx]
    add rsp, 56
    ret
Cg2dComputeLegacyPower ENDP

END
