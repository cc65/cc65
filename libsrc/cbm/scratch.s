;
; Ullrich von Bassewitz, 16.11.2002
;
; Scratch a file on disk
;

        .export         scratch
        .import         opencmdchannel, closecmdchannel, writediskcmd
        .import         fnunit, fnlen, fncmd
        .importzp       ptr1

        .include        "cbm.inc"

;--------------------------------------------------------------------------
; scratch: Scratch a file on disk. Expects the name of the file to be already
; parsed. Returns an error code in A, which may either be the code read from
; the command channel, or another error when accessing the command channel
; failed.

.proc   scratch

        ldx     fnunit
        jsr     opencmdchannel
        bne     done

        lda     #'s'            ; Scratch command
        sta     fncmd

        lda     #<fncmd
        sta     ptr1
        lda     #>fncmd
        sta     ptr1+1

        ldx     fnlen
        inx                     ; Account for "S"
        txa                     ; Length of name into A
        ldx     fnunit          ; Unit
        jsr     writediskcmd

        pha
        ldx     fnunit
        jsr     closecmdchannel
        pla

done:   rts

.endproc


