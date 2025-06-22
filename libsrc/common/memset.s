;
; void* __fastcall__ memset (void* ptr, int c, size_t n);
; void* __fastcall__ __bzero (void* ptr, size_t n);
; void __fastcall__ bzero (void* ptr, size_t n);
;
; Ullrich von Bassewitz, 29.05.1998
; Performance increase (about 20%) by
; Christian Krueger, 12.09.2009, slightly improved 12.01.2011
;
; NOTE: bzero will return it's first argument as memset does. It is no problem
;       to declare the return value as void, since it may be ignored. __bzero
;       (note the leading underscores) is declared with the proper return type,
;       because the compiler will replace memset by __bzero if the fill value
;       is zero, and the optimizer looks at the return type to see if the value
;       in a/x is of any use.
;

        .export         _memset, _bzero, ___bzero
        .import         popax
        .importzp       c_sp, ptr1, ptr2, ptr3

_bzero:
___bzero:
        sta     ptr3
        stx     ptr3+1          ; Save n
        ldx     #0              ; Fill with zeros
        beq     common

_memset:
        sta     ptr3            ; Save n
        stx     ptr3+1
        jsr     popax           ; Get c
        tax

; Common stuff for memset and bzero from here

common:                         ; Fill value is in X!
        ldy     #1
        lda     (c_sp),y
        sta     ptr1+1          ; save high byte of ptr
        dey                     ; Y = 0
        lda     (c_sp),y        ; Get ptr
        sta     ptr1

        lsr     ptr3+1          ; divide number of
        ror     ptr3            ; bytes by two to increase
        bcc     evenCount       ; speed (ptr3 = ptr3/2)
oddCount:
                                ; y is still 0 here
        txa                     ; restore fill value
        sta     (ptr1),y        ; save value and increase
        inc     ptr1            ; dest. pointer
        bne     evenCount
        inc     ptr1+1
evenCount:
        lda     ptr1            ; build second pointer section
        clc
        adc     ptr3            ; ptr2 = ptr1 + (length/2) <- ptr3
        sta     ptr2
        lda     ptr1+1
        adc     ptr3+1
        sta     ptr2+1

        txa                     ; restore fill value
        ldx     ptr3+1          ; Get high byte of n
        beq     L2              ; Jump if zero

; Set 256/512 byte blocks
                                ; y is still 0 here
L1:     .repeat 2               ; Unroll this a bit to make it faster
        sta     (ptr1),y        ; Set byte in lower section
        sta     (ptr2),y        ; Set byte in upper section
        iny
        .endrepeat
        bne     L1
        inc     ptr1+1
        inc     ptr2+1
        dex                     ; Next 256 byte block
        bne     L1              ; Repeat if any

; Set the remaining bytes if any

L2:     ldy     ptr3            ; Get the low byte of n
        beq     leave           ; something to set? No -> leave

L3:     dey
        sta     (ptr1),y                ; set bytes in low
        sta     (ptr2),y                ; and high section
        bne     L3              ; flags still up to date from dey!
leave:
        jmp     popax           ; Pop ptr and return as result


