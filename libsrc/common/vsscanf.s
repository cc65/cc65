;
; int __fastcall__ vsscanf (const char* str, const char* format, va_list ap);
; /* Standard C function */
;
; 2004-11-28, Ullrich von Bassewitz
; 2014-09-10, Greg King
;

        .export         _vsscanf

        .import         popax, __scanf
        .importzp       c_sp, ptr1, ptr2

        .macpack        generic


; ----------------------------------------------------------------------------
; Structure used to pass data to the callback functions

.struct SSCANFDATA
        STR     .addr
        INDEX   .word
.endstruct


; ----------------------------------------------------------------------------
; static int __fastcall__ get (struct sscanfdata* d)
; /* Read a character from the input string and return it */
; {
;     char C = d->str[d->index];
;     if (C == '\0') {
;         return EOF;
;     }
;     /* Increment index only if end not reached */
;     ++d->index;
;     return C;
; }
;

.code
.proc   get

        sta     ptr1
        stx     ptr1+1                  ; Save d

; Get d->str adding the high byte of index to the pointer, so we can access
; the byte in the string with just the low byte as index

        ldy     #SSCANFDATA::STR
        lda     (ptr1),y
        sta     ptr2
        iny
        lda     (ptr1),y
        ldy     #SSCANFDATA::INDEX+1
        add     (ptr1),y
        sta     ptr2+1

; Load the low byte of the index and fetch the byte from the string

        dey                             ; = SSCANFDATA::INDEX
        lda     (ptr1),y
        tay
        lda     (ptr2),y

; Return EOF if we are at the end of the string

        bne     L1
        lda     #<-1
        tax
        rts

; Bump the index (beware: A contains the char we must return)

L1:     tax                             ; Save return value
        tya                             ; Low byte of index
        ldy     #SSCANFDATA::INDEX
        add     #<$0001
        sta     (ptr1),y
        iny
        lda     (ptr1),y
        adc     #>$0001
        sta     (ptr1),y

; Return the char just read

        txa
        ldx     #>$0000
        rts

.endproc

; ----------------------------------------------------------------------------
; static int __fastcall__ unget (int c, struct sscanfdata* d)
; /* Push back a character onto the input stream */
; {
;     /* We do assume here that the _scanf routine will not push back anything
;     ** not read, so we can ignore c safely and won't check the index.
;     */
;     --d->index;
;     return c;
; }
;

.code
.proc   unget

        sta     ptr1
        stx     ptr1+1                  ; Save d

; Decrement the index

        ldy     #SSCANFDATA::INDEX
        lda     (ptr1),y
        sub     #<$0001
        sta     (ptr1),y
        iny
        lda     (ptr1),y
        sbc     #>$0001
        sta     (ptr1),y

; Return c

        jmp     popax

.endproc

; ----------------------------------------------------------------------------
; int __fastcall__ vsscanf (const char* str, const char* format, va_list ap)
; /* Standard C function */
; {
;     /* Initialize the data structs. The sscanfdata struct will be passed back
;     ** to the get and unget functions by _scanf().
;     */
;     static       struct sscanfdata sd;
;     static const struct  scanfdata  d = {
;         (  getfunc)   get,
;         (ungetfunc) unget,
;         (void*) &sd
;     };
;
;     sd.str   = str;
;     sd.index = 0;
;
;     /* Call the internal function and return the result */
;     return _scanf (&d, format, ap);
; }
;

.bss
sd:     .tag    SSCANFDATA

.rodata
d:      .addr   get
        .addr   unget
        .addr   sd

.code
.proc   _vsscanf

; Save the low byte of ap (which is passed in a/x)

        pha

; Initialize sd and at the same time replace str on the stack by a pointer
; to d

        ldy     #2                      ; Stack offset of str
        lda     (c_sp),y
        sta     sd + SSCANFDATA::STR
        lda     #<d
        sta     (c_sp),y
        iny
        lda     (c_sp),y
        sta     sd + SSCANFDATA::STR+1
        lda     #>d
        sta     (c_sp),y

        lda     #$00
        sta     sd + SSCANFDATA::INDEX
        sta     sd + SSCANFDATA::INDEX+1

; Restore the low byte of ap, and jump to _scanf() which will clean up the stack

        pla
        jmp     __scanf

.endproc

