;
; Ullrich von Bassewitz, 23.09.1998
; Adapted for Vic20 by Steve Schmidtke 05.08.2002
;
; unsigned readjoy (unsigned char joy);
;

        .export         _readjoy

        .include        "vic20.inc"


; The current implemenation will ignore the joystick number because we do only
; have one joystick


.proc   _readjoy

        lda     #$C3            ; mask for VIA1 JOYBITS: sw0,sw1,sw2,sw4
        ldx     #$7F            ; mask for VIA2 JOYBIT: sw3
        sei                     ; necessary?

        ldy     VIA1_DDRA       ; remember the state of DDRA
        sta     VIA1_DDRA       ; set JOYBITS on this VIA for input
        lda     VIA1_JOY        ; read JOYBITS: sw0,sw1,sw2,sw4
        sty     VIA1_DDRA       ; restore the state of DDRA
        and     #$3C            ; Mask relevant bits...
        sta     tmp1            ; ...and save for later

        ldy     VIA2_DDRB       ; remember the date of DDRB
        stx     VIA2_DDRB       ; set JOYBITS on this VIA for input
        lda     VIA2_JOY        ; read JOYBIT: sw3
        sty     VIA2_DDRB       ; restore the state of DDRB

        cli                     ; necessary?
        and     #$80            ; Mask sw3
        ora     tmp1            ; Get sw0, sw1, sw2, sw4
        eor     #$BC            ; Active states are inverted

        rts

.endproc

