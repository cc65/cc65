;
; Ullrich von Bassewitz, 2003-05-05
;
; void* __fastcall__ memchr (const void* p, int c, size_t n);
;

        .export         _memchr
        .import         popax, popptr1, return0
        .importzp       ptr1, ptr2


.proc   _memchr

        eor     #$FF
        sta     ptr2
        txa
        eor     #$FF
        sta     ptr2+1          ; Save ones complement of n
        jsr     popax           ; get c
        pha

        jsr     popptr1         ; get p

        ; ldy     #$00            is guaranteed by popptr1
        pla                     ; Get c
        ldx     ptr2            ; Use X as low counter byte

L1:     inx
        beq     L3
L2:     cmp     (ptr1),y
        beq     found
        iny
        bne     L1
        inc     ptr1+1
        bne     L1              ; Branch always

L3:     inc     ptr2+1          ; Bump counter high byte
        bne     L2

; Not found, return NULL

notfound:
        jmp     return0

; Found, return pointer to char

found:  ldx     ptr1+1          ; get high byte of pointer
        tya                     ; low byte offset
        clc
        adc     ptr1
        bcc     L9
        inx
L9:     rts

.endproc

