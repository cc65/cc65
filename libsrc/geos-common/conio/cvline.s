;
; Maciej 'YTM/Elysium' Witkowiak
;
; 06.03.2002

; void cvlinexy (unsigned char x, unsigned char y, unsigned char length);
; void cvline (unsigned char length);

            .export _cvlinexy, _cvline
            .import popa, _gotoxy, fixcursor
            .importzp cursor_x, cursor_y, cursor_r

            .include "jumptab.inc"
            .include "geossym.inc"

_cvlinexy:
        pha                     ; Save the length
        jsr popa                ; Get y
        jsr _gotoxy             ; Call this one, will pop params
        pla                     ; Restore the length

_cvline:    
        cmp #0                  ; Is the length zero?
        beq L9                  ; Jump if done
        tax
        lda cursor_x            ; x position
        clc
        adc #3                  ; in the middle of cell
        sta r4L
        lda cursor_x+1
        adc #0
        sta r4L+1
        lda cursor_y            ; top start
        sta r3L
        txa                     ; bottom end
        clc
        adc cursor_r
        sta cursor_r
        asl a
        asl a
        asl a
        clc                     ; one pixel less
        sbc #0
        sta r3H
        lda #%11111111          ; pattern
        jsr VerticalLine
        jsr fixcursor
L9:     rts
