;
; Ullrich von Bassewitz, 31.05.1998
; Christian Krueger: 2013-Jul-24, minor optimizations
;
; char* strcat (char* dest, const char* src);
;

        .export         _strcat
        .import         popax
        .importzp       ptr1, ptr2, tmp3

_strcat:
        sta ptr1        ; Save src
        stx ptr1+1
        jsr popax       ; Get dest
        sta tmp3        ; Remember for function return
        tay
.if .cap(CPU_HAS_STZ)
        stz ptr2
.else
        lda #0
        sta ptr2        ; access from page start, y contains low byte
.endif
        stx ptr2+1

findEndOfDest:
        lda (ptr2),y
        beq endOfDestFound
        iny
        bne findEndOfDest
        inc ptr2+1
        bne findEndOfDest

endOfDestFound:
        sty ptr2        ; advance pointer to last y position
        ldy #0          ; reset new y-offset

copyByte:
        lda (ptr1),y
        sta (ptr2),y
        beq done
        iny
        bne copyByte
        inc ptr1+1
        inc ptr2+1
        bne copyByte    ; like bra here

; return pointer to dest
done:   lda tmp3        ; X does still contain high byte
        rts
