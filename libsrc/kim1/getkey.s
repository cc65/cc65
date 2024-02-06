;
; int __fastcall__ getkey();
;

.include        "kim1.inc"

.import         popa

.export         _getkey

.proc           _getkey

        jsr     KEYIN           ; Open up keyboard channel
        jsr     GETKEY          ; Get key code
        ldx     #0              ; MSB of return value is zero
        rts

.endproc
