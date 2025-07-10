;
; Ullrich von Bassewitz, 2003-06-02
;
; int __fastcall__ vprintf (const char* format, va_list ap);
;

        .export         _vprintf
        .import         _vfprintf, _stdout
        .import         decsp2
        .importzp       c_sp


.proc   _vprintf

; Save A which contains the low part of ap

        pha

; Allocate one more word on the stack

        jsr     decsp2

; Move the format parameter down and store stdout in it's place

        ldy     #2
        lda     (c_sp),y
        ldy     #0
        sta     (c_sp),y
        ldy     #3
        lda     (c_sp),y
        ldy     #1
        sta     (c_sp),y

        iny
        lda     _stdout
        sta     (c_sp),y
        iny
        lda     _stdout+1
        sta     (c_sp),y

; Restore A

        pla

; Call vfprintf (stdout, format, ap) which will cleanup the stack and return

        jmp     _vfprintf

.endproc


