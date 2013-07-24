;
; Stefan Haubenthal, 2005-12-24
; Based on on code by Christian Groessler
;
; unsigned char __fastcall__ _syschdir (const char* name);
; for SpartaDOS and MyDOS
;

        .include "atari.inc"
        .import findfreeiocb
        .import __dos_type
        .import initcwd
        .importzp tmp4
.ifdef  UCASE_FILENAME
        .importzp tmp3
        .import addysp
        .import ucase_fn
.ifdef  DEFAULT_DEVICE
        .importzp tmp2
.endif
.endif
        .export __syschdir

.proc   __syschdir

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
        lda     __dos_type
        cmp     #SPARTADOS
        beq     :+
        lda     #CHDIR_MYDOS
        .byte   $2C             ; BIT <abs>
:       lda     #CHDIR_SPDOS
        sta     ICCOM,x
        lda     #0
        sta     ICAX1,x
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
        jsr     initcwd
        lda     #0
        rts
cioerr: tya
        rts

.endproc        ; __syschdir
