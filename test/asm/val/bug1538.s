
        .macpack longbranch

        .import _exit
        .export _main

offset1 = ($123456 + ((.bank(_main)) * $2345))   ; should produce $123456 + ($23 * $2345) = 1706c5

_main:
        ldx #1  ; test counter

        ;---------------------------------------------------------------------
        ; first check if we can still force an 16bit address although we are
        ; ANDing with a 8bit value
test1:
test1a: bit a:$1234 & $ff   ; should produce 00 34
        lda test1a+1
        cmp #$34
        jne exiterror

        inx
        lda test1a+2
        cmp #$00
        jne exiterror

        inx
test1b: bit a:$34 & $ff     ; should produce 00 34
        lda test1b+1
        cmp #$34
        jne exiterror

        inx
        lda test1b+2
        cmp #$00
        jne exiterror

        ;---------------------------------------------------------------------
        ; the original bug report, use an expression that contains a non constant
        ; part (.bank(_main)) and limit it to 8bit size by ANDing with $ff
test2:
        inx
        lda #(offset1 >> 0) & $ff
        cmp #$c5
        jne exiterror

        inx
        lda #(offset1 >> 8) & $ff
        cmp #$06
        jne exiterror

        inx
        lda #(offset1 >> 16) & $ff
        cmp #$17
        jne exiterror

        ;---------------------------------------------------------------------
        ; import a zeropage symbol (1 byte size) and then AND with a constant
        ; 16bit value - it should not turn into a 16bit address
test3:
        jmp test3chk
        .importzp tmp1
test3a: .byte tmp1              ; verify its 8bit
test3b: .byte tmp1 & $ff        ; AND with $ff should work of course
test3c: .byte tmp1 & $ffff      ; AND with $ffff should not change size
test3d: .byte tmp1 & $ffffff    ; AND with $ffffff should not change size
test3e: .byte tmp1 & $ffffffff  ; AND with $ffffffff should not change size
test3f: .word tmp1 & $ffffff    ; AND with $ffffff should not change size
test3g: .word tmp1 & $ffffffff  ; AND with $ffffffff should not change size
test3chk:
        inx
        lda test3a
        cmp #tmp1
        jne exiterror

        inx
        lda test3b
        cmp #tmp1
        jne exiterror

        inx
        lda test3c
        cmp #tmp1
        jne exiterror

        inx
        lda test3d
        cmp #tmp1
        jne exiterror

        inx
        lda test3e
        cmp #tmp1
        jne exiterror

        inx
        lda test3f
        cmp #tmp1
        jne exiterror

        inx
        lda test3f+1
        cmp #$00
        jne exiterror

        inx
        lda test3g
        cmp #tmp1
        jne exiterror

        inx
        lda test3g+1
        cmp #$00
        jne exiterror

        ;---------------------------------------------------------------------
        ; exit OK
        ldx #0
exiterror:
        txa
        ldx #0
        jmp _exit

