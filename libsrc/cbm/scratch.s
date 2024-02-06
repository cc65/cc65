;
; Ullrich von Bassewitz, 2002-11-16, 2009-02-22
;
; Scratch a file on disk
;

        .export         scratch
        .import         opencmdchannel, closecmdchannel, writefndiskcmd
        .import         fnunit, fncmd

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
        jsr     writefndiskcmd

        pha
        ldx     fnunit
        jsr     closecmdchannel
        pla

done:   rts

.endproc


