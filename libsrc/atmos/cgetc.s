;
; Ullrich von Bassewitz, 2003-04-13
;
; char cgetc (void);
;

	.export		_cgetc
        .constructor    cursoroff
        .destructor     cursoron
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
        jsr     cursoron
@L1:    lda     KEYBUF
        bpl     @L1

; If the cursor was enabled, disable it now

        ldx     cursor
        beq     @L3
        dec     STATUS          ; Clear bit zero

; We have the character, clear avail flag

@L2:    and     #$7F            ; Mask out avail flag
        sta     KEYBUF

; Done

@L3:    ldx     #$00
        rts

.endproc

; ------------------------------------------------------------------------
; Switch the cursor on

cursoron:
        lda     STATUS
        ora     #%00000001
        sta     STATUS
        rts

; ------------------------------------------------------------------------
; Switch the cursor off

cursoroff:
        lda     STATUS
        and     #%11111110
        sta     STATUS
        rts

