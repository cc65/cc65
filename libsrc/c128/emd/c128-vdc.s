;
; Extended memory driver for the VDC RAM available on all C128 machines
; (based on code by Ullrich von Bassewitz)
; Maciej 'YTM/Elysium' Witkowiak <ytm@elysium.pl>
; 06,20.12.2002

        .include        "zeropage.inc"

        .include        "em-kernel.inc"
        .include        "em-error.inc"

        .macpack        generic
        .macpack        module


; ------------------------------------------------------------------------
; Header. Includes jump table

        module_header   _c128_vdc_emd

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

VDC_ADDR_REG      = $D600                 ; VDC address
VDC_DATA_REG      = $D601                 ; VDC data

VDC_DATA_HI       = 18                    ; used registers
VDC_DATA_LO       = 19
VDC_CSET          = 28
VDC_DATA          = 31

; ------------------------------------------------------------------------
; Data.

.data

pagecount:      .word  64                  ; $0000-$3fff as 16k default
curpage:        .word  $ffff               ; currently mapped-in page (invalid)

.bss

window:         .res    256                ; memory window

.code

; ------------------------------------------------------------------------
; INSTALL routine. Is called after the driver is loaded into memory. If
; possible, check if the hardware is present and determine the amount of
; memory available.
; Must return an EM_ERR_xx code in a/x.
;

INSTALL:
        ; do test for VDC presence here???

        ldx     #VDC_CSET       ; determine size of RAM...
        jsr     vdcgetreg
        sta     tmp1
        ora     #%00010000
        jsr     vdcputreg       ; turn on 64k

        jsr     settestadr1     ; save original value of test byte
        jsr     vdcgetbyte
        sta     tmp2

        lda     #$55            ; write $55 here
        ldy     #ptr1
        jsr     test64k         ; read it here and there
        lda     #$aa            ; write $aa here
        ldy     #ptr2
        jsr     test64k         ; read it here and there

        jsr     settestadr1
        lda     tmp2
        jsr     vdcputbyte      ; restore original value of test byte

        lda     ptr1            ; do bytes match?
        cmp     ptr1+1
        bne     @have64k
        lda     ptr2
        cmp     ptr2+1
        bne     @have64k

        ldx     #VDC_CSET
        lda     tmp1
        jsr     vdcputreg       ; restore 16/64k flag
        jmp     @endok          ; and leave default values for 16k

@have64k:
        lda     #<256
        ldx     #>256
        sta     pagecount
        stx     pagecount+1
@endok:
        lda     #<EM_ERR_OK
        ldx     #>EM_ERR_OK
        rts

test64k:
        sta     tmp1
        sty     ptr3
        lda     #0
        sta     ptr3+1
        jsr     settestadr1
        lda     tmp1
        jsr     vdcputbyte              ; write $55
        jsr     settestadr1
        jsr     vdcgetbyte              ; read here
        pha
        jsr     settestadr2
        jsr     vdcgetbyte              ; and there
        ldy     #1
        sta     (ptr3),y
        pla
        dey
        sta     (ptr3),y
        rts

settestadr1:
        ldy     #$02                    ; test page 2 (here)
        .byte   $2c
settestadr2:
        ldy     #$42                    ; or page 64+2 (there)
        lda     #0
        jmp     vdcsetsrcaddr

; ------------------------------------------------------------------------
; UNINSTALL routine. Is called before the driver is removed from memory.
; Can do cleanup or whatever. Must not return anything.
;

UNINSTALL:
        ;on C128 restore font and clear the screen?
        rts

; ------------------------------------------------------------------------
; PAGECOUNT: Return the total number of available pages in a/x.
;

PAGECOUNT:
        lda     pagecount
        ldx     pagecount+1
        rts

; ------------------------------------------------------------------------
; MAP: Map the page in a/x into memory and return a pointer to the page in
; a/x. The contents of the currently mapped page (if any) may be discarded
; by the driver.
;

MAP:    sta     curpage
        stx     curpage+1
        sta     ptr1+1
        ldy     #0
        sty     ptr1

        lda     #<window
        sta     ptr2
        lda     #>window
        sta     ptr2+1

        jsr     transferin

        lda     #<window
        ldx     #>window
        rts

; copy a single page from (ptr1):VDCRAM to (ptr2):RAM

transferin:
        lda     ptr1
        ldy     ptr1+1
        jsr     vdcsetsrcaddr           ; set source address in VDC
        ldy     #0
        ldx     #VDC_DATA
        stx     VDC_ADDR_REG
@L0:    bit     VDC_ADDR_REG
        bpl     @L0
        lda     VDC_DATA_REG            ; get 2 bytes at a time to speed-up
        sta     (ptr2),y                ; (in fact up to 8 bytes could be fetched with special VDC config)
        iny
        lda     VDC_DATA_REG
        sta     (ptr2),y
        iny
        bne     @L0
        rts

; ------------------------------------------------------------------------
; USE: Tell the driver that the window is now associated with a given page.

USE:    sta     curpage
        stx     curpage+1               ; Remember the page
        lda     #<window
        ldx     #>window                ; Return the window
done:   rts

; ------------------------------------------------------------------------
; COMMIT: Commit changes in the memory window to extended storage.

COMMIT:
        lda     curpage                 ; jump if no page mapped
        ldx     curpage+1
        bmi     done
        sta     ptr1+1
        ldy     #0
        sty     ptr1

        lda     #<window
        sta     ptr2
        lda     #>window
        sta     ptr2+1

; fall through to transferout

; copy a single page from (ptr2):RAM to (ptr1):VDCRAM

transferout:
        lda     ptr1
        ldy     ptr1+1
        jsr     vdcsetsrcaddr           ; set source address in VDC
        ldy     #0
        ldx     #VDC_DATA
        stx     VDC_ADDR_REG
@L0:    bit     VDC_ADDR_REG
        bpl     @L0
        lda     (ptr2),y                ; speedup does not work for writing
        sta     VDC_DATA_REG
        iny
        bne     @L0
        rts

; ------------------------------------------------------------------------
; COPYFROM: Copy from extended into linear memory. A pointer to a structure
; describing the request is passed in a/x.
; The function must not return anything.
;

COPYFROM:
        jsr     setup
        beq     @L2                     ; Skip if no full pages

; Copy full pages

@L1:    jsr     transferin
        inc     ptr1+1
        inc     ptr2+1
        dec     tmp1
        bne     @L1

; Copy the remainder of the page

@L2:    ldy     #EM_COPY::COUNT
        lda     (ptr3),y                ; Get bytes in last page
        beq     @L4
        sta     tmp1

; Transfer the bytes in the last page

        ldy     #0
@L3:    jsr     vdcgetbyte
        sta     (ptr2),y
        iny
        dec     tmp1
        lda     tmp1
        bne     @L3
@L4:    rts

; ------------------------------------------------------------------------
; COPYTO: Copy from linear into extended memory. A pointer to a structure
; describing the request is passed in a/x.
; The function must not return anything.
;

COPYTO:
        jsr     setup
        beq     @L2                     ; Skip if no full pages

; Copy full pages

@L1:    jsr     transferout
        inc     ptr1+1
        inc     ptr2+1
        dec     tmp1
        bne     @L1

; Copy the remainder of the page

@L2:    ldy     #EM_COPY::COUNT
        lda     (ptr3),y                ; Get bytes in last page
        beq     @L4
        sta     tmp1

; Transfer the bytes in the last page

        ldy     #0
@L3:    lda     (ptr2),y
        jsr     vdcputbyte
        iny
        dec     tmp1
        lda     tmp1
        bne     @L3
@L4:    rts

;-------------------------------------------------------------------------
; Helper functions to handle VDC ram
;

vdcsetsrcaddr:
        ldx     #VDC_DATA_LO
        stx     VDC_ADDR_REG
@L0:    bit     VDC_ADDR_REG
        bpl     @L0
        sta     VDC_DATA_REG
        dex
        tya
        stx     VDC_ADDR_REG
        sta     VDC_DATA_REG
        rts

vdcgetbyte:
        ldx     #VDC_DATA
vdcgetreg:
        stx     VDC_ADDR_REG
@L0:    bit     VDC_ADDR_REG
        bpl     @L0
        lda     VDC_DATA_REG
        rts

vdcputbyte:
        ldx     #VDC_DATA
vdcputreg:
        stx     VDC_ADDR_REG
@L0:    bit     VDC_ADDR_REG
        bpl     @L0
        sta     VDC_DATA_REG
        rts

; ------------------------------------------------------------------------
; Helper function for COPYFROM and COPYTO: Store the pointer to the request
; structure and prepare data for the copy
;

setup:
        sta     ptr3
        stx     ptr3+1                  ; Save the passed em_copy pointer

        ldy     #EM_COPY::OFFS
        lda     (ptr3),y
        sta     ptr1
        ldy     #EM_COPY::PAGE
        lda     (ptr3),y
        sta     ptr1+1                  ; From

        ldy     #EM_COPY::BUF
        lda     (ptr3),y
        sta     ptr2
        iny
        lda     (ptr3),y
        sta     ptr2+1                  ; To

        ldy     #EM_COPY::COUNT+1
        lda     (ptr3),y                ; Get number of pages
        sta     tmp1
        rts

