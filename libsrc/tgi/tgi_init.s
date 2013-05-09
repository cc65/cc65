;
; Ullrich von Bassewitz, 21.06.2002
;
; void tgi_init (void);
; /* Initialize the already loaded graphics driver */


        .include        "tgi-kernel.inc"
        .include        "tgi-error.inc"

        .import         pushax, pusha, decax1
        .importzp       ptr1


;----------------------------------------------------------------------------

.code
.proc   _tgi_init

        jsr     _tgi_done               ; Switch off graphics if needed
        jsr     tgi_init                ; Go into graphics mode
        jsr     tgi_geterror            ; Get the error code
        sta     _tgi_error              ; Save for later reference
        cmp     #TGI_ERR_OK
        bne     @L9                     ; Jump on error

        inc     _tgi_gmode              ; Remember that graph mode is active

; Get the maximum X and Y coordinate

        jsr     _tgi_getxres
        jsr     decax1
        sta     _tgi_xmax
        stx     _tgi_xmax+1

        jsr     _tgi_getyres
        jsr     decax1
        sta     _tgi_ymax
        stx     _tgi_ymax+1

; Do driver initialization. Set draw and view pages.

        lda     #0
        jsr     tgi_setviewpage
        lda     #0
        jsr     tgi_setdrawpage

; Set the default palette.

        jsr     tgi_getdefpalette       ; Get the default palette into A/X
        sta     ptr1
        stx     ptr1+1                  ; Save it
        jsr     tgi_setpalette          ; Set the default palette.
        jsr     tgi_geterror            ; Clear a possible error code

; Set the drawing color to white

@L1:    lda     #tgi_color_white
        jsr     _tgi_setcolor           ; tgi_setcolor (TGI_COLOR_WHITE);

; Set the text style

        lda     #<$100
        ldx     #>$100
        jsr     pushax                  ; Width scale = 1.0
        jsr     pushax                  ; Heigh scale = 1.0
        jsr     pusha                   ; Text direction = TGI_TEXT_HORIZONTAL
        jmp     _tgi_settextstyle       ; A = Font = TGI_FONT_BITMAP

; Error exit

@L9:    rts

.endproc
