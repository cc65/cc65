;
; Extended memory driver for the C64 D2TV (the second or PAL version).
; Driver works without problems when statically linked.
;
; Ullrich von Bassewitz, 2005-11-27
;

	.include 	"zeropage.inc"

      	.include 	"em-kernel.inc"
        .include        "em-error.inc"


        .macpack        generic


; ------------------------------------------------------------------------
; Header. Includes jump table

.segment        "JUMPTABLE"

; Driver signature

        .byte   $65, $6d, $64           ; "emd"
        .byte   EMD_API_VERSION		; EM API version number

; Jump table.

        .word   INSTALL
        .word   UNINSTALL
        .word   PAGECOUNT
        .word   MAP
        .word   USE
        .word   COMMIT
	.word	COPYFROM
        .word   COPYTO

; ------------------------------------------------------------------------
; Constants

OP_COPYFROM     = %00001101
OP_COPYTO       = %00001111

PAGES           = (2048 - 128) * 4


; ------------------------------------------------------------------------
; Data.

.bss
window:         .res    256             ; Memory "window"

.data

; The MAP and COMMIT entries will actually call COPYFROM/COPYTO with
; a pointer to the following data structure:

dma_params:     .word  	window          ; Host address
                .byte   0               ; Offset in page
curpage:        .word  	$0000  	       	; Page
       	       	.word  	.sizeof (window); # bytes to move, lo, hi

.code

; ------------------------------------------------------------------------
; INSTALL routine. Is called after the driver is loaded into memory. If
; possible, check if the hardware is present and determine the amount of
; memory available.
; Must return an EM_ERR_xx code in a/x.
;

INSTALL:
        ldx     #$FF
        stx     curpage+1               ; Invalidate curpage
        inx                             ; X = 0
        txa                             ; A/X = EM_ERR_OK
        rts

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

transfer:
        lda     #$01            ; Enable extended features
        sta     $d03f

; Wait until the current DMA operation is finished

        jsr     WAIT

; Setup the source address. Carry is clear.

        ldy     #EM_COPY::OFFS
        lda     (ptr1),y
        sta     $d300
        ldy     #EM_COPY::PAGE
        lda     (ptr1),y
        sta     $d301
        iny
        lda     (ptr1),y
        adc     #$42            ; Always RAM, start at $20000
        sta     $d302

; Setup the target address

        ldy     #EM_COPY::BUF
        lda     (ptr1),y
        sta     $d303
        iny
        lda     (ptr1),y
        sta     $d304
        lda     #$40            ; Always RAM, start at $00000
        sta     $d305

; Source and target steps

        lda     #$01
        ldy     #$00
        sta     $d306
        sty     $d307
        sta     $d308
        sty     $d309

; Modulo disable

        sty     $d31e

; Length

        ldy     #EM_COPY::COUNT
        lda     (ptr1),y
        sta     $d30a
        iny
        lda     (ptr1),y
        sta     $d30b

; Start DMA

        stx     $d31f

; Wait until DMA finished

        jsr     WAIT

; Disable access to the extended registers

        lda     #$00
        sta     $d03f

        rts


; ------------------------------------------------------------------------
; Wait until DMA has finished

WAIT:   lda     $d31f
        lsr     a
        bcs     WAIT
        rts
