;
; Extended memory driver for the Commodore REU
;
; Ullrich von Bassewitz, 2002-11-29
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
        .word   COMMIT
	.word	COPYFROM
        .word   COPYTO

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


; ------------------------------------------------------------------------
; Data.

.data
pagecount:      .res    2               ; Number of pages available
curpage:        .word   $FFFF           ; Current page number (invalid)

.bss
window:         .res    256             ; Memory "window"

reu_params:     .word 	$0000  		; Host address, lo, hi
		.word 	$0000		; Exp  address, lo, hi
                .byte	$00		; Expansion  bank no.
       	       	.word  	$0000  		; # bytes to move, lo, hi
       		.byte 	$00    		; Interrupt mask reg.
       		.byte 	$00    		; Adress control reg.

.code

; ------------------------------------------------------------------------
; INSTALL routine. Is called after the driver is loaded into memory. If
; possible, check if the hardware is present and determine the amount of
; memory available.
; Must return an EM_ERR_xx code in a/x.
;

INSTALL:
        lda     #$55
        sta     REU_REUADDR
        cmp     REU_REUADDR             ; Check for presence of REU
        bne     nodevice
        lda     #$AA
        sta     REU_REUADDR
        cmp     REU_REUADDR             ; Check for presence of REU
        bne     nodevice

        ldx     #>(128*4)               ; Assume 128KB
        lda     REU_STATUS
        and     #$10                    ; Check size bit
        beq     @L1
        ldx     #>(256*4)               ; 256KB when size bit is set
@L1:    stx     pagecount+1

        lda     #<EM_ERR_OK
        ldx     #>EM_ERR_OK
        rts

; No REU found

nodevice:
        lda     #<EM_ERR_NO_DEVICE
        ldx     #>EM_ERR_NO_DEVICE
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
        lda     pagecount
        ldx     pagecount+1
        rts

; ------------------------------------------------------------------------
; MAP: Map the page in a/x into memory and return a pointer to the page in
; a/x. The contents of the currently mapped page (if any) are assumed to be
; dirty and must be saved into secondary storage if this is necessary.
;

MAP:    sta     curpage
        stx     curpage+1               ; Remember the new page

        ldy     #OP_COPYFROM
        jsr     common                  ; Copy the window

        lda     #<window
        ldx     #>window                ; Return the window address
done:   rts

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
        sty    	REU_COMMAND     ; Issue command

        ldy 	$01       	; Save the value of the c64 control port...
        tya          	  	;
        ora 	#$03      	; Turn on lower 3 bits to bank out ROMs, I/O.
  	sei          	  	;
        sta 	$01
        lda     REU_TRIGGER     ; Don't change $FF00
        sta 	REU_TRIGGER     ; Start the transfer...

        sty     $01             ; Restore the old configuration
        cli
        rts

