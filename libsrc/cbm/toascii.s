;
; Ullrich von Bassewitz, 2009-11-02
;
; unsigned char __fastcall__ toascii (unsigned char c);
; /* Convert a target specific character to ascii */
;

        .export         _toascii

        .macpack        generic

.proc   _toascii

; X must be zero on return

        ldx     #0

; Check for code between 65 and 90

        cmp     #$41
        blt     @L1
        cmp     #$5B
        bge     @L1

; Code between 65 and 90

        ora     #$20
        rts

; Check for code between 193 and 219

@L1:    cmp     #$C1
        blt     @L9
        cmp     #$DC
        bge     @L9

; Code between 193 and 219

        and     #$7F
@L9:    rts

.endproc
