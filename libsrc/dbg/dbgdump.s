;
; Ullrich von Bassewitz, 11.08.1998
;
; char* __cdecl__ DbgMemDump (unsigend Addr, char* Buf, unsigned char Length);
;

        .export         _DbgMemDump
        .import         addysp1
        .import         __hextab
        .importzp       sp, tmp2, tmp3, tmp4, ptr3, ptr4

_DbgMemDump:
        ldy     #0
        lda     (sp),y          ; Get length
        sta     tmp4
        iny
        lda     (sp),y          ; Get the string buffer
        sta     ptr3
        iny
        lda     (sp),y
        sta     ptr3+1
        iny
        lda     (sp),y          ; Get the address
        sta     ptr4
        iny
        lda     (sp),y
        sta     ptr4+1
        jsr     addysp1         ; Drop the parameters

        lda     #0
        sta     tmp2            ; String index
        sta     tmp3            ; Byte index

; Print the address

        lda     ptr4+1          ; Hi address byte
        jsr     dump            ; Print address
        lda     ptr4            ; Lo address byte
        jsr     dump
        jsr     putspace        ; Add a space

dump1:  dec     tmp4            ; Bytes left?
        bmi     dump9           ; Jump if no
        jsr     putspace        ; Add a space
        ldy     tmp3
        inc     tmp3
        lda     (ptr4),y
        jsr     dump
        jmp     dump1

dump9:  lda     #0
        ldy     tmp2
        sta     (ptr3),y        ; Add string terminator
        lda     ptr3
        ldx     ptr3+1          ; We assume this is not zero
        rts

; Dump one hex byte

dump:   pha
        lsr     a
        lsr     a
        lsr     a
        lsr     a
        tax
        lda     __hextab,x
        jsr     putc
        pla
        and     #$0F
        tax
        lda     __hextab,x
putc:   ldy     tmp2
        inc     tmp2
        sta     (ptr3),y
        rts

putspace:
        lda     #$20
        bne     putc


