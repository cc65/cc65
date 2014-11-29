;
; char cgetc (void);
;
        .export         _cgetc
        .import         cursor

        .include        "c1p.inc"
        .include        "extzp.inc"

; Input routine from 65V PROM MONITOR, show cursor if enabled
_cgetc:
        lda     cursor          ; show cursor?
        beq     nocursor
        ldy     CURS_X
        lda     (SCREEN_PTR),y  ; fetch current character
        sta     CURS_SAV        ; save it
        lda     #$A1            ; full white square
        sta     (SCREEN_PTR),y  ; store at cursor position
nocursor:
        jsr     INPUTC
        pha                     ; save retrieved character
        lda     cursor          ; was cursor on?
        beq     nocursor2
        lda     CURS_SAV        ; fetch saved character
        ldy     CURS_X
        sta     (SCREEN_PTR),y  ; store at cursor position
nocursor2:
        pla                     ; restore retrieved character
        rts
