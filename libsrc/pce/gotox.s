;
; void __fastcall__ gotox (unsigned char x);
;

        .export         _gotox

        .import         plot

        .include        "pce.inc"
        .include        "extzp.inc"

_gotox:
        sta     CURS_X          ; Set X
        jmp     plot            ; Set the cursor position
