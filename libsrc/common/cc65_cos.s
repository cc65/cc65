;
; Fixed point cosine function.
;
; Returns the cosine for the given argument as angular degree.
; Valid argument range is 0..360
;
;
; Ullrich von Bassewitz, 2009-10-29
;

        .export         _cc65_cos

        .import         _cc65_sin



; ---------------------------------------------------------------------------
;

.code

.proc   _cc65_cos         

; cos(x) = sin(x+90)

        clc
        adc     #90
        bcc     L1
        inx

; If x is now larger than 360, we need to subtract 360.

L1:     cpx     #>360
        bne     L2
        cmp     #<360
L2:     bcc     L4

        sbc     #<360
        bcs     L3
        dex
L3:     dex

L4:     jmp     _cc65_sin


.endproc


