;
; char __fastcall__ toascii (char c);
; /* Convert a target-specific character to ASCII. */
;

.export _toascii

.proc   _toascii

; .X must be zero, on return.
        ldx     #>$0000
        rts

.endproc
