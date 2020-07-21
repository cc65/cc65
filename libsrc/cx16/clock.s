;
; 1998-09-21, Ullrich von Bassewitz
; 2019-12-25, Greg King
;
; clock_t clock (void);
;

        .constructor    initclock
        .export         _clock

        .import         SETTIM, RDTIM
        .importzp       sreg


; clock() counts the amount of time that the process has run.
; Therefore, reset it when the program begins.

.proc   initclock

        lda     #$00
        tax
        tay
        jmp     SETTIM

.endproc


.proc   _clock

        stz     sreg + 1        ; Byte 3 always is zero
        jsr     RDTIM
        sty     sreg
        rts

.endproc
