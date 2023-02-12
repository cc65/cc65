;
; Stefan Haubenthal, 2005-06-08
;
; CBM _curunit to _cwd
;

        .export         initcwd, devicestr
        .import         curunit, __cwd
        .importzp       tmp1, sreg, ptr2

initcwd:
        lda     #<__cwd
        ldx     #>__cwd
        sta     ptr2
        stx     ptr2+1
        lda     curunit
        ; Fall through

;------------------------------------------------------------------------------
; Convert unit number in A into string representation pointed to by ptr2.

devicestr:
; Divide by 10 by "Omegamatrix"
; 17 bytes, 30 cycles
        lsr
        sta  tmp1
        lsr
        adc  tmp1
        ror
        lsr
        lsr
        adc  tmp1
        ror
        adc  tmp1
        ror
        lsr
        lsr

        ldy     #0
        tax                     ; result of the division (lsb)
        beq     @L0             ; < 10

        clc
        adc     #'0'
        sta     (ptr2),y
        iny
@L0:
        lda     sreg            ; reminder of the division
        clc
        adc     #'0'
        sta     (ptr2),y
        iny

        lda     #0              ; terminating 0
        sta     (ptr2),y
        rts
