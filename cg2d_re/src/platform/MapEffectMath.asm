OPTION CASEMAP:NONE

.const
ALIGN 8
Radians54DD18 QWORD 03F91DF46A2506B91h
PowBase54E238 QWORD 03FF4000000000000h
FixedScale54E220 QWORD 040F0000000000000h
Zero54D5D0 QWORD 0
SpeedScale54E228 DWORD 03A83126Fh
Fallback54E230 DWORD 038D1B717h
SpawnScale54E22C DWORD 044340000h
Percent54D4AC DWORD 042C80000h
Height54D47C DWORD 043F00000h
PercentScale54D4E4 DWORD 03C23D70Ah
HalfHeight54D408 DWORD 043700000h
ArcCenter54DD38 DWORD 042200000h
ArcScale54DD34 DWORD 03D088889h
ActorRate54DD3C DWORD 03FCCCCCDh
ActorRate54DD40 DWORD 03F99999Ah
ActorRate54D4A4 DWORD 040000000h
ActorStep54DD44 DWORD 040800000h
ALIGN 8
ObserverAngles QWORD 04002D97C7F30D173h, 03FF921FB54411744h
    QWORD 03FE921FB54411744h, 0000000000000000h
    QWORD 04015FDBBE9B8F45Ch, 04012D97C7F30D173h
    QWORD 0400F6A7A29515D15h, 0400921FB54411744h

.code
PUBLIC Cg2dMapEffectDirection
PUBLIC Cg2dMapEffectMotion
PUBLIC Cg2dMapEffectDivisor
PUBLIC Cg2dMapEffectStep
PUBLIC Cg2dMapObserverProjection
PUBLIC Cg2dMapObserverArc
PUBLIC Cg2dMapActorVelocity
PUBLIC Cg2dMapMoveDirection

; 所有叶仅借用参数暂存区 [rsp+8,rsp+40)，不改 RSP 或非易失寄存器。
Cg2dMapEffectDirection PROC
    mov dword ptr [rsp+8], ecx
    fnstcw word ptr [rsp+12]
    mov word ptr [rsp+14], 027Fh
    fldcw word ptr [rsp+14]
    mov r10, qword ptr [rsp+40]
    fild dword ptr [rsp+8]
    fmul qword ptr [Radians54DD18]
    fld st(0)
    fcos
    fmul dword ptr [rdx]
    fadd dword ptr [r8]
    fstp qword ptr [r10]
    fld dword ptr [r9]
    fxch st(1)
    fsin
    fmul dword ptr [rdx]
    fsubp st(1), st(0)
    fstp qword ptr [r10+8]
    fldcw word ptr [rsp+12]
    ret
Cg2dMapEffectDirection ENDP

Cg2dMapEffectMotion PROC
    mov dword ptr [rsp+8], edx
    mov dword ptr [rsp+12], r8d
    fnstcw word ptr [rsp+16]
    mov word ptr [rsp+18], 027Fh
    fldcw word ptr [rsp+18]
    fild dword ptr [rsp+12]
    fadd qword ptr [rcx+8]
    fld st(0)
    fstp qword ptr [rsp+24]
    fild dword ptr [rsp+8]
    fadd qword ptr [rcx]
    fld st(0)
    fld st(0)
    fmul st(0), st(1)
    fld qword ptr [rsp+24]
    fmul qword ptr [rsp+24]
    faddp st(1), st(0)
    fsqrt
    fstp qword ptr [r9]
    fstp st(0)
    xor ecx, ecx
    fcom qword ptr [Zero54D5D0]
    fnstsw ax
    test ah, 041h
    jnz MotionXNotPositive
    mov ecx, 1
MotionXNotPositive:
    fcomp qword ptr [Zero54D5D0]
    fnstsw ax
    test ah, 1
    jz MotionXDone
    mov ecx, -1
MotionXDone:
    mov dword ptr [r9+8], ecx
    xor ecx, ecx
    fcom qword ptr [Zero54D5D0]
    fnstsw ax
    test ah, 041h
    jnz MotionYNotPositive
    mov ecx, 1
MotionYNotPositive:
    fcomp qword ptr [Zero54D5D0]
    fnstsw ax
    test ah, 1
    jz MotionYDone
    mov ecx, -1
MotionYDone:
    mov dword ptr [r9+12], ecx
    fldcw word ptr [rsp+16]
    ret
Cg2dMapEffectMotion ENDP

Cg2dMapEffectDivisor PROC
    mov dword ptr [rsp+8], ecx
    fnstcw word ptr [rsp+12]
    mov word ptr [rsp+14], 027Fh
    mov word ptr [rsp+16], 0E7Fh
    fldcw word ptr [rsp+14]
    fild dword ptr [rsp+8]
    fld qword ptr [PowBase54E238]
    fyl2x
    ; 原 __twoToTOS@5433F0，不将幂结果存回 F64 后再消费。
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
    fmul qword ptr [rdx]
    fcom dword ptr [Zero54D5D0]
    fnstsw ax
    test ah, 040h
    jz DivisorHasMagnitude
    fstp st(0)
    fld dword ptr [Fallback54E230]
DivisorHasMagnitude:
    fld dword ptr [SpawnScale54E22C]
    fdiv st(0), st(1)
    fldcw word ptr [rsp+16]
    fistp qword ptr [rsp+24]
    fldcw word ptr [rsp+14]
    fstp st(0)
    fldcw word ptr [rsp+12]
    mov eax, dword ptr [rsp+24]
    ret
Cg2dMapEffectDivisor ENDP

Cg2dMapEffectStep PROC
    mov dword ptr [rsp+8], ecx
    fnstcw word ptr [rsp+12]
    mov word ptr [rsp+14], 027Fh
    mov word ptr [rsp+16], 0E7Fh
    fldcw word ptr [rsp+14]
    fild dword ptr [rsp+8]
    fmul dword ptr [SpeedScale54E228]
    fld st(0)
    fmul qword ptr [rdx]
    fmul qword ptr [FixedScale54E220]
    fldcw word ptr [rsp+16]
    fistp qword ptr [r8]
    fldcw word ptr [rsp+14]
    fmul qword ptr [rdx+8]
    fmul qword ptr [FixedScale54E220]
    fldcw word ptr [rsp+16]
    fistp qword ptr [r8+8]
    fldcw word ptr [rsp+12]
    ret
Cg2dMapEffectStep ENDP

Cg2dMapObserverProjection PROC
    mov dword ptr [rsp+8], ecx
    fnstcw word ptr [rsp+12]
    mov word ptr [rsp+14], 027Fh
    mov word ptr [rsp+16], 0E7Fh
    fldcw word ptr [rsp+14]
    fild dword ptr [rsp+8]
    fmul dword ptr [Percent54D4AC]
    fidiv dword ptr [rdx]
    fst dword ptr [r9+16]
    fmul dword ptr [Height54D47C]
    fmul dword ptr [PercentScale54D4E4]
    fsub dword ptr [HalfHeight54D408]
    lea r10, ObserverAngles
    movsxd r8, r8d
    fld qword ptr [r10+r8*8]
    fld st(0)
    fcos
    fld st(2)
    fmulp st(1), st(0)
    fldcw word ptr [rsp+16]
    fistp qword ptr [r9]
    fldcw word ptr [rsp+14]
    fsin
    fxch st(1)
    fmulp st(1), st(0)
    fchs
    fldcw word ptr [rsp+16]
    fistp qword ptr [r9+8]
    fldcw word ptr [rsp+12]
    ret
Cg2dMapObserverProjection ENDP

Cg2dMapObserverArc PROC
    fnstcw word ptr [rsp+8]
    mov word ptr [rsp+10], 027Fh
    mov word ptr [rsp+12], 0E7Fh
    fldcw word ptr [rsp+10]
    fld dword ptr [rcx]
    fsub dword ptr [ArcCenter54DD38]
    fld st(0)
    fmul st(0), st(1)
    fmul dword ptr [ArcScale54DD34]
    fldcw word ptr [rsp+12]
    fistp qword ptr [rsp+16]
    fldcw word ptr [rsp+10]
    fstp st(0)
    fldcw word ptr [rsp+8]
    mov rax, qword ptr [rsp+16]
    ret
Cg2dMapObserverArc ENDP

Cg2dMapActorVelocity PROC
    fnstcw word ptr [rsp+8]
    mov word ptr [rsp+10], 027Fh
    fldcw word ptr [rsp+10]
    fld1
    fld dword ptr [rcx+16]
    fcomp dword ptr [Zero54D5D0]
    fnstsw ax
    test ah, 040h
    jnz ActorVelocitySelectRate
    mov eax, dword ptr [rcx+16]
    fstp st(0)
    mov dword ptr [rdx], eax
    jmp ActorVelocityNormalize
ActorVelocitySelectRate:
    movsx eax, word ptr [rcx+12]
    cmp eax, 5
    jle ActorVelocityRateAtMostFive
    fstp st(0)
    fld dword ptr [ActorRate54D4A4]
    jmp ActorVelocityScaleRate
ActorVelocityRateAtMostFive:
    cmp eax, 4
    jl ActorVelocityRateBelowFour
    fstp st(0)
    fld dword ptr [ActorRate54DD3C]
    jmp ActorVelocityScaleRate
ActorVelocityRateBelowFour:
    cmp eax, 2
    jl ActorVelocityScaleRate
    fstp st(0)
    fld dword ptr [ActorRate54DD40]
ActorVelocityScaleRate:
    fild dword ptr [rcx+8]
    fmul st(0), st(1)
    fmul dword ptr [PercentScale54D4E4]
    fstp dword ptr [rdx]
    fstp st(0)
ActorVelocityNormalize:
    mov eax, dword ptr [rcx]
    shl eax, 6
    mov dword ptr [rsp+12], eax
    fild dword ptr [rsp+12]
    fsub dword ptr [rcx+20]
    mov eax, dword ptr [rcx+4]
    shl eax, 6
    mov dword ptr [rsp+12], eax
    fild dword ptr [rsp+12]
    fsub dword ptr [rcx+24]
    fst dword ptr [rsp+20]
    fmul dword ptr [rsp+20]
    fld st(1)
    fmul st(0), st(2)
    faddp st(1), st(0)
    fsqrt
    fst dword ptr [rsp+16]
    fcomp dword ptr [Zero54D5D0]
    fnstsw ax
    test ah, 041h
    jnz ActorVelocityZeroDistance
    fdiv dword ptr [rsp+16]
    fstp dword ptr [rdx+4]
    fld dword ptr [rsp+20]
    fdiv dword ptr [rsp+16]
    fstp dword ptr [rdx+8]
    jmp ActorVelocityStoreStep
ActorVelocityZeroDistance:
    fstp st(0)
    mov dword ptr [rdx+4], 0
    mov dword ptr [rdx+8], 0
ActorVelocityStoreStep:
    fld dword ptr [rdx+4]
    fmul dword ptr [rdx]
    fmul dword ptr [ActorStep54DD44]
    fstp dword ptr [rdx+12]
    fld dword ptr [rdx+8]
    fmul dword ptr [rdx]
    fmul dword ptr [ActorStep54DD44]
    fstp dword ptr [rdx+16]
    fldcw word ptr [rsp+8]
    ret
Cg2dMapActorVelocity ENDP

Cg2dMapMoveDirection PROC
    fnstcw word ptr [rsp+8]
    mov word ptr [rsp+10], 027Fh
    fldcw word ptr [rsp+10]
    fld qword ptr [rcx+8]
    fst dword ptr [rsp+12]
    fmul dword ptr [rsp+12]
    fld qword ptr [rcx]
    fmul qword ptr [rcx]
    faddp st(1), st(0)
    fsqrt
    fst dword ptr [rsp+16]
    fcomp dword ptr [Zero54D5D0]
    fnstsw ax
    test ah, 041h
    jnz MapMoveDirectionZero
    fld qword ptr [rcx]
    fdiv dword ptr [rsp+16]
    fst dword ptr [rdx+8]
    fstp qword ptr [rdx]
    fld dword ptr [rsp+12]
    fdiv dword ptr [rsp+16]
    fstp dword ptr [rdx+12]
    jmp MapMoveDirectionDone
MapMoveDirectionZero:
    fldz
    fstp qword ptr [rdx]
    mov dword ptr [rdx+8], 0
    mov dword ptr [rdx+12], 0
MapMoveDirectionDone:
    fldcw word ptr [rsp+8]
    ret
Cg2dMapMoveDirection ENDP

END
