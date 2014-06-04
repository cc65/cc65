;
; Extended memory driver for the Apple II auxiliary memory
;
; Stefan Haubenthal, 2003-12-12
; Ullrich von Bassewitz, 2002-12-02
;

        .include        "zeropage.inc"

        .include        "em-kernel.inc"
        .include        "em-error.inc"

        .macpack        module

; ------------------------------------------------------------------------
; Header. Includes jump table

        .ifdef  __APPLE2ENH__
        module_header   _a2e_auxmem_emd
        .else
        module_header   _a2_auxmem_emd
        .endif

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

BASE    = $0200
AUXCARD = $C30C                         ; Card signature
AUXMOVE = $C311                         ; Auxiliary move routine
PAGES   = ($C000 - BASE) / 256

; ------------------------------------------------------------------------
; Data.

.data
curpage:        .byte   $FF             ; Current page number (invalid)

.bss
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
        lda     AUXCARD
        and     #$f0
        cmp     #$80
        bne     @L1
        lda     #EM_ERR_OK
        rts
@L1:    lda     #EM_ERR_NO_DEVICE
;       rts

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

MAP:    sta     curpage                 ; Remember the new page

        clc
        adc     #>BASE
        sta     ptr1+1
        ldy     #$00
        sty     ptr1

        lda     #<window
        sta     ptr2
        lda     #>window
        sta     ptr2+1

; Transfer one page

        clc                             ; Direction flag
        jsr     transfer                ; Transfer one page

; Return the memory window

        lda     #<window
        ldx     #>window                ; Return the window address

; Done

done:   rts

; ------------------------------------------------------------------------
; USE: Tell the driver that the window is now associated with a given page.

USE:    sta     curpage                 ; Remember the page
        lda     #<window
        ldx     #>window                ; Return the window
        rts

; ------------------------------------------------------------------------
; COMMIT: Commit changes in the memory window to extended storage.

COMMIT: lda     curpage                 ; Get the current page
        cmp     #$FF
        beq     done                    ; Jump if no page mapped

        clc
        adc     #>BASE
        sta     ptr2+1
        ldy     #$00
        sty     ptr2

        lda     #<window
        sta     ptr1
        lda     #>window
        sta     ptr1+1
        lda     #<$FF
        sta     ptr4
        lda     #>$FF
        sta     ptr4+1
        sec                             ; Direction flag

; Transfer one page/all bytes

transfer:
        php
        clc
        lda     ptr1
        sta     $3C
        adc     ptr4
        sta     $3E
        lda     ptr1+1
        sta     $3D
        adc     ptr4+1
        sta     $3F
        lda     ptr2
        sta     $42
        lda     ptr2+1
        sta     $43
        plp
        jmp     AUXMOVE

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
        clc                             ; Direction flag

common: ldy     #EM_COPY::COUNT
        lda     (ptr3),y                ; Get bytes in last page
        sta     ptr4
        iny
        lda     (ptr3),y                ; Get number of pages
        sta     ptr4+1

        jmp     transfer

; ------------------------------------------------------------------------
; COPYTO: Copy from linear into extended memory. A pointer to a structure
; describing the request is passed in a/x.
; The function must not return anything.
;

COPYTO: sta     ptr3
        stx     ptr3+1                  ; Save the passed em_copy pointer

        ldy     #EM_COPY::OFFS
        lda     (ptr3),y
        sta     ptr2
        ldy     #EM_COPY::PAGE
        lda     (ptr3),y
        clc
        adc     #>BASE
        sta     ptr2+1                  ; To

        ldy     #EM_COPY::BUF
        lda     (ptr3),y
        sta     ptr1
        iny
        lda     (ptr3),y
        sta     ptr1+1                  ; From

        sec                             ; Direction flag
        jmp     common
