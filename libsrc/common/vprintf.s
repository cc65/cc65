;
; Ullrich von Bassewitz, 2003-06-02
;
; int __fastcall__ vprintf (const char* format, va_list ap);
;

        .export         _vprintf
        .import         _vfprintf, _stdout
        .import         decsp2
        .importzp       spc


.proc   _vprintf

; Save A which contains the low part of ap

        pha

; Allocate one more word on the stack

        jsr     decsp2

; Move the format parameter down and store stdout in it's place

        ldy     #2
        lda     (spc),y
        ldy     #0
        sta     (spc),y
        ldy     #3
        lda     (spc),y
        ldy     #1
        sta     (spc),y

        iny
        lda     _stdout
        sta     (spc),y
        iny
        lda     _stdout+1
        sta     (spc),y

; Restore A

        pla

; Call vfprintf (stdout, format, ap) which will cleanup the stack and return

        jmp     _vfprintf

.endproc


