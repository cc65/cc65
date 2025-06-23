;
; Christian Groessler, Jan-2003
;
; int open(const char *name,int flags,...);
;

        .include "atari.inc"
        .include "fcntl.inc"
        .include "errno.inc"
        .include "fd.inc"
        .include "zeropage.inc"

        .export _open
        .destructor     closeallfiles, 5

        .import _close
        .import clriocb
        .import fddecusage,newfd
        .import findfreeiocb
        .import incsp4
        .import ldaxysp,addysp
        .import ___oserror, returnFFFF
.ifdef  UCASE_FILENAME
        .import ucase_fn
.endif

.proc   _open

        dey                     ; parm count < 4 shouldn't be needed to be checked
        dey                     ;       (it generates a c compiler warning)
        dey
        dey
        beq     parmok          ; parameter count ok
        jsr     addysp          ; fix stack, throw away unused parameters

parmok: jsr     findfreeiocb
        beq     iocbok          ; we found one

        lda     #<EMFILE        ; "too many open files"
seterr: jsr     ___directerrno
        jsr     incsp4          ; clean up stack
        jmp     returnFFFF

        ; process the mode argument

iocbok: stx     tmp4
        jsr     clriocb         ; init with zero
        ldy     #1
        jsr     ldaxysp         ; get mode
        ldx     tmp4
        pha
        and     #O_APPEND
        beq     no_app
        pla
        and     #15
        cmp     #O_RDONLY       ; DOS supports append with write-only only
        beq     invret
        cmp     #O_RDWR
        beq     invret
        lda     #OPNOT|APPEND
        bne     set

.ifndef UCASE_FILENAME
invret: lda     #<EINVAL        ; file name is too long
        jmp     seterr
.endif

no_app: pla
        and     #15
        cmp     #O_RDONLY
        bne     l1
        lda     #OPNIN
set:    sta     ICAX1,x
        bne     cont

l1:     cmp     #O_WRONLY
        bne     l2
        lda     #OPNOT
        bne     set

l2:     ; O_RDWR
        lda     #OPNOT|OPNIN
        bne     set

        ; process the filename argument

cont:   ldy     #3
        jsr     ldaxysp

.ifdef  UCASE_FILENAME
.ifdef  DEFAULT_DEVICE
        ldy     #$80
.else
        ldy     #$00
.endif
        sty     tmp2            ; set flag for ucase_fn
        jsr     ucase_fn
        bcc     ucok1
invret: lda     #<EINVAL        ; file name is too long
        jmp     seterr
ucok1:

.endif  ; defined UCASE_FILENAME

        ldy     tmp4

        ;AX - points to filename
        ;Y  - iocb to use, if open needed
        jsr     newfd           ; maybe we don't need to open and can reuse an iocb
                                ; returns fd num to use in tmp2, all regs unchanged
        bcs     doopen          ; C set: open needed
        lda     #0              ; clears N flag
        beq     finish

doopen: sta     ICBAL,y
        txa
        sta     ICBAH,y
        ldx     tmp4
        lda     #OPEN
        sta     ICCOM,x
        jsr     CIOV

        ; clean up the stack

finish: php
        txa
        pha
        tya
        pha

.ifdef  UCASE_FILENAME
        ldy     tmp3            ; get size
        jsr     addysp          ; free used space on the stack
.endif  ; defined UCASE_FILENAME

        jsr     incsp4          ; clean up stack

        pla
        tay
        pla
        tax
        plp

        bpl     ok
        sty     tmp3            ; remember error code
        lda     #CLOSE
        sta     ICCOM,x
        jsr     CIOV            ; close IOCB again since open failed
        jsr     fddecusage      ; and decrement usage counter of fd
        lda     tmp3            ; put error code into A
        jmp     ___mappederrno

ok:     lda     tmp2            ; get fd
        ldx     #0
        stx     ___oserror
        rts

.endproc


; closeallfiles: Close all files opened by the program.

.proc   closeallfiles

        lda     #MAX_FD_INDEX-1
loop:   ldx     #0
        pha
        jsr     _close
        pla
        clc
        sbc     #0
        bpl     loop
        rts

.endproc
