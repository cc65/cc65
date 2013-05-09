;
; Ullrich von Bassewitz, 31.05.1998
;
; const char* strchr (const char* s, int c);
;

        .export         _strchr
        .import         popax
        .importzp       ptr1, tmp1

_strchr:
        sta     tmp1            ; Save c
        jsr     popax           ; get s
        sta     ptr1
        stx     ptr1+1
        ldy     #0

Loop:   lda     (ptr1),y        ; Get next char
        beq     EOS             ; Jump on end of string
        cmp     tmp1            ; Found?
        beq     Found           ; Jump if yes
        iny
        bne     Loop
        inc     ptr1+1
        bne     Loop            ; Branch always

; End of string. Check if we're searching for the terminating zero

EOS:    lda     tmp1            ; Get the char we're searching for
        bne     NotFound        ; Jump if not searching for terminator

; Found. Calculate pointer to c.

Found:  ldx     ptr1+1          ; Load high byte of pointer
        tya                     ; Low byte offset          
        clc
        adc     ptr1
        bcc     Found1
        inx
Found1: rts

; Not found, return NULL

NotFound:
        lda     #0
        tax
        rts

