;
; Christian Groessler, 2013-07-24
;
; unsigned char __fastcall__ _sysrename (const char* oldname, const char* newname);
;

        .include "atari.inc"
        .import findfreeiocb
        .importzp tmp4, sp, ptr2, ptr3
        .import incsp2, subysp, addysp, popax
.ifdef  UCASE_FILENAME
        .importzp tmp3
        .import ucase_fn
.ifdef  DEFAULT_DEVICE
        .importzp tmp2
.endif
.endif
        .export __sysrename

.proc   __sysrename

        pha                     ; save input parameter
        txa
        pha

        jsr     findfreeiocb
        beq     iocbok          ; we found one

        pla
        pla                     ; fix up stack
        jsr     incsp2

        lda     #TMOF           ; too many open files
        rts

iocbok: stx     tmp4            ; remember IOCB index

        pla
        sta     ptr2+1          ; remember newname
        pla
        sta     ptr2            ; ditto.

        jsr     popax           ; get oldname

        ldy     #0
        sty     sspc+1          ; initialize stack space

.ifdef  UCASE_FILENAME

; uppercase first (old) name and prepend device if needed

.ifdef  DEFAULT_DEVICE
        ldy     #$80
        sty     tmp2            ; set flag for ucase_fn
.endif
        jsr     ucase_fn
        bcc     ucok1

        lda     #183            ; see oserror.s
        rts

ucok1:  sta     ptr3
        stx     ptr3+1          ; remember pointer to uppercased old name
        lda     tmp3            ; # of bytes reserved on the stack
        sta     sspc            ; remember...

; uppercase second (new) name and don't prepend device

.ifdef  DEFAULT_DEVICE
        ldy     #0
        sty     tmp2            ; set flag for ucase_fn
.endif
        lda     ptr2
        ldx     ptr2+1

        jsr     ucase_fn
        bcc     ucok2

        ldy     tmp3            ; get size
        jsr     addysp          ; free used space on the stack
        lda     #183            ; see oserror.s
        rts

ucok2:  sta     ptr2            ; remember pointer to uppercased new name
        stx     ptr2+1

; update sspc -- # of bytes used on the stack

        lda     sspc
        clc
        adc     tmp3
        sta     sspc
        bcc     ukok4
        inc     sspc+1
ukok4:

.else

        sta     ptr3
        stx     ptr3+1
        sty     sspc

.endif

; create a string on the stack with the old filename and the new filename separated by an invalid character (space in our case)
; ptr2 - pointer to new name
; ptr3 - pointer to old name

        lda     #128
        tay
        clc
        adc     sspc
        sta     sspc
        bcc     L1
        inc     sspc+1
L1:     jsr     subysp          ; make room on the stack

; copy old name
        ldy     #0
con:    lda     (ptr3),y
        sta     (sp),y
        beq     copyend
        iny
        bne     con

copyend:lda     #$20            ; space
        sta     (sp),y
        iny
        tya                     ; get current offset (beyond old name)
        clc
        adc     sp
        sta     ptr3
        lda     sp+1
        adc     #0
        sta     ptr3+1          ; ptr3 now contains pointer to space for new filename

; copy new name
        ldy     #0
cnn:    lda     (ptr2),y
        sta     (ptr3),y
        beq     copend2
        iny
        bne     cnn

copend2:ldx     tmp4
        lda     sp
        sta     ICBAL,x
        lda     sp+1
        sta     ICBAH,x
        lda     #RENAME
        sta     ICCOM,x
        lda     #0
        sta     ICAX1,x
        sta     ICAX2,x
        sta     ICBLL,x
        sta     ICBLH,x
        jsr     CIOV
        tya
        pha

; clean up stack

        lda     sp
        clc
        adc     sspc
        sta     sp
        lda     sp+1
        adc     sspc+1
        sta     sp+1

; handle status

        pla
        tay
        bmi     cioerr
        lda     #0
        rts
cioerr: tya
        rts

.endproc        ; __sysrename

        .bss

sspc:   .res    2               ; stack space used
