;
; int cscanf(const char* format, ...);
;
; 2000-12-01, Ullrich von Bassewitz
; 2005-01-01, Greg King
;

        .export         _cscanf

        .import         pushax, addysp, _vcscanf

        .macpack        generic
        .include        "zeropage.inc"

; ----------------------------------------------------------------------------
; Code
;
_cscanf:
        sty     ArgSize         ; Number of argument bytes passed in .Y
        dey                     ; subtract size of format pointer
        dey
        tya

; Now, calculate the va_list pointer -- which points to format.

        ldx     sp+1
        add     sp
        bcc     @L1
        inx
@L1:    sta     ptr1
        stx     ptr1+1

; Push a copy of the format pointer onto the stack.

        ldy     #1
        lda     (ptr1),y
        tax
        dey
        lda     (ptr1),y
        jsr     pushax

; Load va_list [last and __fastcall__ argument for vcscanf()].

        lda     ptr1
        ldx     ptr1+1

; Call vcscanf().

        jsr     _vcscanf

; Clean up the stack.  We will return what we got from vcscanf().

        ldy     ArgSize
        jmp     addysp

; ----------------------------------------------------------------------------
; Data
;
        .bss
ArgSize:
        .res    1               ; Number of argument bytes

