;
; Colin Leroy-Mira <colin@colino.net>, 2024
;
; void __fastcall__ detect_iigs(void)
;

        .export         _detect_iigs
        .import         ostype, return0, return1

        .include        "apple2.inc"

        ; Returns 1 if running on IIgs, 0 otherwise
_detect_iigs:
        lda     ostype
        bpl     :+
        jmp     return1
:       jmp     return0
