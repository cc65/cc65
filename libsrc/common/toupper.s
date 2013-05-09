;
; Ullrich von Bassewitz, 02.06.1998
;
; int toupper (int c);
;

        .export         _toupper
        .import         __ctype

_toupper:
        cpx     #$00            ; Outside valid range?
        bne     L9              ; If so, return the argument unchanged
        tay                     ; Get c into Y
        lda     __ctype,y       ; Get character classification
        lsr     a               ; Get bit 0 (lower char) into carry
        tya                     ; Get C back into A
        bcc     L9              ; Jump if not lower char
        clc
        adc     #<('A'-'a')     ; make upper case char
L9:     rts                     ; CC are set

