;
; Ullrich von Bassewitz, 15.09.2000
; Christian Krueger: 2013-Sep-16, minor speed optimization
;                                 regcall parameter passing 
;
; int __fastcall__ memcmp (const void* p1, const void* p2, size_t count);
; int __fastcall__ _rc_memcmp(void);
; ptr1 = const void* p1
; ptr2 = const void* p2
; ptr3 = size_t count
;

.export _memcmp
.export _rc_memcmp

.import popax
.import return0
.importzp   ptr1, ptr2, ptr3


_memcmp:
    sta ptr3
    stx ptr3+1
    
    jsr popax
    sta ptr2
    stx ptr2+1

    jsr popax
    sta ptr1
    stx ptr1+1

_rc_memcmp:
    ldy #0                  ; start with offset = 0
    ldx ptr3+1              ; check high byte of length (to X)
    beq handleLowByteLen
    
handleHighByteLen:          ; compare pages
nextPageByte:
    lda (ptr1),y
    cmp (ptr2),y
    bne notEqual            ; handle difference
    iny
    bne nextPageByte
    inc ptr1+1              ; advance to next page
    inc ptr2+1
    dex
    bne nextPageByte    
    
handleLowByteLen:           ; assert Y = 0 here!
    lda ptr3                ; check low byte of length
    beq equal               ; end reached, memory is equal

nextByte:    
    lda (ptr1),y
    cmp (ptr2),y
    bne notEqual
    iny
    cpy ptr3
    bne nextByte

equal:                      ; ran through without difference
    jmp return0

notEqual:
    bcs greater
    ldx #$FF                ; make result negative
    rts

greater:
    ldx #$01                ; make result positive
    rts

