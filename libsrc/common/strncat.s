;
; Ullrich von Bassewitz, 31.05.1998
; Christian Krueger: 12-Aug-2013, minor optimizations
;
; char* strncat (char* dest, const char* src, size_t n);
;

        .export         _strncat
        .import         popax, popptr1
        .importzp       ptr1, ptr2, ptr3, tmp1, tmp2
        .macpack        cpu
        
_strncat:
    eor #$FF        ; one's complement to count upwards
    sta tmp1
    txa
    eor #$FF
    sta tmp2
    
    jsr popptr1     ; get src

    jsr popax       ; get dest
    sta ptr3        ; remember for function return
    stx ptr3+1          
    stx ptr2+1
    tay             ; low byte as offset in Y
.if (.cpu .bitand ::CPU_ISET_65SC02)
    stz ptr2
.else
    ldx #0
    stx ptr2        ; destination on page boundary
.endif

; find end of dest

L1: lda (ptr2),y
    beq L2
    iny
    bne L1
    inc ptr2+1
    bne L1

; end found, apply offset to dest ptr and reset y
L2: sty ptr2

; copy src. We've put the ones complement of the count into the counter, so
; we'll increment the counter on top of the loop

L3: ldy #0
    ldx tmp1        ; low counter byte

L4: inx
    bne L5
    inc tmp2
    beq L6          ; jump if done
L5: lda (ptr1),y
    sta (ptr2),y
    beq L7
    iny
    bne L4
    inc ptr1+1
    inc ptr2+1
    bne L4

; done, set the trailing zero and return pointer to dest

L6: lda #0
    sta (ptr2),y
L7: lda ptr3
    ldx ptr3+1
    rts
