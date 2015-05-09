; serial.s

        .setcpu "c39-native"
        
        .include "c39.inc"
        .include "c39_sfrs.inc"

        .export _serial_putc
        .export _serial_getc
        .export _serial_getc_echo
        .export _serial_crlf

.proc _serial_putc
        bbr #BIT_SOUT_BUFFER_EMPTY, SERIAL_STATUS, _serial_putc
        sta SERIAL_BUF
        rts
.endproc

.proc _serial_getc
L1:     
        bbr #BIT_SIN_BUFFER_FULL, SERIAL_STATUS, _serial_getc
        lda SERIAL_BUF
        ldx #0                  ; in case int is expected
        rts
.endproc
        
.proc _serial_getc_echo
L1:     
        bbr #BIT_SIN_BUFFER_FULL, SERIAL_STATUS, L1
        lda SERIAL_BUF
        ldx #0                  ; in case int is expected
        
        ;; echo
L2:     
        bbr #BIT_SOUT_BUFFER_EMPTY, SERIAL_STATUS, L2
        sta SERIAL_BUF
        rts
.endproc
        
.proc _serial_crlf
        lda #$0D
L1:
        bbr #BIT_SOUT_BUFFER_EMPTY, SERIAL_STATUS, L1
        sta SERIAL_BUF

        lda #$0A
L2:     
        bbr #BIT_SOUT_BUFFER_EMPTY, SERIAL_STATUS, L2
        sta SERIAL_BUF
        rts
.endproc
