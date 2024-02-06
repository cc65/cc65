;
; Ullrich von Bassewitz, 13.09.2001
;
; Keyboard polling stuff for the 510.
;

        .export         scnkey
        .importzp       tpi2, ktab1, ktab2, ktab3, ktab4
        .importzp       keyidx, keybuf, keyscanbuf, keysave, modkey, norkey
        .importzp       graphmode, lastidx, rptdelay, rptcount

        .include        "cbm510.inc"


.proc   scnkey

        lda     #$FF
        sta     modkey
        sta     norkey
        lda     #$00
        sta     keyscanbuf
        ldy     #TPI::PRB
        sta     (tpi2),y
        ldy     #TPI::PRA
        sta     (tpi2),y
        jsr     Poll
        and     #$3F
        eor     #$3F
        bne     L1
        jmp     NoKey

L1:     lda     #$FF
        ldy     #TPI::PRA
        sta     (tpi2),y
        asl     a
        ldy     #TPI::PRB
        sta     (tpi2),y
        jsr     Poll
        pha
        sta     modkey
        ora     #$30
        bne     L3              ; Branch always

L2:     jsr     Poll
L3:     ldx     #$05
        ldy     #$00
L4:     lsr     a
        bcc     L5
        inc     keyscanbuf
        dex
        bpl     L4
        sec
        ldy     #TPI::PRB
        lda     (tpi2),y
        rol     a
        sta     (tpi2),y
        ldy     #TPI::PRA
        lda     (tpi2),y
        rol     a
        sta     (tpi2),y
        bcs     L2
        pla
        bcc     NoKey           ; Branch always

L5:     ldy     keyscanbuf
        sty     norkey
        pla
        asl     a
        asl     a
        asl     a
        bcc     L6
        bmi     L7
        lda     (ktab2),y               ; Shifted normal key
        ldx     graphmode
        beq     L8
        lda     (ktab3),y               ; Shifted key in graph mode
        bne     L8

L6:     lda     (ktab4),y               ; Key with ctrl pressed
        bne     L8
L7:     lda     (ktab1),y               ; Normal key
L8:     tax
        cpx     #$FF                    ; Valid key?
        beq     Done
        cpy     lastidx
        beq     Repeat
        ldx     #$13
        stx     rptdelay
        ldx     keyidx
        cpx     #$09
        beq     NoKey
        cpy     #$59
        bne     PutKey
        cpx     #$08
        beq     NoKey
        sta     keybuf,x
        inx
        bne     PutKey

NoKey:  ldy     #$FF
Done:   sty     lastidx
End:    lda     #$7F
        ldy     #TPI::PRA
        sta     (tpi2),y
        ldy     #TPI::PRB
        lda     #$FF
        sta     (tpi2),y
        rts

Repeat: dec     rptdelay
        bpl     End
        inc     rptdelay
        dec     rptcount
        bpl     End
        inc     rptcount
        ldx     keyidx
        bne     End

PutKey: sta     keybuf,x
        inx
        stx     keyidx
        ldx     #$03
        stx     rptcount
        bne     Done

.endproc


; Poll the keyboard port until it's stable

.proc   Poll
        ldy     #TPI::PRC
L1:     lda     (tpi2),y
        sta     keysave
        lda     (tpi2),y
        cmp     keysave
        bne     L1
        rts
.endproc




