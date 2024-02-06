;
; void __fastcall__ gotoy (unsigned char y);
;

        .export         _gotoy

        .import         plot

        .include        "pce.inc"
        .include        "extzp.inc"

_gotoy:
        sta     CURS_Y          ; Set Y
        jmp     plot            ; Set the cursor position
