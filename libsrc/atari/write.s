;
; int __fastcall__ write (int fd, const void* buf, unsigned count);
;
        .include "atari.inc"
        .import __rwsetup,__do_oserror,__inviocb,__oserror
        .export _write
_write:
        jsr     __rwsetup       ; do common setup
        beq     write9          ; if size 0, it's a no-op
        cpx     #$FF            ; invalid iocb?
        beq     _inviocb
        lda     #PUTCHR
        sta     ICCOM,x
        jsr     CIOV
        bpl     write9
        jmp     __do_oserror    ; update errno

write9:
        lda     ICBLL,x         ; get buf len low
        pha
        lda     ICBLH,x         ; buf len high
        tax
        lda     #0
        sta     __oserror       ; clear system dependend error code
        pla
        rts

_inviocb:
        jmp     __inviocb
