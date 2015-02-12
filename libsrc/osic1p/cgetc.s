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
        jsr     INPUTC
        pha                     ; save retrieved character
        lda     cursor          ; was cursor on?
        beq     nocursor2
        lda     tmp1            ; fetch saved character
        ldy     CURS_X
        sta     (SCREEN_PTR),y  ; store at cursor position
nocursor2:
        pla                     ; restore retrieved character
        rts
