;
; Extended memory driver for the C64 hidden RAM
;
; Ullrich von Bassewitz, 2002-12-02
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
        .byte   $00                     ; EM API version number

; Jump table.

        .word   INSTALL
        .word   DEINSTALL
        .word   PAGECOUNT
        .word   MAP
        .word   USE
        .word   COMMIT
	.word	COPYFROM
        .word   COPYTO

; ------------------------------------------------------------------------
; Constants

BASE	= $D000
PAGES  	= ($10000 - BASE) / 256

; ------------------------------------------------------------------------
; Data.

.data
curpage:        .byte	$FF		; Current page number (invalid)

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
        lda     #<EM_ERR_OK
        ldx     #>EM_ERR_OK
        rts

; ------------------------------------------------------------------------
; DEINSTALL routine. Is called before the driver is removed from memory.
; Can do cleanup or whatever. Must not return anything.
;

DEINSTALL:
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

MAP:    sta     curpage			; Remember the new page

        clc
        adc	#>BASE
        sta	ptr1+1
        ldy	#$00
        sty    	ptr1

        lda	#<window
        sta	ptr2
        lda	#>window
        sta	ptr2+1

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

COMMIT: lda     curpage			; Get the current page
        bmi     done                    ; Jump if no page mapped

        clc
        adc	#>BASE
        sta	ptr2+1
        ldy	#$00
        sty    	ptr2

        lda	#<window
        sta	ptr1
        lda	#>window
        sta	ptr1+1

; Transfer one page. Y must be zero on entry

transfer:
        ldx    	$01   			; Remember c64 control port
        txa
        and     #$F8           		; Bank out ROMs, I/O
        sei
        sta    	$01

; Unroll the following loop

loop:   lda	(ptr1),y
        sta	(ptr2),y
        iny

        lda	(ptr1),y
        sta	(ptr2),y
        iny

        lda	(ptr1),y
        sta	(ptr2),y
        iny

        lda	(ptr1),y
        sta	(ptr2),y
        iny

        lda	(ptr1),y
        sta	(ptr2),y
        iny

        lda	(ptr1),y
        sta	(ptr2),y
        iny

        lda	(ptr1),y
        sta	(ptr2),y
        iny

        lda	(ptr1),y
        sta	(ptr2),y
        iny

        bne     loop

; Restore the old memory configuration, allow interrupts

        stx     $01                     ; Restore the old configuration
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

        ldy     #EM_COPY_OFFS
        lda     (ptr3),y
        sta     ptr1
        ldy     #EM_COPY_PAGE
        lda     (ptr3),y
        clc
        adc     #>BASE
        sta     ptr1+1                  ; From

        ldy     #EM_COPY_BUF
        lda     (ptr3),y
        sta     ptr2
        iny
        lda     (ptr3),y
        sta     ptr2+1                  ; To

common: ldy     #EM_COPY_COUNT+1
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

@L2:    ldy     #EM_COPY_COUNT
        lda     (ptr3),y                ; Get bytes in last page
        beq     @L4
        tax

        lda     $01                     ; Remember c64 control port
        pha
        and     #$F8           		; Bank out ROMs, I/O
        sei
        sta    	$01

; Transfer the bytes in the last page

@L3:    lda	(ptr1),y
        sta	(ptr2),y
        iny
        dex
        bne     @L3

; Restore the old memory configuration, allow interrupts

        pla
        sta     $01                     ; Restore the old configuration
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

        ldy     #EM_COPY_OFFS
        lda     (ptr3),y
        sta     ptr2
        ldy     #EM_COPY_PAGE
        lda     (ptr3),y
        clc
        adc     #>BASE
        sta     ptr2+1                  ; To

        ldy     #EM_COPY_BUF
        lda     (ptr3),y
        sta     ptr1
        iny
        lda     (ptr3),y
        sta     ptr1+1                  ; From

        jmp     common


