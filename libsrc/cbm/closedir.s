;
; Ullrich von Bassewitz, 2012-05-30
;
; Based on C code by Groepaz
;
; int __fastcall__ closedir(DIR *dir);
;


        .include        "dir.inc"
        .include        "zeropage.inc"

        .import         _close, _free


.proc   _closedir

        sta     ptr1
        stx     ptr1+1

; Load dir->fd

        ldy     #DIR::fd+1
        lda     (ptr1),y
        tax
        dey
        lda     (ptr1),y

; Close the file

        jsr     _close

; Save the error code

        pha
        txa
        pha

; Free the memory block

        lda     ptr1
        ldx     ptr1+1
        jsr     _free

; Return the error code from close()

        pla
        tax
        pla
        rts

.endproc

