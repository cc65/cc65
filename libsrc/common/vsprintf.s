;
; int __fastcall__ vsprintf (char* Buf, const char* Format, va_list ap);
;
; Ullrich von Bassewitz, 2009-09-26
;

   	.export	      	_vsprintf
	.import	      	pushw0sp, staxysp
	.import	      	_vsnprintf


; ----------------------------------------------------------------------------
; vsprintf - formatted output into a buffer
;
; int __fastcall__ vsprintf (char* buf, const char* format, va_list ap);
;


_vsprintf:
	pha	   		; Save low byte of ap
        txa
        pha                     ; Save high byte of op

; Build a stackframe for vsnprintf. To do that, we move format one word down,
; and store 0x7FFF (INT_MAX) as size.

        jsr     pushw0sp        ; Get format and push it
        lda     #$FF
        ldx     #$7F            ; INT_MAX
        ldy     #2
        jsr     staxysp

; Retrieve ap and contine by jumping to _vsnprintf, which will cleanup the stack

        pla
        tax
        pla
        jmp     _vsnprintf

