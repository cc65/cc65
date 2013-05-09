;
; Ullrich von Bassewitz, 31.05.1998
;
; char* strcat (char* dest, const char* src);
;

        .export         _strcat
        .import         popax
        .importzp       ptr1, ptr2, tmp3

_strcat:
        sta     ptr1            ; Save src
        stx     ptr1+1
        jsr     popax           ; Get dest
        sta     ptr2
        stx     ptr2+1
        sta     tmp3            ; Remember for function return
        ldy     #0

; find end of dest

sc1:    lda     (ptr2),y
        beq     sc2
        iny
        bne     sc1
        inc     ptr2+1
        bne     sc1

; end found, get offset in y into pointer

sc2:    tya
        clc
        adc     ptr2
        sta     ptr2
        bcc     sc3
        inc     ptr2+1

; copy src

sc3:    ldy     #0
sc4:    lda     (ptr1),y
        sta     (ptr2),y
        beq     sc5
        iny
        bne     sc4
        inc     ptr1+1
        inc     ptr2+1
        bne     sc4

; done, return pointer to dest

sc5:    lda     tmp3            ; X does still contain high byte
        rts


