;
; char cgetc (void);
;
        .export         _cgetc
        .import         cursor

        .include        "osic1p.inc"
        .include        "extzp.inc"
        .include        "zeropage.inc"

; Input routine from 65V PROM MONITOR, show cursor if enabled
_cgetc:
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
        txa                     ; restore saved character from X
        ldx     #$00            ; high byte of int return value
done:
        rts
