;
; unsigned char __fastcall__ toascii (unsigned char c);
; /* Convert a target specific character to ascii */
;

.export _toascii

.proc _toascii

; X must be zero on return
        ldx     #0

; Done!
        rts

.endproc
