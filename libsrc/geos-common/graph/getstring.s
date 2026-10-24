;
; 2026 cc65 project
;

; void __fastcall__ GetString (char *buffer, char maxLen, char y, unsigned x,
;                               void (*doneHandler)(void), void (*faultHandler)(void));

            .export _GetString
            .import popa, popax

            .include "jumptab.inc"
            .include "geossym.inc"

_GetString:
        sta r4L                 ; faultHandler -- passed in A/X (fastcall, pointer)
        stx r4H
        jsr popax               ; doneHandler
        sta keyVector
        stx keyVector+1
        jsr popax               ; x
        sta r11L
        stx r11H
        jsr popa                ; y
        sta r1H
        jsr popa                ; maxLen
        sta r2L
        jsr popax               ; buffer
        sta r0L
        stx r0H

        lda r4L
        ora r4H
        beq @nofault
        lda #$80
        bne @setflag
@nofault:
        lda #$00
@setflag:
        sta r1L
        jmp GetString
