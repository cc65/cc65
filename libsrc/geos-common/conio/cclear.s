;
; Maciej 'YTM/Elysium' Witkowiak
;
; 06.03.2002

; void cclearxy (unsigned char x, unsigned char y, unsigned char length);
; void cclear (unsigned char length);

            .export _cclearxy, _cclear
            .import popa, _gotoxy, fixcursor
            .importzp cursor_x, cursor_y, cursor_c

            .include "jumptab.inc"
            .include "geossym.inc"

_cclearxy:
        pha                     ; Save the length
        jsr popa                ; Get y
        jsr _gotoxy             ; Call this one, will pop params
        pla                     ; Restore the length

_cclear:
        cmp #0                  ; Is the length zero?
        beq L9                  ; Jump if done
        tax
        lda cursor_x            ; left start
        sta r3L
        lda cursor_x+1
        sta r3L+1
        lda cursor_y            ; level
        sta r2L
        clc
        adc #7
        sta r2H
        txa                     ; right end
        clc
        adc cursor_c
        sta cursor_c
        sta r4L
        ldx #r4
        ldy #3
        jsr DShiftLeft
        clc                     ; one pixel less
        lda r4L
        sbc #0
        sta r4L
        lda r4L+1
        sbc #0
        sta r4L+1
        lda curPattern          ; store current pattern
        pha
        lda #0                  ; set pattern to clear
        jsr SetPattern
        jsr Rectangle
        pla
        jsr SetPattern          ; restore pattern
        jsr fixcursor
L9:     rts
