;
; Ullrich von Bassewitz, 21.06.2002
;
; void __fastcall__ tgi_init (void);
; /* Initialize the already loaded graphics driver */


        .include        "tgi-kernel.inc"
        .include        "tgi-error.inc"

        .importzp       ptr1
        .import         _tgi_done
        .import         _tgi_setcolor
        .export         _tgi_init

_tgi_init:
        jsr     _tgi_done               ; Switch off graphics if needed
        jsr     tgi_init                ; Go into graphics mode
        jsr     tgi_geterror            ; Get the error code
        sta     _tgi_error              ; Save for later reference
        cmp     #TGI_ERR_OK
        bne     @L9                     ; Jump on error

        inc     _tgi_gmode              ; Remember that graph mode is active

; Do driver initialization. First set the default palette.

        jsr     tgi_getdefpalette       ; Get the default palette into A/X
        sta     ptr1
        stx     ptr1+1
        ora     ptr1+1                  ; Do we have a default palette?
        beq     @L1                     ; Jump if no
        jsr     tgi_setpalette          ; Set the default palette

; Set the drawing color to the maximum color

@L1:    ldx     _tgi_colorcount
        dex
        txa
        jsr     _tgi_setcolor           ; tgi_setcolor (tgi_getmaxcolor ());

; Set the text style

        lda     #TGI_TEXT_HORIZONTAL
        sta     _tgi_textdir
        ldx     #1
        stx     _tgi_textmagx
        ldy     #1
        sty     _tgi_textmagy
        jsr     tgi_textstyle           ; Tell the driver about the text style

; Clear the screen

        jmp     tgi_clear

; Error exit

@L9:    rts

