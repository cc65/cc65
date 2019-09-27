;
; 2019-09-16, Greg King
;
; unsigned char __fastcall__ revers (unsigned char onoff);
;

        .export         _revers

        .include        "cx16.inc"

.proc   _revers
        ldy     #$00            ; Assume revers off
        tax                     ; Test on/off
        beq     :+              ; Jump if off
        ldy     #$80            ; Load "on" value
        ldx     #>$0000         ; Zero high byte of result
:       lda     RVS             ; Load old value
        sty     RVS             ; Set new value
        clc
        rol     a               ; Convert bit-mask into boolean
        rol     a
        rts
.endproc
