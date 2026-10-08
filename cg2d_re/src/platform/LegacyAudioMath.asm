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
; 原空间音效三个F32常量，使用位样本，不从宿主F64生成。
SpatialGrid64_54DD30 DWORD 042800000h
SpatialScreen320_54F6A0 DWORD 043A00000h
SpatialScreen240_54D408 DWORD 043700000h

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
PUBLIC Cg2dInitializeFixedMusicVolumeX87
PUBLIC Cg2dComputeSpatialSoundX87
PUBLIC Cg2dComputeSpatialScanQuotientX87
PUBLIC Cg2dScaleSpatialSourceX87
PUBLIC Cg2dPublishSpatialProjectionX87

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

; 原454E49..454EB8及454EBC，RCX借独立128*S32固定音乐音量表。
; S32计数含正常0槽，原S64截断后只取低W32，不特化log(0)或设默认值。
Cg2dInitializeFixedMusicVolumeX87 PROC FRAME
    push rsi
    .pushreg rsi
    push rdi
    .pushreg rdi
    sub rsp, 72
    .allocstack 72
    .endprolog
    mov r11, rcx

    fldln2
    mov dword ptr [rsp+32], 7Fh
    fld qword ptr [SoundLogBase54DDA0]
    fyl2x
    fldln2
    fld qword ptr [SoundMaximum54DD98]
    fyl2x
    fdiv st(0), st(1)
    fmul qword ptr [SoundNegativeScale54DD90]
    TruncateSoundIntermediate
    mov edi, eax
    lea rsi, [r11+1FCh]
FixedMusicVolume:
    fldln2
    fild dword ptr [rsp+32]
    fyl2x
    fdiv st(0), st(1)
    fmul qword ptr [SoundVolumeScale54DD88]
    TruncateSoundIntermediate
    mov ecx, edi
    neg ecx
    sub ecx, eax
    cmp ecx, 2710h
    jle FixedMusicVolumeWrite
    mov ecx, 2710h
FixedMusicVolumeWrite:
    mov edx, dword ptr [rsp+32]
    sub rsi, 4
    neg ecx
    mov dword ptr [rsi+4], ecx
    dec edx
    cmp rsi, r11
    mov dword ptr [rsp+32], edx
    jge FixedMusicVolume
    fstp st(0)

    add rsp, 72
    pop rdi
    pop rsi
    ret
Cg2dInitializeFixedMusicVolumeX87 ENDP

; 原52CA07..52CA4E：RCX/RDX为S32坐标，R8同步借本地S32距离。
; 浮点平方在FILD之后；返回原W32左移及有符号常数除法的S32商。
Cg2dComputeSpatialSoundX87 PROC FRAME
    push rsi
    .pushreg rsi
    sub rsp, 64
    .allocstack 64
    .endprolog
    mov r9d, ecx
    mov ecx, edx

    add ecx, -0F0h
    lea eax, [r9-140h]
    mov dword ptr [rsp+32], eax
    fild dword ptr [rsp+32]
    mov dword ptr [rsp+32], ecx
    fild dword ptr [rsp+32]
    fld st(0)
    fmul st(0), st(1)
    fld st(2)
    fmul st(0), st(3)
    faddp st(1), st(0)
    fsqrt
    TruncateSoundIntermediate
    mov esi, eax
    mov eax, 4D4873EDh
    mov ecx, esi
    shl ecx, 4
    imul ecx
    fstp st(0)
    fstp st(0)
    sar edx, 5
    mov eax, edx
    shr eax, 1Fh
    add edx, eax

    mov dword ptr [r8], esi
    mov eax, edx
    add rsp, 64
    pop rsi
    ret
Cg2dComputeSpatialSoundX87 ENDP

; 原52CD77..52CDA6：RCX/RDX为已取样S32坐标；先W32平方和，再FILD。
; 仅返回S32距离商，扫描阈值/原请求调用及标记仍由既有CPP消费。
Cg2dComputeSpatialScanQuotientX87 PROC FRAME
    sub rsp, 72
    .allocstack 72
    .endprolog
    mov r9d, ecx

    mov eax, edx
    mov ecx, r9d
    imul eax, edx
    imul ecx, r9d
    add eax, ecx
    mov dword ptr [rsp+32], eax
    fild dword ptr [rsp+32]
    fsqrt
    TruncateSoundIntermediate
    mov ecx, eax
    mov eax, 4D4873EDh
    shl ecx, 4
    imul ecx
    sar edx, 5
    mov eax, edx
    shr eax, 1Fh
    add edx, eax

    mov eax, edx
    add rsp, 72
    ret
Cg2dComputeSpatialScanQuotientX87 ENDP

; 原52CDE0..52CE05中的六条数值指令；RCX/RDX为S32 X/Y。
; R8/R9同步借本地F32世界X/Y，原当前CW下先写Y，再写X。
Cg2dScaleSpatialSourceX87 PROC FRAME
    sub rsp, 40
    .allocstack 40
    .endprolog
    mov dword ptr [rsp+32], ecx
    mov dword ptr [rsp+36], edx

    fild dword ptr [rsp+36]
    fmul dword ptr [SpatialGrid64_54DD30]
    fstp dword ptr [r9]
    fild dword ptr [rsp+32]
    fmul dword ptr [SpatialGrid64_54DD30]
    fstp dword ptr [r8]

    add rsp, 40
    ret
Cg2dScaleSpatialSourceX87 ENDP

; 原52CE0D..52CE8A，RCX借唯一50*20B表，EDX为原W32索引。
; R8/R9借本地屏幕F32；只拓宽寻址，三个截断和字段发布顺序保持。
Cg2dPublishSpatialProjectionX87 PROC FRAME
    push rsi
    .pushreg rsi
    push rdi
    .pushreg rdi
    sub rsp, 72
    .allocstack 72
    .endprolog
    mov r11, rcx
    movsxd r10, edx

    fld dword ptr [r8]
    fsub dword ptr [SpatialScreen320_54F6A0]
    fld dword ptr [r9]
    fsub dword ptr [SpatialScreen240_54D408]
    fld st(0)
    fmul st(0), st(1)
    fld st(2)
    fmul st(0), st(3)
    faddp st(1), st(0)
    fsqrt
    TruncateSoundIntermediate
    mov edi, eax
    mov eax, r10d
    fstp st(0)
    lea rsi, [r10+r10*4]
    shl rsi, 2
    fstp st(0)
    mov ecx, dword ptr [r11+rsi]
    test ecx, ecx
    jnz SpatialProjectionPublish
    cmp dword ptr [r11+rsi+4], -1
    jz SpatialProjectionPublish
    cmp edi, dword ptr [r11+rsi+10h]
    jge SpatialProjectionReturn
SpatialProjectionPublish:
    fld dword ptr [r8]
    mov dword ptr [r11+rsi+4], eax
    TruncateSoundIntermediate
    fld dword ptr [r9]
    mov dword ptr [r11+rsi+8], eax
    TruncateSoundIntermediate
    mov dword ptr [r11+rsi+0Ch], eax
    mov dword ptr [r11+rsi+10h], edi
    mov dword ptr [r11+rsi], 0
SpatialProjectionReturn:
    add rsp, 72
    pop rdi
    pop rsi
    ret
Cg2dPublishSpatialProjectionX87 ENDP

END
