;
; int cprintf (const char* Format, ...);
;
; Ullrich von Bassewitz, 1.12.2000
;

        .export         _cprintf
        .import         pushax, addysp, _vcprintf
        .importzp       sp, ptr1

        .macpack        generic

; ----------------------------------------------------------------------------
; Data

.bss

ParamSize:      .res    1               ; Number of parameter bytes

; ----------------------------------------------------------------------------
; Code

.code


_cprintf:
        sty     ParamSize               ; Number of param bytes passed in Y

; Calculate a pointer that points to Format

        dey
        dey                             ; Sub size of Format
        tya
        add     sp
        sta     ptr1
        ldx     sp+1
        bcc     @L1
        inx
@L1:    stx     ptr1+1

; Push Format

        ldy     #1
        lda     (ptr1),y
        tax
        dey
        lda     (ptr1),y
        jsr     pushax

; Load va_list (last and __fastcall__ parameter to vfprintf)

        lda     ptr1
        ldx     ptr1+1

; Call vcprintf

        jsr     _vcprintf

; Cleanup the stack. We will return what we got from vcprintf

        ldy     ParamSize
        jmp     addysp


