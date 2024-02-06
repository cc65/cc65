;
; Ullrich von Bassewitz, 2004-11-07
;
; const char* __fastcall__ mouse_geterrormsg (unsigned char code);
; /* Get an error message describing the error in code. */
;

        .include        "mouse-kernel.inc"


.proc   _mouse_geterrormsg

        cmp     #MOUSE_ERR_COUNT
        bcc     L1
        lda     #MOUSE_ERR_COUNT        ; "Unknown error"
L1:     tay
        ldx     #>msgtab
        lda     #<msgtab
        clc
        adc     offs,y
        bcc     L2
        inx
L2:     rts

.endproc


;----------------------------------------------------------------------------
; Error messages. The messages are currently limited to 256 bytes total.

.rodata

offs:   .byte   <(msg0-msgtab)
        .byte   <(msg1-msgtab)
        .byte   <(msg2-msgtab)
        .byte   <(msg3-msgtab)
        .byte   <(msg4-msgtab)
        .byte   <(msg5-msgtab)
        .byte   <(msg6-msgtab)

msgtab:
msg0:   .asciiz         "No error"
msg1:   .asciiz         "No driver available"
msg2:   .asciiz         "Cannot load driver"
msg3:   .asciiz         "Invalid driver"
msg4:   .asciiz         "Mouse hardware not found"
msg5:   .asciiz         "Invalid ioctl code"
msg6:   .asciiz         "Unknown error"

