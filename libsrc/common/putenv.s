;
; Ullrich von Bassewitz, 2005-04-21
;
; int putenv (char* s);
;
; Note: The function will place s into the environment, *not* a copy!
;

        .export _putenv
        .import _malloc, _free
        .import searchenv, copyenvptr
        .import __environ, __envcount, __envsize
        .import return0, ___directerrno
        .import ptr1:zp, ptr2:zp, ptr3:zp, tmp1:zp

        .include "errno.inc"

.code

;----------------------------------------------------------------------------
; putenv()

.proc   _putenv

        sta     ptr1
        sta     name
        stx     ptr1+1                  ; Save name
        stx     name+1

; Loop over the name to find the '='. If there is no '=', set errno to EINVAL
; and return an error.

        ldy     #$FF
@L0:    iny
        lda     (ptr1),y
        bne     @L1
        lda     #EINVAL
        jmp     error                   ; End of string without '=' found
@L1:    cmp     #'='
        bne     @L0

; Remember the offset of the equal sign and replace it by a zero.

        sty     tmp1
        lda     #$00
        sta     (ptr1),y

; Search for the string in the environment. searchenv will set the N flag if
; the string is not found, otherwise X contains the index of the entry, ptr2
; contains the entry and Y the offset of the '=' in the string. ptr3 will
; point to the environment.

        jsr     searchenv

; Before doing anything else, restore the old environment string.

        ldy     tmp1
        lda     #'='
        sta     (ptr1),y

; Check the result of searchenv

        txa                             ; Did we find the entry?
        bpl     addentry                ; Jump if yes

; We didn't find the entry, so we have to add a new one. Before doing so, we
; must check if the size of the _environ array must be increased.
; Note: There must be one additional slot for the final NULL entry.

        ldx     __envcount
        inx
        cpx     __envsize
        bcc     addnewentry             ; Jump if space enough

; We need to increase the size of the environ array. Calculate the new size.
; We will not support a size larger than 64 entries, double the size with
; each overflow, and the starting size is 8 entries.

        lda     __envsize
        bne     @L2
        lda     #4                      ; Start with 4*2 entries
@L2:    asl     a                       ; Double current size
        bmi     nomem                   ; Bail out if > 64
        sta     newsize                 ; Remember the new size

; Call malloc() and store the result in ptr2

        asl     a                       ; Make words
        ldx     #$00
        jsr     _malloc
        sta     ptr2
        stx     ptr2+1

; Check the result of malloc

        ora     ptr2+1
        beq     nomem

; Copy the old environment pointer to ptr3, and the new one to __environ.

        ldx     #1
@L3:    lda     __environ,x
        sta     ptr3,x
        lda     ptr2,x
        sta     __environ,x
        dex
        bpl     @L3

; Use the new size.

        lda     newsize
        sta     __envsize

; Copy the old environment data into the new space.

        lda     __envcount
        asl     a
        tay
        jmp     @L5
@L4:    lda     (ptr3),y
        sta     (ptr2),y
@L5:    dey
        bpl     @L4

; Free the old environment space

        lda     ptr3
        ldx     ptr3+1
        jsr     _free

; Since free() has destroyed ptr2, we need another copy ...

        jsr     copyenvptr              ; Copy __environ to ptr2

; Bump the environment count and remember it in X. Add the final NULL entry.

addnewentry:
        inc     __envcount
        ldx     __envcount
        txa
        asl     a
        tay
        lda     #$00
        sta     (ptr2),y
        iny
        sta     (ptr2),y

; The index of the new entry is the old environment count.

        dex
        txa

; Add the new entry to the slot with index in X. The pointer to the environment
; is already in ptr2, either by a call to searchenv, or by above code.

addentry:
        asl     a
        tay
        lda     name
        sta     (ptr2),y
        iny
        lda     name+1
        sta     (ptr2),y

; Done

        jmp     return0

; Error entries

nomem:  lda     #ENOMEM
error:  jmp     ___directerrno

.endproc


;----------------------------------------------------------------------------
; data

.bss

name:           .addr   0               ; Pointer to name
newsize:        .byte   0               ; New environment size
