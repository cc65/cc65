;
; Ullrich von Bassewitz, 21.06.2002
;
; void __fastcall__ tgi_done (void);
; /* End graphics mode, switch back to text mode. Will NOT unload the driver! */

        .include        "tgi-kernel.inc"
        .include        "tgi-error.inc"

        .export         _tgi_done

_tgi_done:
        lda     _tgi_gmode              ; Is a graphics mode active?
        beq     @L1                     ; Jump if not
        jsr     tgi_done                ; Call the driver routine
        jsr     tgi_geterror            ; Get the error code
        sta     _tgi_error              ; Save it for reference
        cmp     #TGI_ERR_OK
        bne     @L1                     ; Jump if we had an error
        sta     _tgi_gmode              ; Reset the graph mode flag (A = 0)
@L1:    rts



