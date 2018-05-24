;
; Christian Krueger, 23-May-2018
;
; CC65 runtime: helper call for mod/div with signed ints
;
; When negating values, we will ignore the possibility here, that one of the
; values is $8000, in which case the negate will fail.

        .export         absvaludiv
        .import         _abs, popax, udiv16
        .importzp       ptr1, ptr4


absvaludiv:
        jsr     _abs
        sta     ptr4
        stx     ptr4+1          ; Save right absolute operand      
        jsr     popax
        jsr     _abs
        sta     ptr1
        stx     ptr1+1          ; Save left absolute operand  
        jmp     udiv16
