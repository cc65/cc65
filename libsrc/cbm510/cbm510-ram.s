;
; Extended memory driver for the CBM510 additional RAM banks. Driver works
; without problems when linked statically.
;
; Ullrich von Bassewitz, 2002-12-09        !!! UNTESTED !!!
;

	.include 	"zeropage.inc"

      	.include     	"em-kernel.inc"
        .include        "em-error.inc"
        .include        "cbm510.inc"

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

RAMBANK = 2
OFFS    = 2

; ------------------------------------------------------------------------
; Data.

.bss
curpage:        .res    1               ; Current page number

window:         .res    256             ; Memory "window"
pagecount:	.res	1               ; Number of available pages


.code

; ------------------------------------------------------------------------
; INSTALL routine. Is called after the driver is loaded into memory. If
; possible, check if the hardware is present and determine the amount of
; memory available.
; Must return an EM_ERR_xx code in a/x.
;

INSTALL:
       	lda	#$FF
        sta     curpage                 ; Invalidate the current page

	ldx	UsrMemTop+2
	cpx    	#RAMBANK                ; Top of memory in bank 2?
        bne     @L1                     ; No: We can use all the memory
        clc
        adc     UsrMemTop+1
@L1:    sta     pagecount

        lda     #<EM_ERR_OK
        ldx     #>EM_ERR_OK
;       rts				; Run into UNINSTALL instead

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
        ldx     #0
        rts

; ------------------------------------------------------------------------
; MAP: Map the page in a/x into memory and return a pointer to the page in
; a/x. The contents of the currently mapped page (if any) may be discarded
; by the driver.
;

MAP:    sta     curpage	   		; Remember the new page

        sta	ptr1+1
        lda	#OFFS
        sta    	ptr1

; Transfer one page

        ldx     IndReg
        lda     #RAMBANK
        sta     IndReg

        ldy     #$00
@L1:    lda     (ptr1),y
        sta     window,y
        iny
        lda     (ptr1),y
        sta     window,y
        iny
        bne     @L1

        stx     IndReg

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

COMMIT: lda     curpage			; Get the current page
        cmp     #$FF
        beq     done                    ; Jump if no page mapped

        sta     ptr1+1
        lda     #OFFS
        sta     ptr1

; Transfer one page

        ldx     IndReg
        lda     #RAMBANK
        sta     IndReg

        ldy     #$00
@L1:    lda     window,y
        sta     (ptr1),y
        iny
        lda     window,y
        sta     (ptr1),y
        iny
        bne     @L1

        stx     IndReg

; Done

done:   rts

; ------------------------------------------------------------------------
; COPYFROM: Copy from extended into linear memory. A pointer to a structure
; describing the request is passed in a/x.
; The function must not return anything.
;

COPYFROM:
        jsr     setup

; Setup the buffer address in this bank.

        sta     copyfrom_buf
        stx     copyfrom_buf+1

; Check if we must copy full pages

        ldx     ptr2+1
        beq     @L2

; Copy full pages

        ldx     #$00
@L1:    jsr     copyfrom
        inc     ptr1+1
        inc     copyfrom_buf+1
@L2:    dec     ptr2+1
        bne     @L1

; Copy the remaining page

        ldx     ptr2
        beq     @L3

        jsr     copyfrom

; Restore the indirect segment

@L3:    lda     ExecReg
        sta     IndReg

; Done

        rts


; ------------------------------------------------------------------------
; COPYTO: Copy from linear into extended memory. A pointer to a structure
; describing the request is passed in a/x.
; The function must not return anything.
;

COPYTO: jsr     setup

; Setup the buffer address in this bank.

        sta     copyto_buf
        stx     copyto_buf+1

; Check if we must copy full pages

        ldx     ptr2+1
        beq     @L2

; Copy full pages

        ldx     #$00
@L1:    jsr     copyto
        inc     ptr1+1
        inc     copyto_buf+1
@L2:    dec     ptr2+1
        bne     @L1

; Copy the remaining page

        ldx     ptr2
        beq     @L3

        jsr     copyto

; Restore the indirect segment

@L3:    lda     ExecReg
        sta     IndReg

; Done

        rts

; ------------------------------------------------------------------------
; setup: Helper function for COPYFROM and COPYTO, will setup parameters.
;

setup:  sta     ptr3
        stx     ptr3+1                  ; Save the passed em_copy pointer

        ldy     #EM_COPY::OFFS
        lda     (ptr3),y
        add     #OFFS
        sta     ptr1
        ldy     #EM_COPY::PAGE
        lda     (ptr3),y
        adc     #$00
        sta     ptr1+1

        ldy     #EM_COPY::COUNT
        lda     (ptr3),y
        sta     ptr2
        iny
        lda     (ptr3),y
        sta     ptr2+1                  ; Get count into ptr2

        ldy     #EM_COPY::BUF+1
        lda     (ptr3),y
        tax
        dey
        lda     (ptr3),y                ; Get the buffer pointer into a/x

        ldy     #RAMBANK
        sty     IndReg

        ldy     #$00

        rts

; ------------------------------------------------------------------------
; copyfrom

.data
copyfrom:
        lda     (ptr1),y
copyfrom_buf = * + 1
        sta     $0000,y
        iny
        dex
        bne     copyfrom
        rts

; ------------------------------------------------------------------------
; copyto

.data
copyto:
copyto_buf = * + 1
        lda     $0000,y
        sta     (ptr1),y
        iny
        dex
        bne     copyto
        rts

