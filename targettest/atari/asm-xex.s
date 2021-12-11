; Sample using ATARI file format, by "atari-xex.cfg" linker configuration.
;
; This is a very simple example, shows a message to the screen, waits and
; returns to DOS.
;
; Compile with:
;    cl65 -tatari -Catari-xex.cfg asm-xex.s -o prog.xex

        .include        "atari.inc"

; Default RUNAD is "start", export that:
        .export         start


; Write string to screen
.proc   puts
        sta     ICBAL
        stx     ICBAH
        lda     #PUTREC
        sta     ICCOM
        ldx     #$FF
        stx     ICBLL
        inx
        stx     ICBLH
        jsr     CIOV
        rts
.endproc


; Write a message and exit

.proc   start
        lda     #<msg
        ldx     #>msg
        jsr     puts


        ; Delay before returning to DOS
        lda     #0
        tax
loop:
        inx
        cpx     #$FF
        adc     #0
        bcc     loop

        rts
.endproc

msg:    .byte   "Hello world", ATEOL

