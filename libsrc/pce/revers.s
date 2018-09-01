;
; 1998-08-07, Ullrich von Bassewitz
; 2015-11-23, Greg King
;
; unsigned char __fastcall__ revers (unsigned char onoff);
;

        .export         _revers

        .importzp       RVS

.proc   _revers
        cmp     #$01            ; False or true?
        cla
        ror     a               ; Either $00 or $80
        ldy     RVS             ; Load old value
        sta     RVS             ; Set new value
        tya
        asl     a
        rol     a               ; Either $00 or $01
        clx
        rts
.endproc
