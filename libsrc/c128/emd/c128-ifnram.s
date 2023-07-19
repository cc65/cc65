;
; Extended memory driver for the C128 Internal Function RAM. Driver works
; without problems when statically linked.
;
; Marco van den Heuvel, 2015-11-30
;

        .include        "zeropage.inc"

        .include        "em-kernel.inc"
        .include        "em-error.inc"
        .include        "c128.inc"

        .macpack        generic
        .macpack        module


; ------------------------------------------------------------------------
; Header. Includes jump table

        module_header   _c128_ifnram_emd

; Driver signature

        .byte   $65, $6d, $64           ; "emd"
        .byte   EMD_API_VERSION         ; EM API version number

; Library reference

        .addr   $0000

; Jump table

        .addr   INSTALL
        .addr   UNINSTALL
        .addr   PAGECOUNT
        .addr   MAP
        .addr   USE
        .addr   COMMIT
        .addr   COPYFROM
        .addr   COPYTO

; ------------------------------------------------------------------------
; Constants

BASE    = $8000
PAGES   = 127      ; Do not touch MMU

; ------------------------------------------------------------------------
; Data.

.bss
curpage:        .res    2               ; Current page number

window:         .res    256             ; Memory "window"

.code

; ------------------------------------------------------------------------
; INSTALL routine. Is called after the driver is loaded into memory. If
; possible, check if the hardware is present and determine the amount of
; memory available.
; Must return an EM_ERR_xx code in a/x.
;

INSTALL:
        ldx     #0
        stx     ptr1
        ldx     #$80
        stx     ptr1+1
        ldx     #<ptr1
        stx     FETVEC
        stx     STAVEC
        ldy     #0
        ldx     #MMU_CFG_IFROM
        sei
        jsr     FETCH
        tax
        inx
        txa
        sta     tmp1
        ldx     #MMU_CFG_IFROM
        sei
        jsr     STASH
        ldx     #MMU_CFG_IFROM
        jsr     FETCH
        cli
        cmp     tmp1
        beq     @ram_present
        lda     #EM_ERR_NO_DEVICE
        ldx     #0 ; return value is char
        rts

@ram_present:
        ldx     #$FF
        stx     curpage
        stx     curpage+1               ; Invalidate the current page
        .assert EM_ERR_OK = 0, error
        inx
        txa
;       rts                             ; Run into UNINSTALL instead

; ------------------------------------------------------------------------
; UNINSTALL routine. Is called before the driver is removed from memory.
; Can do cleanup or whatever. Must not return anything.
;

UNINSTALL:
        rts


; ------------------------------------------------------------------------
; PAGECOUNT: Return the total number of available pages in a/x.
;

PAGECOUNT:
        lda     #<PAGES
        ldx     #>PAGES
        rts

; ------------------------------------------------------------------------
; MAP: Map the page in a/x into memory and return a pointer to the page in
; a/x. The contents of the currently mapped page (if any) may be discarded
; by the driver.
;

MAP:    sta     curpage
        stx     curpage+1               ; Remember the new page

        clc
        adc     #>BASE
        sta     ptr1+1
        ldy     #$00
        sty     ptr1

        lda     #<ptr1
        sta     FETVEC
        sei

; Transfer one page

@L1:    ldx     #MMU_CFG_IFROM
        jsr     FETCH
        sta     window,y
        iny
        bne     @L1

; Return the memory window

        cli
        lda     #<window
        ldx     #>window                ; Return the window address
        rts

; ------------------------------------------------------------------------
; USE: Tell the driver that the window is now associated with a given page.

USE:    sta     curpage
        stx     curpage+1               ; Remember the page
        lda     #<window
        ldx     #>window                ; Return the window
        rts

; ------------------------------------------------------------------------
; COMMIT: Commit changes in the memory window to extended storage.

COMMIT: lda     curpage                 ; Get the current page
        ldx     curpage+1
        bmi     done                    ; Jump if no page mapped

        clc
        adc     #>BASE
        sta     ptr1+1
        ldy     #$00
        sty     ptr1

        lda     #<ptr1
        sta     STAVEC
        sei

; Transfer one page. Y must be zero on entry

@L1:    lda     window,y
        ldx     #MMU_CFG_IFROM
        jsr     STASH
        iny
        bne     @L1
        cli

; Done

done:   rts

; ------------------------------------------------------------------------
; COPYFROM: Copy from extended into linear memory. A pointer to a structure
; describing the request is passed in a/x.
; The function must not return anything.
;

COPYFROM:
        sta     ptr3
        stx     ptr3+1                  ; Save the passed em_copy pointer

        ldy     #EM_COPY::OFFS
        lda     (ptr3),y
        sta     ptr1
        ldy     #EM_COPY::PAGE
        lda     (ptr3),y
        clc
        adc     #>BASE
        sta     ptr1+1                  ; From

        ldy     #EM_COPY::BUF
        lda     (ptr3),y
        sta     ptr2
        iny
        lda     (ptr3),y
        sta     ptr2+1                  ; To

        lda     #<ptr1
        sta     FETVEC

        ldy     #EM_COPY::COUNT+1
        lda     (ptr3),y                ; Get number of pages
        beq     @L2                     ; Skip if no full pages
        sta     tmp1

; Copy full pages

        ldy     #$00
        sei
@L1:    ldx     #MMU_CFG_IFROM
        jsr     FETCH
        sta     (ptr2),y
        iny
        bne     @L1
        inc     ptr1+1
        inc     ptr2+1
        dec     tmp1
        bne     @L1

; Copy the remainder of the page

@L2:    ldy     #EM_COPY::COUNT
        lda     (ptr3),y                ; Get bytes in last page
        beq     @L4
        sta     tmp1

        ldy     #$00
@L3:    ldx     #MMU_CFG_IFROM
        jsr     FETCH
        sta     (ptr2),y
        iny
        dec     tmp1
        bne     @L3
; Done

@L4:    cli
        rts

; ------------------------------------------------------------------------
; COPYTO: Copy from linear into extended memory. A pointer to a structure
; describing the request is passed in a/x.
; The function must not return anything.
;

COPYTO: sta     ptr3
        stx     ptr3+1                  ; Save the passed em_copy pointer

        ldy     #EM_COPY::OFFS
        lda     (ptr3),y
        sta     ptr1
        ldy     #EM_COPY::PAGE
        lda     (ptr3),y
        clc
        adc     #>BASE
        sta     ptr1+1                  ; To

        ldy     #EM_COPY::BUF
        lda     (ptr3),y
        sta     ptr2
        iny
        lda     (ptr3),y
        sta     ptr2+1                  ; From

        lda     #<ptr1
        sta     STAVEC

        ldy     #EM_COPY::COUNT+1
        lda     (ptr3),y                ; Get number of pages
        beq     @L2                     ; Skip if no full pages
        sta     tmp1

; Copy full pages

        sei
        ldy     #$00
@L1:    lda     (ptr2),y
        ldx     #MMU_CFG_IFROM
        jsr     STASH
        iny
        bne     @L1
        inc     ptr1+1
        inc     ptr2+1
        dec     tmp1
        bne     @L1

; Copy the remainder of the page

@L2:    ldy     #EM_COPY::COUNT
        lda     (ptr3),y                ; Get bytes in last page
        beq     @L4
        sta     tmp1

        ldy     #$00
@L3:    lda     (ptr2),y
        ldx     #MMU_CFG_IFROM
        jsr     STASH
        iny
        dec     tmp1
        bne     @L3

; Done

@L4:    cli
        rts
