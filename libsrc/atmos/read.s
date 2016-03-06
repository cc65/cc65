;
; 2014-08-22, Greg King
;
; int read (int fd, void* buf, unsigned count);
;
; This function is a hack!  It lets us get text from the stdin console.
;

        .export         _read
        .constructor    initstdin

        .import         popax
        .importzp       ptr1, ptr2, ptr3
        .forceimport    disable_caps

        .macpack        generic
        .include        "atmos.inc"

.proc   _read

        sta     ptr3
        stx     ptr3+1          ; save count as result
        eor     #$FF
        sta     ptr2
        txa
        eor     #$FF
        sta     ptr2+1          ; Remember -count-1

        jsr     popax           ; get buf
        sta     ptr1
        stx     ptr1+1
        jsr     popax           ; get fd and discard

L1:     inc     ptr2
        bnz     L2
        inc     ptr2+1
        bze     L9              ; no more room in buf

; If there are no more characters in BASIC's input buffer, then get a line from
; the console into that buffer.

L2:     ldx     text_count
        bpl     L3
        jsr     GETLINE
        ldx     #<(0 - 1)

L3:     inx
        lda     BASIC_BUF,x
        bnz     L4              ; (zero-terminated buffer)
        ldx     #<-1
        lda     #$0A            ; return newline char. at end of line
L4:     stx     text_count
        ldy     #0
        sta     (ptr1),y
        inc     ptr1
        bnz     L1
        inc     ptr1+1
        bnz     L1              ; branch always

; No error, return count.

L9:     lda     ptr3
        ldx     ptr3+1
        rts

.endproc


;--------------------------------------------------------------------------
; initstdin:  Reset the stdin console.

.segment        "ONCE"

initstdin:
        ldx     #<-1
        stx     text_count
        rts


;--------------------------------------------------------------------------

.bss

text_count:
        .res    1

