;
; common iocb setup routine for read, write
; expects __fastcall__ parameters (int fd, void *buf, int count)
;
        .include "atari.inc"
        .include "errno.inc"
        .import popax
        .import fdtoiocb

        .export __rwsetup


__rwsetup:

        pha                     ; push size in stack
        txa
        pha
        jsr     popax           ; get buffer address
        pha
        txa
        pha
        jsr     popax           ; get handle
        jsr     fdtoiocb        ; convert to iocb
        bmi     iocberr         ; negative (X=$FF or A>$7F) on error.
        tax
        pla                     ; store address
        sta     ICBAH,x
        pla
        sta     ICBAL,x
        pla                     ; store length
        sta     ICBLH,x
        pla
        sta     ICBLL,x
        ora     ICBLH,x         ; returns Z if length is 0
        rts

iocberr:pla
        pla
        pla
        pla
        ldx     #$FF            ; indicate error + clear ZF
        rts

