;
; Ullrich von Bassewitz, 2005-04-21
;
; Search the environment for a string.
;

        .export searchenv, copyenvptr
        .import __environ, __envcount
        .import ptr1:zp, ptr2:zp, ptr3:zp

.code

;----------------------------------------------------------------------------
; searchenv:
;
; ptr1 must contain the string to search for. On exit, the N flag will tell
; if the entry was found, and X will contain the index of the environment
; string in the environment (a negative value if the entry was not found).
; On success, ptr3 will contain the entry and Y the offset of the '=' within
; the string.

.proc   searchenv

; Copy the pointer to the environment to the zero page

        jsr     copyenvptr

; Loop over all environment entries trying to find the requested one.

        ldx     __envcount
@L0:    dex
        bmi     @L9                     ; Out of entries

; Since the maximum number of entries is 64, the index can only be 63, so
; the following shift cannot overflow and the carry is clear.

        txa
        asl     a                       ; Mul by two for word access
        tay
        lda     (ptr2),y
        sta     ptr3
        iny
        lda     (ptr2),y
        sta     ptr3+1

; ptr1 points to name, ptr3 points to the next environment entry. Compare the
; two. The following loop limits the length of name to 255 bytes.

        ldy     #$00
@L1:    lda     (ptr1),y
        beq     @L2                     ; Jump on end of name
        cmp     (ptr3),y
        bne     @L0                     ; Next environment entry
        iny
        bne     @L1

; End of name reached, check if the environment entry contains a '=' char

@L2:    lda     (ptr3),y
        cmp     #'='
        bne     @L0                     ; Next environment entry

; Done. The function result is in X and the N flag is set correctly.

@L9:    rts

.endproc


;----------------------------------------------------------------------------
; copyenvptr: Copy _environ to ptr2
;

.proc   copyenvptr

        lda     __environ
        sta     ptr2
        lda     __environ+1
        sta     ptr2+1
        rts

.endproc


