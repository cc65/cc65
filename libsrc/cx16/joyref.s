;
; 2019-11-14, Greg King
;
; Link an interrupt handler if joysticks are used by a program.
;

        .interruptor    joy_libref, 9

        .include        "cbm_kernal.inc"
        .include        "cx16.inc"


joy_libref:
        lda     VERA::IRQ_FLAGS
        lsr     a
        bcc     not_vsync
        jsr     GETJOY          ; Bit-bang game controllers
        clc                     ; Let other Jiffy handlers run
not_vsync:
        rts
