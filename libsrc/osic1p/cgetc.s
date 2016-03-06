;
; char cgetc (void);
;

        .constructor    initcgetc
        .export         _cgetc
        .import         cursor

        .include        "osic1p.inc"
        .include        "extzp.inc"
        .include        "zeropage.inc"

; Initialize one-character buffer that is filled by kbhit()
        .segment        "ONCE"
initcgetc:
        lda     #$00
        sta     CHARBUF         ; No character in buffer initially
        rts

; Input routine from 65V PROM MONITOR, show cursor if enabled
        .code
_cgetc:
        lda     CHARBUF         ; character in buffer available?
        beq     nobuffer
        tax                     ; save character in X
        lda     #$00
        sta     CHARBUF         ; empty buffer
        beq     restorex        ; restore X and return
nobuffer:
        lda     cursor          ; show cursor?
        beq     nocursor
        ldy     CURS_X
        lda     (SCREEN_PTR),y  ; fetch current character
        sta     tmp1            ; save it
        lda     #$A1            ; full white square
        sta     (SCREEN_PTR),y  ; store at cursor position
nocursor:
        jsr     INPUTC          ; get input character in A
        ldx     cursor
        beq     done            ; was cursor on?
        tax                     ; save A in X
        lda     tmp1            ; fetch saved character
        ldy     CURS_X
        sta     (SCREEN_PTR),y  ; store at cursor position

restorex:
        txa                     ; restore saved character from X
done:
        ldx     #$00            ; high byte of int return value
        rts
