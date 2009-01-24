;
; Ullrich von Bassewitz, 2003-04-13
;
; char cgetc (void);
;

	.export		_cgetc
        .constructor    initcgetc
	.import		cursor

	.include	"atmos.inc"



; ------------------------------------------------------------------------
;

.proc   _cgetc

        lda     KEYBUF          ; Do we have a character?
        bmi     @L2             ; Yes: Get it

; No character, enable cursor and wait

        lda     cursor          ; Cursor currently off?
        beq     @L1             ; Skip if so
        lda     STATUS
        ora     #%00000001      ; Cursor ON
        sta     STATUS
@L1:    lda     KEYBUF
        bpl     @L1

; If the cursor was enabled, disable it now

        ldx     cursor
        beq     @L2
        ldx     #$00            ; Zero high byte
        dec     STATUS          ; Clear bit zero

; We have the character, clear avail flag

@L2:    and     #$7F            ; Mask out avail flag
        sta     KEYBUF
        ldy     $209
        cpy     #$A5
        bne     @L3
        ora     #$80            ; FUNCT pressed

; Done

@L3:    rts

.endproc

; ------------------------------------------------------------------------
; Switch the cursor off, disable capslock. Code goes into the INIT segment
; which may be reused after it is run.

.segment        "INIT"

initcgetc:
        lda     STATUS
        and     #%11111110
        sta     STATUS
        lda     #$7F
        sta     CAPSLOCK
        rts

