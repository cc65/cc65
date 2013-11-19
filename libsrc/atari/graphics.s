;
; Christian Groessler, June 2004
;
; this file provides an equivalent to the BASIC GRAPHICS function
;
; int __fastcall__ _graphics(unsigned char mode);
;
;

        .export __graphics

        .import findfreeiocb
        .import __oserror
        .import fddecusage
        .import clriocb
        .import fdtoiocb
        .import newfd
        .import scrdev
        .importzp tmp1,tmp2,tmp3

        .include        "atari.inc"
        .include        "errno.inc"

        .code

; set new grapics mode
; gets new mode in A
; returns handle or -1 on error
; uses tmp1, tmp2, tmp3, tmp4 (in subroutines)

.proc   __graphics

;       tax
;       and     #15             ; get required graphics mode
;       cmp     #12
;       bcs     invmode         ; invalid mode
;       txa
;       and     #$c0            ; invalid bits set?
;       bne     invmode

;       stx     tmp1
        sta     tmp1            ; remember graphics mode

parmok: jsr     findfreeiocb
        beq     iocbok          ; we found one

        lda     #<EMFILE        ; "too many open files"
seterr: jsr     __mappederrno   ; @@@ probably not correct to set errno here @@@
        rts                     ; return -1

;invmode:ldx    #>EINVAL
;       lda     #<EINVAL
;       bne     seterr

iocbok: txa
        tay                     ; move iocb # into Y
        lda     #3
        sta     tmp3            ; name length + 1
        lda     #<scrdev
        ldx     #>scrdev
        jsr     newfd
        tya
        tax
        bcs     doopen          ; C set: open needed

        ldx     #0
        lda     tmp2            ; get fd used
        jsr     fdtoiocb
        tax

doopen: txa
        ;brk
        pha
        jsr     clriocb
        pla
        tax
        lda     #<scrdev
        sta     ICBAL,x
        lda     #>scrdev
        sta     ICBAH,x
        lda     #OPEN
        sta     ICCOM,x
        lda     tmp1            ; get requested graphics mode
        and     #15
        sta     ICAX2,x
        lda     tmp1
        and     #$30
        eor     #$10
        ora     #12
        sta     ICAX1,x

        jsr     CIOV
        bmi     cioerr

        lda     tmp2            ; get fd
        ldx     #0
        stx     __oserror
        rts

cioerr: sty     tmp3            ; remember error code
        lda     #CLOSE
        sta     ICCOM,x
        jsr     CIOV            ; close IOCB again since open failed
        jsr     fddecusage      ; and decrement usage counter of fd
        lda     tmp3            ; put error code into A
        jmp     __mappederrno

.endproc        ; __graphics
