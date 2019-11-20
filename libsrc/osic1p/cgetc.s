;
; char cgetc (void);
;

        .export         _cgetc
        .import         cursor
        .import         _kbhit

        .include        "osic1p.inc"
        .include        "extzp.inc"
        .include        "zeropage.inc"

; Input routine, show cursor if enabled
        .code
_cgetc:
        ldx     CHARBUF         ; character in buffer available?
        bne     done
        lda     cursor          ; show cursor?
        beq     nocursor
        ldy     CURS_X
        lda     (SCREEN_PTR),y  ; fetch current character
        sta     tmp1            ; save it
        lda     #$A1            ; full white square
        sta     (SCREEN_PTR),y  ; store at cursor position

nocursor:
        jsr     _kbhit          ; get input character in A
        tax                     ; save A in X, set flags
        beq     nocursor        ; until a key is actually pressed
        lda     cursor
        beq     done            ; was cursor on?
        lda     tmp1            ; fetch saved character
        ldy     CURS_X
        sta     (SCREEN_PTR),y  ; store at cursor position

done:
        lda     #$00
        sta     CHARBUF         ; empty buffer
        txa                     ; restore saved character from X
        ldx     #$00            ; high byte of int return value
        rts
