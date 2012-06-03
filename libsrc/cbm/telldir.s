;
; Ullrich von Bassewitz, 2012-06-03
;
; Based on C code by Groepaz
;
; long __fastcall__ telldir (DIR *dir);
;


        .include        "dir.inc"
        .include        "zeropage.inc"


.proc   _telldir

        sta     ptr1
        stx     ptr1+1

; Clear high word of returned value

        lda     #0
        sta     sreg
        sta     sreg+1

; Return dir->off

        ldy     #DIR::off+1
        lda     (ptr1),y
        tax
        dey
        lda     (ptr1),y
        rts

.endproc

