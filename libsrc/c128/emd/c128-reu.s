;
; Extended memory driver for the Commodore REU. Driver works without
; problems when statically linked.
;
; Ullrich von Bassewitz, 2002-11-29
;

        .include        "zeropage.inc"

        .include        "em-kernel.inc"
        .include        "em-error.inc"
        .include        "c128.inc"

        .macpack        generic
        .macpack        module


; ------------------------------------------------------------------------
; Header. Includes jump table

        module_header   _c128_reu_emd

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

REU_STATUS      = $DF00                 ; Status register
REU_COMMAND     = $DF01                 ; Command register
REU_C64ADDR     = $DF02                 ; C64 base address register
REU_REUADDR     = $DF04                 ; REU base address register
REU_COUNT       = $DF07                 ; Transfer count register
REU_IRQMASK     = $DF09                 ; IRQ mask register
REU_CONTROL     = $DF0A                 ; Control register
REU_TRIGGER     = $FF00                 ; REU command trigger

OP_COPYFROM     = $ED
OP_COPYTO       = $EC

OP_COPYFROM_ALOAD = $B1
OP_COPYTO_ALOAD   = $B0

; ------------------------------------------------------------------------
; Data.

.bss
pagecount:      .res    2               ; Number of pages available
curpage:        .res    2               ; Current page number

window:         .res    256             ; Memory "window"

reu_params:     .word   $0000           ; Host address, lo, hi
                .word   $0000           ; Exp  address, lo, hi
                .byte   $00             ; Expansion  bank no.
                .word   $0000           ; # bytes to move, lo, hi
                .byte   $00             ; Interrupt mask reg.
                .byte   $00             ; Address control reg.

.code

; ------------------------------------------------------------------------
; INSTALL routine. Is called after the driver is loaded into memory. If
; possible, check if the hardware is present and determine the amount of
; memory available.
; Must return an EM_ERR_xx code in a/x.
;

INSTALL:
        ldx     #$00                    ; High byte of return code
        lda     #$55
        sta     REU_REUADDR
        cmp     REU_REUADDR             ; Check for presence of REU
        bne     nodevice
        asl     a                       ; A = $AA
        sta     REU_REUADDR
        cmp     REU_REUADDR             ; Check for presence of REU
        bne     nodevice

; determine the size
        php
        sei
        ldy     #$FF
loop:
        sty     window
        jsr     reu_size_check_common
        ldx     #OP_COPYTO_ALOAD
        stx     REU_COMMAND
        dey
        cpy     #$FF
        bne     loop
        iny
size_loop:
        jsr     reu_size_check_common
        ldx     #OP_COPYFROM_ALOAD
        stx     REU_COMMAND
        cpy     window
        bne     size_found
        iny
        bne     size_loop
size_found:
        plp
        ldx     #$00
        cpy     #$00                    ; too many pages, shave off 2
        bne     pagecount_ok
        dex
        dex
        dey
pagecount_ok:
        stx     pagecount
        sty     pagecount+1
        lda     #EM_ERR_OK
        .assert EM_ERR_OK = 0, error
        tax
        rts

; common REU setup for size check
reu_size_check_common:
        sty     REU_REUADDR+2
        ldx     #<window
        stx     REU_C64ADDR
        ldx     #>window
        stx     REU_C64ADDR+1
        ldx     #$00
        stx     REU_REUADDR
        stx     REU_REUADDR+1
        stx     REU_COUNT+1
        stx     REU_CONTROL
        inx
        stx     REU_COUNT
        rts

; No REU found

nodevice:
        lda     #EM_ERR_NO_DEVICE
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
        lda     pagecount
        ldx     pagecount+1
        rts

; ------------------------------------------------------------------------
; MAP: Map the page in a/x into memory and return a pointer to the page in
; a/x. The contents of the currently mapped page (if any) may be discarded
; by the driver.
;

MAP:    sta     curpage
        stx     curpage+1               ; Remember the new page

        ldy     #OP_COPYFROM
        jsr     common                  ; Copy the window

        lda     #<window
        ldx     #>window                ; Return the window address
done:   rts

; ------------------------------------------------------------------------
; USE: Tell the driver that the window is now associated with a given page.

USE:    sta     curpage
        stx     curpage+1               ; Remember the page
        lda     #<window
        ldx     #>window                ; Return the window
        rts

; ------------------------------------------------------------------------
; COMMIT: Commit changes in the memory window to extended storage.

COMMIT: lda     curpage
        ldx     curpage+1               ; Do we have a page mapped?
        bmi     done                    ; Jump if no page mapped

        ldy     #OP_COPYTO
common: sty     tmp1

        ldy     #<window
        sty     REU_C64ADDR
        ldy     #>window
        sty     REU_C64ADDR+1

        ldy     #0
        sty     REU_REUADDR+0
        sta     REU_REUADDR+1
        stx     REU_REUADDR+2

        sty     REU_COUNT+0
        ldy     #1
        sty     REU_COUNT+1             ; Move 256 bytes
        bne     transfer1               ; Transfer 256 bytes into REU

; ------------------------------------------------------------------------
; COPYFROM: Copy from extended into linear memory. A pointer to a structure
; describing the request is passed in a/x.
; The function must not return anything.
;

COPYFROM:
        ldy     #OP_COPYFROM
        .byte   $2C

; ------------------------------------------------------------------------
; COPYTO: Copy from linear into extended memory. A pointer to a structure
; describing the request is passed in a/x.
; The function must not return anything.
;

COPYTO:
        ldy     #OP_COPYTO
        sty     tmp1

; Remember the passed pointer

        sta     ptr1
        stx     ptr1+1          ; Save the pointer

; The structure passed to the functions has the same layout as the registers
; of the Commodore REU, so register programming is easy.

        ldy     #7-1
@L1:    lda     (ptr1),y
        sta     REU_C64ADDR,y
        dey
        bpl     @L1

; Invalidate the page in the memory window

        sty     curpage+1       ; Y = $FF

; Reload the REU command and start the transfer

transfer1:
        ldy     tmp1

; Transfer subroutine for the REU. Expects command in Y.

transfer:
        sty     REU_COMMAND     ; Issue command

        ldy     MMU_CR          ; Save the current MMU settings
        lda     #MMU_CFG_RAM0   ;
        sei                     ;
        sta     MMU_CR          ; Enable RAM in bank #0
        lda     REU_TRIGGER     ; Don't change $FF00
        sta     REU_TRIGGER     ; Start the transfer...

        sty     MMU_CR          ; Restore the old configuration
        cli
        rts

