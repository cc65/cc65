;
; Ullrich von Bassewitz, 21.06.2002
;
; void __fastcall__ tgi_done (void);
; /* End graphics mode, switch back to text mode. Will NOT unload the driver! */

        .include        "tgi-kernel.inc"

        .export         _tgi_done

_tgi_done:
        lda     _tgi_mode               ; Is a graphics mode active?
        beq     @L1                     ; Jump if not
        jsr     tgi_done                ; Call the driver routine
        jsr     tgi_fetch_error         ; Get the error code
        bne     @L1                     ; Jump if we had an error
        sta     _tgi_mode               ; Reset the current mode (A = 0)
@L1:    rts



