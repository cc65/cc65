;
; Ullrich von Bassewitz, 2004-06-15
;
; const char* __fastcall__ tgi_geterrormsg (unsigned char code);
; /* Get an error message describing the error in code. */
;

        .include        "tgi-kernel.inc"
        .include        "tgi-error.inc"


.proc   _tgi_geterrormsg

        cmp     #TGI_ERR_COUNT
        bcc     L1
        lda     #TGI_ERR_COUNT          ; "Unknown error"
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
        .byte   <(msg7-msgtab)
        .byte   <(msg8-msgtab)
        .byte   <(msg9-msgtab)
        .byte   <(msg10-msgtab)

msgtab:
msg0:   .asciiz         "No error"
msg1:   .asciiz         "No driver available"
msg2:   .asciiz         "Cannot load driver"
msg3:   .asciiz         "Invalid driver"
msg4:   .asciiz         "Mode not supported by driver"
msg5:   .asciiz         "Invalid function argument"
msg6:   .asciiz         "Function not supported"
msg7:   .asciiz         "Invalid font file"
msg8:   .asciiz         "Out of resources"
msg9:   .asciiz         "Unknown error"
msg10:  .asciiz         "A driver is already installed"

.assert (*-msgtab) < 256, error, "Message table too large"
