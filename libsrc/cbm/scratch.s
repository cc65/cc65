;
; Ullrich von Bassewitz, 16.11.2002
;
; Scratch a file on disk
;

        .export         scratch
        .import         readdiskerror
        .import         fnunit, fnlen, fncmd

        .include        "cbm.inc"

;--------------------------------------------------------------------------
; scratch: Scratch a file on disk. Expects the name of the file to be already
; parsed. Returns an error code in A, which may either be the code read from
; the command channel, or another error when accessing the command channel
; failed.

.proc   scratch

        lda     #15             ; Command channel
        ldx     fnunit          ; Unit
        tay                     ; Secondary address
        jsr     SETLFS

        lda     #'s'            ; Scratch command
        sta     fncmd
        ldx     fnlen
        inx                     ; Account for "S"
        txa                     ; Length of name into A
        ldx     #<fncmd
        ldy     #>fncmd
        jsr     SETNAM

        jsr     OPEN
        bcs     done

        jsr     readdiskerror   ; Read the command channel

        pha
        lda     #15
        jsr     CLOSE
        pla

done:   rts

.endproc


