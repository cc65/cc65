;
; Christian Groessler, May-2000
;
; int __fastcall__ close(int fd);
;

        .include "atari.inc"
        .export _close
        .import __do_oserror,popax,__oserror
        .import fdtoiocb_down,__inviocb

.proc   _close
        jsr     fdtoiocb_down           ; get iocb index into X and decr. usage count
        bmi     inverr
        bne     ok                      ; not last one -> don't close yet
        lda     #CLOSE
        sta     ICCOM,x
        jsr     CIOV
        bmi     closerr
ok:     ldx     #0
        stx     __oserror               ; clear system specific error code
        txa
        rts

inverr: jmp     __inviocb
closerr:jmp     __do_oserror

.endproc

