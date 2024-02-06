;
; unsigned char kbhit (void);
;
; The method to detect a pressed key is based on the documentation in
; "Section 3 Programmed Key Functions" in "The Challenger Character Graphics
; Reference Manual"
; We only want to return true for characters that can be returned by cgetc(),
; but not for keys like <Shift> or <Ctrl>. Therefore a special handling is
; needed for the first row. This is implemented by a bit mask that is stored
; in tmp1 and that is set to zero after the first round.
;

        .constructor    initkbhit
        .export _kbhit
        .include "osic1p.inc"
        .include "extzp.inc"
        .include "zeropage.inc"

; Internal state that needs to be preserved across calls.
.segment        "EXTZP" : zeropage

LASTSCAN:       .res    1       ; Result of previous keyboard scan
DBNCCNT:        .res    1       ; Debounce counter
KBDTMP:         .res    1       ; Temporary values
CTRLSHIFT:      .res    1       ; State of CTRL and SHIFT keys

; Initialize one-character buffer that is filled by kbhit()
        .segment        "ONCE"
initkbhit:
        lda     #$00
        sta     CHARBUF         ; No character in buffer initially

        sta     LASTSCAN        ; Initialize keyboard state
        sta     DBNCCNT
        sta     KBDTMP
        sta     CTRLSHIFT

        rts

; Routine to get character from keyboard and return it in A.
; Based on the OSI ROM routine at $FD00 but uses different
; storage locations to avoid corrupting CC65 run-time code.

_kbhit: lda     CHARBUF         ; Check for previously saved character
        beq     LFD05
        ldx     #$00            ; High byte of return is always zero
        rts                     ; A contains non-zero character code meaning true
LFD05:  lda     #$80            ; Bit mask for initial keyboard row
LFD06:  jsr     LFCBE           ; Write keyboard row
        jsr     LFCC6           ; Read keyboard column
        bne     LFD13           ; Branch if a key in this column was pressed
        lsr     a               ; Otherwise shift mask to next row
        bne     LFD06           ; If not done yet, check next row
        beq     LFD3A           ; Branch if last row reached and no key pressed
LFD13:  lsr     a               ; Have a key press. Shift LSB into carry
        bcc     LFD1F           ; Branch if no key pressed in column 0
        txa                     ; Key pressed in row zero. Get the column data
        and     #$20            ; Mask only the bit for <ESC> as it is the only key in row zero that returns key press
        beq     LFD3A           ; Branch if <ESC> was not the key
        lda     #$1B            ; Set character to <ESC>
        bne     LFD50           ; Do more processing
LFD1F:  jsr     LFE86           ; Shift to find bit that is set (in Y)
        tya                     ; Get bit
        sta     KBDTMP          ; Save it
        asl     a               ; Multiply by 7 by shifting left three times (X8)...
        asl     a
        asl     a
        sec                     ; ...then subtracting one
        sbc     KBDTMP
        sta     KBDTMP          ; Save value*7 for later lookup in table
        txa                     ; Get the keyboard column
        lsr     a               ; Shift out bit zero (only key there is <SHIFT LOCK>)
        asl     a               ; And shift back
        jsr     LFE86           ; Shift to find bit that is set (in Y)
        beq     LFD47           ; Branch if no keys pressed
        lda     #$00
LFD3A:  sta     CTRLSHIFT       ; Save state of <CTRL> and shift keys
LFD3D:  sta     LASTSCAN
        lda     #$02            ; Count used for key debouncing
        sta     DBNCCNT
        ldx     #$00            ; High byte of return is always zero
        lda     #$00            ; Return false
        rts
LFD47:  clc
        tya                     ; Get bit number of pressed key
        adc     KBDTMP          ; Add previously calculated offset for keyboard row*7
        tay
        lda     LFF3B,y         ; Read ASCII code for key from table
LFD50:  cmp     LASTSCAN        ; Debounce - same as last key scan?
        bne     LFD3D           ; If not, try again
        dec     DBNCCNT         ; Decrement debounce counter
        beq     LFD5F           ; Branch if done debouncing
        jsr     LFCDF           ; Wait for short delay to debounce keyboard
        beq     _kbhit          ; Go back and scan keyboard.
LFD5F:  ldx     #$64            ; Was <CONTROL> key down?
        cmp     CTRLSHIFT
        bne     LFD68           ; Branch if not
        ldx     #$0F
LFD68:  stx     DBNCCNT
        sta     CTRLSHIFT
        cmp     #$21
        bmi     LFDD0           ; Done, return key
        cmp     #$5F
        beq     LFDD0           ; Done, return key
        lda     #$01
        jsr     LFCBE           ; Write keyboard row
        jsr     LFCCF           ; Read keyboard column
        sta     KBDTMP
        tax
        and     #$06
        bne     LFDA2
        bit     LASTSCAN
        bvc     LFDBB
        txa
        eor     #$01
        and     #$01
        beq     LFDBB
        lda     #$20
        bit     KBDTMP
        bvc     LFDC3
        lda     #$C0
        bne     LFDC3
LFDA2:  bit     LASTSCAN
        bvc     LFDAA
        txa
        beq     LFDBB
LFDAA:  ldy     LASTSCAN
        cpy     #$31
        bcc     LFDB9
        cpy     #$3C
        bcs     LFDB9
        lda     #$F0
        bne     LFDBB
LFDB9:  lda     #$10
LFDBB:  bit     KBDTMP
        bvc     LFDC3
        clc
        adc     #$C0
LFDC3:  clc
        adc     LASTSCAN
        and     #$7F
        bit     KBDTMP
        bpl     LFDD0
        ora     #$80
LFDD0:  sta     KBDTMP          ; Save pressed key and return in A
        sta     CHARBUF
        rts

; Write keyboard row with value in A.
; Invert the bits before writing.
; Returns original value of A.

LFCBE:  eor     #$FF
        sta     KBD
        eor     #$FF
        rts

; Read keyboard column and return in X.
; Sets Z flag if no keys were pressed.
; Saves current value of A.

LFCC6:  pha                     ; Save A
        jsr     LFCCF           ; Read keyboard column
        tax                     ; Save in X
        pla                     ; Restore A
        dex                     ; Decrement and then increment to
        inx                     ; preserve value of X but set flags
        rts

; Read keyboard column.
; Invert the bits (pressed key(s) will show up as ones).

LFCCF:  lda     KBD             ; Read keyboard hardware
        eor     #$FF            ; Invert the bits
        rts

; Short fixed delay routine.

LFCDF:  ldy     #$10
LFCE1:  ldx     #$40
LFCE3:  dex
        bne     LFCE3
        dey
        bne     LFCE1
        rts

; Shift A left until we find a 1 in the most significant bit.
; Return the bit number in Y.

LFE86:  ldy     #$08
LFE88:  dey
        asl     a
        bcc     LFE88
        rts

; Lookup table of keyboard keys for each scan row.
LFF3B:  .byte   $BD
        .byte   'P', ';', '/', ' ', 'Z', 'A', 'Q'
        .byte   ',', 'M', 'N', 'B', 'V', 'C', 'X'
        .byte   'K', 'J', 'H', 'G', 'F', 'D', 'S'
        .byte   'I', 'U', 'Y', 'T', 'R', 'E', 'W'
        .byte   $00, $00, $0D, $0A, 'O', 'L', '.'
        .byte   $00, '_', '-', ':', '0', '9', '8'
        .byte   '7', '6', '5', '4', '3', '2', '1'
