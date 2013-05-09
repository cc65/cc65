;
; Maciej 'YTM/Elysium' Witkowiak
;
; 30.10.99, 15.8.2003

; void Sleep (int jiffies);

            .importzp ptr1
            .importzp tmp1
            .export _Sleep

            .include "jumptab.inc"
            .include "geossym.inc"

_SleepExit:
        jsr Sleep               ; call Sleep
        ldx tmp1
        txs                     ; restore stack pointer
        jmp (ptr1)              ; when timeouts control will reach here

_Sleep:
        sta r0L                 ; store data
        stx r0H
        pla
        sta ptr1
        pla
        sta ptr1+1              ; preserve return address
        inc ptr1
        bne @L0
        inc ptr1+1              ; fix return address
@L0:    tsx
        stx tmp1                ; preserve stack pointer
        jsr _SleepExit          ; call Sleep
        jmp MainLoop            ; immediate return here - go to idle loop
