;
; 2003-04-13, Ullrich von Bassewitz
; 2014-09-04, Greg King
;
; char cgetc (void);
;

        .export         _cgetc
        .constructor    initcgetc

        .import         cursor
        .forceimport    disable_caps

        .include        "atmos.inc"


; ------------------------------------------------------------------------
;

.proc   _cgetc

        lda     KEYBUF          ; Do we have a character?
        bmi     @L2             ; Yes: Get it

; No character, enable cursor and wait

        lda     cursor          ; Should cursor be off?
        beq     @L1             ; Skip if so
        lsr     STATUS
        sec                     ; Cursor ON
        rol     STATUS
@L1:    lda     KEYBUF
        bpl     @L1

; If the cursor was enabled, disable it now

        ldx     cursor
        beq     @L2
        dec     STATUS          ; Clear bit zero

; We have the character, clear the "available" flag

@L2:    and     #$7F            ; Mask out avail flag
        sta     KEYBUF
        ldx     #>$0000
        ldy     MODEKEY
        cpy     #FUNCTKEY
        bne     @L3
        ora     #$80            ; FUNCT-key pressed

; Done

@L3:    rts

.endproc

; ------------------------------------------------------------------------
; Switch the cursor off. Code goes into the ONCE segment
; which may be reused after it is run.

.segment        "ONCE"

initcgetc:
        lsr     STATUS
        asl     STATUS          ; Clear bit zero
        rts
