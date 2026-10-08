OPTION CASEMAP:NONE

.const
ALIGN 8
SoundMaximum54DD98 QWORD 0405FC00000000000h
SoundLogBase54DDA0 QWORD 04059000000000000h
SoundPanScale54F698 QWORD 040B3880000000000h
SoundNegativeScale54DD90 QWORD 0C0C3880000000000h
SoundVolumeScale54DD88 QWORD 040C3880000000000h
SoundOne54D5C0 QWORD 03FF0000000000000h
SoundZero54D5D0 QWORD 00000000000000000h

; 原53B498的局部截断职责：消费ST0，保当前CW，只临时设置RC向零。
; 在当前音频数值帧内展开，不创建通用旧CRT入口；S64只消费低W32。
TruncateSoundIntermediate MACRO
    fstcw word ptr [rsp+40]
    wait
    mov ax, word ptr [rsp+40]
    or ah, 0Ch
    mov word ptr [rsp+42], ax
    fldcw word ptr [rsp+42]
    fistp qword ptr [rsp+48]
    fldcw word ptr [rsp+40]
    mov eax, dword ptr [rsp+48]
    mov edx, dword ptr [rsp+52]
ENDM

.code
PUBLIC Cg2dInitializeSoundTablesX87

; 原52BA38..52BBC8，RCX借128*S32声像表，RDX借128*S32音量表。
; 连续x87中间值/原CW、奇数写点和端点覆盖序保持，不调用F64宿主对数。
Cg2dInitializeSoundTablesX87 PROC FRAME
    push rsi
    .pushreg rsi
    push rdi
    .pushreg rdi
    push rbx
    .pushreg rbx
    sub rsp, 64
    .allocstack 64
    .endprolog
    mov rbx, rcx
    mov r10, rdx

    fld qword ptr [SoundMaximum54DD98]
    lea rsi, [rbx+100h]
SoundPanPositive:
    fld st(0)
    TruncateSoundIntermediate
    test al, 1
    jz SoundPanPositiveNext
    fldln2
    fld qword ptr [SoundLogBase54DDA0]
    fyl2x
    fld st(1)
    fldln2
    fxch st(1)
    fyl2x
    fdiv st(0), st(1)
    fmul qword ptr [SoundPanScale54F698]
    TruncateSoundIntermediate
    fldln2
    mov edi, eax
    fld qword ptr [SoundMaximum54DD98]
    fyl2x
    fdiv st(0), st(1)
    fmul qword ptr [SoundPanScale54F698]
    TruncateSoundIntermediate
    sub eax, edi
    cmp eax, 1388h
    fstp st(0)
    jle SoundPanPositiveWrite
    mov eax, 2710h
SoundPanPositiveWrite:
    mov dword ptr [rsi], eax
    add rsi, 4
SoundPanPositiveNext:
    fsub qword ptr [SoundOne54D5C0]
    fcom qword ptr [SoundZero54D5D0]
    fnstsw ax
    test ah, 1
    jz SoundPanPositive
    fstp st(0)

    fld qword ptr [SoundMaximum54DD98]
    lea rsi, [rbx+100h]
SoundPanNegative:
    fld st(0)
    TruncateSoundIntermediate
    test al, 1
    jz SoundPanNegativeNext
    fldln2
    fld qword ptr [SoundLogBase54DDA0]
    fyl2x
    fld st(1)
    fldln2
    fxch st(1)
    fyl2x
    fdiv st(0), st(1)
    fmul qword ptr [SoundPanScale54F698]
    TruncateSoundIntermediate
    fldln2
    mov edi, eax
    fld qword ptr [SoundMaximum54DD98]
    fyl2x
    fdiv st(0), st(1)
    fmul qword ptr [SoundPanScale54F698]
    TruncateSoundIntermediate
    sub eax, edi
    cmp eax, 1388h
    fstp st(0)
    jle SoundPanNegativeWrite
    mov eax, 2710h
SoundPanNegativeWrite:
    neg eax
    mov dword ptr [rsi], eax
    sub rsi, 4
SoundPanNegativeNext:
    fsub qword ptr [SoundOne54D5C0]
    fcom qword ptr [SoundZero54D5D0]
    fnstsw ax
    test ah, 1
    jz SoundPanNegative
    fstp st(0)

    fld qword ptr [SoundMaximum54DD98]
    fldln2
    mov dword ptr [rbx], 0FFFFD8F0h
    mov dword ptr [rbx+1FCh], 2710h
    fld qword ptr [SoundLogBase54DDA0]
    fyl2x
    fldln2
    fld qword ptr [SoundMaximum54DD98]
    fyl2x
    fdiv st(0), st(1)
    fmul qword ptr [SoundNegativeScale54DD90]
    TruncateSoundIntermediate
    mov edi, eax
SoundVolume:
    fld st(1)
    fldln2
    fxch st(1)
    fyl2x
    fdiv st(0), st(1)
    fmul qword ptr [SoundVolumeScale54DD88]
    TruncateSoundIntermediate
    mov esi, edi
    neg esi
    sub esi, eax
    cmp esi, 2710h
    jle SoundVolumeWrite
    mov esi, 2710h
SoundVolumeWrite:
    fld st(1)
    neg esi
    TruncateSoundIntermediate
    fxch st(1)
    fsub qword ptr [SoundOne54D5C0]
    fxch st(1)
    fld st(1)
    fcomp qword ptr [SoundZero54D5D0]
    mov dword ptr [r10+rax*4], esi
    fnstsw ax
    test ah, 41h
    jz SoundVolume
    fstp st(0)
    TruncateSoundIntermediate
    shl eax, 2
    mov ecx, dword ptr [r10+rax+4]
    mov dword ptr [r10+rax], ecx

    add rsp, 64
    pop rbx
    pop rdi
    pop rsi
    ret
Cg2dInitializeSoundTablesX87 ENDP

END
