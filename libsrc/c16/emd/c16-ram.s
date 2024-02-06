;
; Extended memory driver for the C16 hidden RAM. Driver works without
; problems when statically linked.
;
; Ullrich von Bassewitz, 2003-12-15
;

        .include        "zeropage.inc"

        .include        "em-kernel.inc"
        .include        "em-error.inc"
        .include        "plus4.inc"

        .macpack        generic
        .macpack        module


; ------------------------------------------------------------------------
; Header. Includes jump table

        module_header   _c16_ram_emd

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

; ------------------------------------------------------------------------
; Data.

.bss
pages:          .res    1               ; Number of pages
curpage:        .res    1               ; Current page number
window:         .res    256             ; Memory "window"

.code

; ------------------------------------------------------------------------
; INSTALL routine. Is called after the driver is loaded into memory. If
; possible, check if the hardware is present and determine the amount of
; memory available.
; Must return an EM_ERR_xx code in a/x.
;

INSTALL:

; Determine how much memory is available. We will use all memory above
; $8000 up to MEMTOP

        sec
        jsr     $FF99                   ; MEMTOP: Get top memory into Y/X
        tya
        sub     #>BASE                  ; Low 32 K are used
        bcc     nomem
        beq     nomem                   ; Offering zero pages is a bad idea
        sta     pages

        ldx     #$FF
        stx     curpage                 ; Invalidate the current page
        .assert EM_ERR_OK = 0, error
        inx
        txa
        rts

nomem:  ldx     #EM_ERR_NO_DEVICE
        lda     #0 ; return value is char
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
        lda     pages
        ldx     #$00                    ; 128 pages max
        rts

; ------------------------------------------------------------------------
; MAP: Map the page in a/x into memory and return a pointer to the page in
; a/x. The contents of the currently mapped page (if any) may be discarded
; by the driver.
;

MAP:    sta     curpage                 ; Remember the new page

        add     #>BASE
        sta     ptr1+1
        ldy     #$00
        sty     ptr1

        lda     #<window
        sta     ptr2
        lda     #>window
        sta     ptr2+1

; Transfer one page

        jsr     transfer                ; Transfer one page

; Return the memory window

        lda     #<window
        ldx     #>window                ; Return the window address
        rts

; ------------------------------------------------------------------------
; USE: Tell the driver that the window is now associated with a given page.

USE:    sta     curpage                 ; Remember the page
        lda     #<window
        ldx     #>window                ; Return the window
        rts

; ------------------------------------------------------------------------
; COMMIT: Commit changes in the memory window to extended storage.

COMMIT: lda     curpage                 ; Get the current page
        bmi     done                    ; Jump if no page mapped

        add     #>BASE
        sta     ptr2+1
        ldy     #$00
        sty     ptr2

        lda     #<window
        sta     ptr1
        lda     #>window
        sta     ptr1+1

; Transfer one page. Y must be zero on entry. Because we bank out the
; kernal, we will run the routine with interrupts disabled but leave
; short breath times. Unroll the following loop to make it somewhat faster.

transfer:
        sei
        sta     ENABLE_RAM

        .repeat 8
        lda     (ptr1),y
        sta     (ptr2),y
        iny
        .endrepeat

        sta     ENABLE_ROM
        cli

        bne     transfer

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
        add     #>BASE
        sta     ptr1+1                  ; From

        ldy     #EM_COPY::BUF
        lda     (ptr3),y
        sta     ptr2
        iny
        lda     (ptr3),y
        sta     ptr2+1                  ; To

common: ldy     #EM_COPY::COUNT+1
        lda     (ptr3),y                ; Get number of pages
        beq     @L2                     ; Skip if no full pages
        sta     tmp1

; Copy full pages allowing interrupts after each page copied

        ldy     #$00
@L1:    jsr     transfer
        inc     ptr1+1
        inc     ptr2+1
        dec     tmp1
        bne     @L1

; Copy the remainder of the page

@L2:    ldy     #EM_COPY::COUNT
        lda     (ptr3),y                ; Get bytes in last page
        beq     @L4
        tax

        sei                             ; Disable ints
        sta     ENABLE_RAM              ; Bank out the ROM

; Transfer the bytes in the last page

        ldy     #$00
@L3:    lda     (ptr1),y
        sta     (ptr2),y
        iny
        dex
        bne     @L3

; Restore the old memory configuration, allow interrupts

        sta     ENABLE_ROM
        cli

; Done

@L4:    rts

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
        add     #>BASE
        sta     ptr2+1                  ; To

        ldy     #EM_COPY::BUF
        lda     (ptr3),y
        sta     ptr1
        iny
        lda     (ptr3),y
        sta     ptr1+1                  ; From

        jmp     common


