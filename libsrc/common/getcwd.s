;
; Ullrich von Bassewitz, 2003-08-12
;
; char* __fastcall__ getcwd (char* buf, size_t size);
;

        .export         _getcwd

        .import         popptr1
        .import         __cwd
        .importzp       ptr1, ptr2

        .include        "errno.inc"


;--------------------------------------------------------------------------

.proc   _getcwd

; Remember -size-1 because this simplifies the following loop

        eor     #$FF
        sta     ptr2
        txa
        eor     #$FF
        sta     ptr2+1

        jsr     popptr1         ; Get buf to ptr1

; Copy __cwd to the given buffer checking the length

        ; ldy     #$00          is guaranteed by popptr1
loop:   inc     ptr2
        bne     @L1
        inc     ptr2+1
        beq     overflow

; Copy one character, end the loop if the zero terminator is reached. We
; don't support directories longer than 255 characters for now.

@L1:    lda     __cwd,y
        sta     (ptr1),y
        beq     done
        iny
        bne     loop

; For some reason the cwd is longer than 255 characters. This should not
; happen, we handle it as if the passed buffer was too short.
;
; String overflow, return ERANGE

overflow:
        lda     #<ERANGE
        jsr     __seterrno      ; Returns 0 in A
        tax                     ; Return zero
        rts                        

; Success, return buf

done:   lda     ptr1
        ldx     ptr1+1
        rts

.endproc


