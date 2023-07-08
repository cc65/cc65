;
; Extended memory driver for the C64 D2TV (the second or PAL version).
; Driver works without problems when statically linked.
;
; Ullrich von Bassewitz, 2005-11-27
;


        .include        "zeropage.inc"

        .include        "em-kernel.inc"
        .include        "em-error.inc"
        .import _get_ostype


        .macpack        generic
        .macpack        module


; ------------------------------------------------------------------------
; Header. Includes jump table

        module_header   _dtv_himem_emd

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

OP_COPYFROM     = %00001101
OP_COPYTO       = %00001111

START_BANK      = 2                     ; Start at $20000
PAGES           = (2048 - 128) * 4


; ------------------------------------------------------------------------
; Data.

.bss
window:         .res    256             ; Memory "window"

.data

; The MAP and COMMIT entries will actually call COPYFROM/COPYTO with
; a pointer to the following data structure:

dma_params:     .word   window          ; Host address
                .byte   0               ; Offset in page
curpage:        .word   $0000           ; Page
                .word   .sizeof (window); # bytes to move, lo, hi

.code

; ------------------------------------------------------------------------
; INSTALL routine. Is called after the driver is loaded into memory. If
; possible, check if the hardware is present and determine the amount of
; memory available.
; Must return an EM_ERR_xx code in a/x.
;

INSTALL:

; Check for a DTV

        ldx     #1
        stx     $d03f
        ldx     $d040
        cpx     $d000
        bne     @present
        inc     $d000
        cpx     $d040
        beq     @present
        dec     $d000

; DTV not found

        lda     #EM_ERR_NO_DEVICE
        ldx     #0 ; return value is char
        rts

@present:
        ldx     #$FF
        stx     curpage+1               ; Invalidate curpage
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
; a/x.  The contents of the currently mapped page (if any) may be discarded
; by the driver.
;

MAP:    sta     curpage
        stx     curpage+1               ; Remember the new page

        lda     #<dma_params
        ldx     #>dma_params
        jsr     COPYFROM                ; Copy data into the window

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

COMMIT: lda     curpage+1               ; Do we have a page mapped?
        bmi     done                    ; Jump if no page mapped

        lda     #<dma_params
        ldx     #>dma_params

; Run into COPYTO

; ------------------------------------------------------------------------
; COPYTO: Copy from linear into extended memory. A pointer to a structure
; describing the request is passed in a/x.
; The function must not return anything.
;

COPYTO: sta     ptr1
        stx     ptr1+1                  ; Save the pointer

        ldx     #OP_COPYTO              ; Load the command
        bne     transfer

; ------------------------------------------------------------------------
; COPYFROM: Copy from extended into linear memory. A pointer to a structure
; describing the request is passed in a/x.
; The function must not return anything.
;

COPYFROM:
        sta     ptr1
        stx     ptr1+1                  ; Save the pointer

        ldx     #OP_COPYFROM

; DTV DMA transfer routine. Expects the command in X.
; NOTE: We're using knowledge about field order in the EM_COPY struct here!

transfer:
        jsr     WAIT                    ; Wait until DMA is finished

; Modulo disable

        ldy     #$00
        sty     $d31e

; Setup the target address and the source and target steps. Y contains zero,
; which is EM_COPY::BUF.

        sty     $d307                   ; Source step high = 0
        sty     $d309                   ; Dest step high = 0
        lda     (ptr1),y
        sta     $d303                   ; Dest address low
        iny                             ; Y = 1
        sty     $d306                   ; Source step low = 1
        sty     $d308                   ; Dest step low = 1
        lda     (ptr1),y
        sta     $d304
        lda     #$40                    ; Dest is always RAM, start at $00000
        sta     $d305

; Setup the source address. Incrementing Y will make it point to EM_COPY::OFFS.
; We will allow page numbers higher than PAGES and map them to ROM. This will
; allow reading the ROM by specifying a page starting with PAGES.

        iny                             ; EM_COPY::OFFS
        lda     (ptr1),y
        sta     $d300
        iny                             ; EM_COPY::PAGE
        lda     (ptr1),y
        sta     $d301
        iny
        lda     (ptr1),y
        adc     #START_BANK             ; Carry clear here from WAIT
        and     #$3f
        cmp     #>PAGES+START_BANK      ; Valid range?
        bcs     @L1                     ; Jump if no
        ora     #$40                    ; Address RAM
@L1:    sta     $d302

; Length

        iny                             ; EM_COPY::COUNT
        lda     (ptr1),y
        sta     $d30a
        iny
        lda     (ptr1),y
        sta     $d30b

; Start DMA

        stx     $d31f

; Wait until DMA is done

WAIT:   lda     $d31f
        lsr     a
        bcs     WAIT
        rts

