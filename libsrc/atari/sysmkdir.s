;
; Initial version: Stefan Haubenthal, 2005-12-24
; Some fixes: Christian Groessler, 2009-01-28
;
; unsigned char _sysmkdir (const char* name, ...);
; for SpartaDOS and MYDOS
;

        .include "atari.inc"
        .include "errno.inc"
        .import addysp
        .import popax
        .import findfreeiocb
        .importzp tmp4
.ifdef  UCASE_FILENAME
        .importzp tmp3
        .import ucase_fn
.ifdef  DEFAULT_DEVICE
        .importzp tmp2
.endif
.endif
        .export __sysmkdir

.proc   __sysmkdir

        dey                     ; parm count < 2 shouldn't be needed to be...
        dey                     ; ...checked (it generates a C compiler warning)
        beq     parmok          ; branch if parameter count ok
        jsr     addysp          ; fix stack, throw away unused parameters

parmok: jsr     popax           ; get name
        pha                     ; save input parameter
        txa
        pha

        jsr     findfreeiocb
        beq     iocbok          ; we found one

        pla
        pla                     ; fix up stack

        lda     #TMOF           ; too many open files
        rts

iocbok: stx     tmp4            ; remember IOCB index
        pla
        tax
        pla                     ; get argument again

.ifdef  UCASE_FILENAME

.ifdef  DEFAULT_DEVICE
        ldy     #$80
        sty     tmp2            ; set flag for ucase_fn
.endif
        jsr     ucase_fn
        bcc     ucok1

        lda     #183            ; see oserror.s
        rts
ucok1:

.endif  ; defined UCASE_FILENAME

        ldy     tmp4            ; IOCB index
        sta     ICBAL,y         ; store pointer to filename
        txa
        sta     ICBAH,y
        tya
        tax
        lda     #MKDIR
        sta     ICCOM,x
        lda     #8
        sta     ICAX1,x
        lda     #0
        sta     ICAX2,x
        sta     ICBLL,x
        sta     ICBLH,x
        jsr     CIOV

.ifdef  UCASE_FILENAME
        tya
        pha
        ldy     tmp3            ; get size
        jsr     addysp          ; free used space on the stack
        pla
        tay
.endif  ; defined UCASE_FILENAME

        bmi     cioerr
        lda     #0
        rts
cioerr: tya
        rts

.endproc        ; __sysmkdir
