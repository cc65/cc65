;
; Ullrich von Bassewitz, 21.06.2002
;
; void tgi_done (void);
; /* End graphics mode, switch back to text mode. Will NOT unload the driver! */

        .include        "tgi-kernel.inc"
        .include        "tgi-error.inc"

.proc   _tgi_done

        lda     _tgi_gmode              ; Is a graphics mode active?
        beq     @L1                     ; Jump if not
        jsr     tgi_done                ; Call the driver routine
        lda     #$00
        sta     _tgi_gmode              ; Reset the graph mode flag
@L1:    rts

.endproc
