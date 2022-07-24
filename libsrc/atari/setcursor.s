;
; Christian Groessler, November-2002
;
; cursor handling, internal function

        .include "atari.inc"
        .import cursor,_mul40
        .export setcursor

.proc   setcursor

        ldy     #0
        lda     OLDCHR
        sta     (OLDADR),y

        lda     ROWCRS
        jsr     _mul40          ; function leaves with carry clear!
        adc     SAVMSC          ; add start of screen memory
        sta     OLDADR
        txa
        adc     SAVMSC+1
        sta     OLDADR+1
        lda     COLCRS
        adc     OLDADR
        sta     OLDADR
        bcc     nc
        inc     OLDADR+1
nc:     lda     (OLDADR),y
        sta     OLDCHR

        ldx     cursor          ; current cursor setting as requested by the user
        beq     off
        ldx     #0
        beq     cont

off:    inx
cont:   stx     CRSINH          ; update system variable

        beq     turnon
        and     #$7f            ; clear high bit / inverse flag
finish: sta     (OLDADR),y      ; update on-screen display
        rts

turnon: ora     #$80            ; set high bit / inverse flag
        bne     finish

.endproc
