;
; Mariano Domínguez
; 2022-12-4
;
; atari lib
;
        .include "atari.inc"
        .export         __sound
        .import         popa
        .importzp       tmp1,tmp2

; play sound, arguments: voice, pitch, distortion, volume. same as BASIC
.proc   __sound
        sta tmp2        ;save volume
        jsr popa        ;get distortion
        sta tmp1        ;save distortion
        jsr popa        ;get pitch
        pha             ;save in stack
        jsr popa        ;get voice
        asl a           ;adjust voice *2 for offset in x
        tax
        pla             ;get pitch from stack
        sta AUDF1,x     ;store pitch
        lda #0
        sta AUDCTL
        lda #3
        sta SKCTL       ;init sound
        lda tmp1        ;get distortion
        asl a           ;ignore the high nibble
        asl a
        asl a
        asl a
        clc             ;setup for adding volume
        adc tmp2        ;add volume
        sta AUDC1,x     ;volume + distortion in control channel
        rts
.endproc
