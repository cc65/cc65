;
; Stefan Haubenthal, 2005-12-24
; Christian Groessler, 2013-07-16
;
; unsigned char __fastcall__ _sysrmdir (const char* name);
;
; for SpartaDOS and MyDOS
;

        .include        "atari.inc"
        .export         __sysrmdir
        .import         __sysremove
        .import         __dos_type
        .import         findfreeiocb
        .importzp       tmp4
.ifdef  UCASE_FILENAME
        .import         ucase_fn
        .import         addysp
        .importzp       tmp3
.ifdef  DEFAULT_DEVICE
        .importzp tmp2
.endif
.endif

.proc   __sysrmdir

        pha
        lda     __dos_type
        beq     not_impl                ; AtariDOS
        cmp     #OSADOS+1
        bcc     do_sparta               ; OS/A and SpartaDOS
        pla
        jmp     __sysremove             ; MyDOS and others (TODO: check XDOS)

not_impl:
        pla
        lda     #NVALID
        rts

iocberr:
        pla                             ; cleanup stack
        pla
        lda     #TMOF
        rts

do_sparta:
        txa
        pha
        jsr     findfreeiocb
        bne     iocberr                 ; no IOCB available

        stx     tmp4                    ; remember IOCB
        pla
        tax
        pla

.ifdef  UCASE_FILENAME

.ifdef  DEFAULT_DEVICE
        ldy     #$80
        sty     tmp2            ; set flag for ucase_fn
.endif
        jsr     ucase_fn
        bcc     ucok1

        lda     #183                    ; see oserror.s
        rts
ucok1:

.endif  ; defined UCASE_FILENAME

        ldy     tmp4                    ; IOCB index
        sta     ICBAL,y                 ; store pointer to filename
        txa
        sta     ICBAH,y
        tya
        tax
        lda     #RMDIR
        sta     ICCOM,x
        lda     #0
        sta     ICAX1,x
        lda     #0
        sta     ICAX2,x
        sta     ICBLL,x
        sta     ICBLH,x
        jsr     CIOV

.ifdef  UCASE_FILENAME
        tya
        pha
        ldy     tmp3                    ; get size
        jsr     addysp                  ; free used space on the stack
        pla
        tay
.endif  ; defined UCASE_FILENAME

        bmi     cioerr
        lda     #0
        rts

cioerr: tya
        rts

.endproc        ; __sysrmdir
