;
; Robert Justice
;
; unsigned char kbhit (void);
;

        .export _kbhit, statcons
        .import consdev

        .include "apple3.inc"
        .include "sos.inc"

_kbhit:
        lda     #STAT_KEYSTROKE_COUNT

statcons:
        sta     statlist+2
        lda     consdev
        sta     statlist+1

        brk
        .byte   D_STATUS_CALL
        .addr   statlist

        lda     statresult
        ldx     #>$0000
        rts

        .data

;status param list
statlist:
        .byte   3
        .byte   0             ; dev_num
        .byte   0
        .addr   statresult

statresult:
        .byte   0