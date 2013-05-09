;
; Maciej 'YTM/Elysium' Witkowiak
;
; 27.10.2001
; 06.03.2002

; unsigned char cgetc (void);

            .export _cgetc
            .import cursor, _PromptOff
            .importzp cursor_x, cursor_y

            .include "jumptab.inc"
            .include "geossym.inc"

_cgetc:
; show cursor if needed
        lda cursor
        beq L0

; prepare cursor
        lda #7
        jsr InitTextPrompt
        lda cursor_x
        ldx cursor_x+1
        sta stringX
        stx stringX+1
        lda cursor_y
        sta stringY
        jsr PromptOn

L0:     jsr GetNextChar
        tax
        beq L0
        pha
        jsr _PromptOff
        pla
        ldx #0
        rts
