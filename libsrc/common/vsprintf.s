;
; int vsprintf (char* Buf, const char* Format, va_list ap);
;
; Ullrich von Bassewitz, 2009-09-26
;

   	.export	      	_vsprintf
	.import	      	ldax0sp, pushax, staxysp
	.import	      	_vsnprintf


; ----------------------------------------------------------------------------
; vsprintf - formatted output into a buffer
;
; int vsprintf (char* buf, const char* format, va_list ap);
;


_vsprintf:
	pha	  		; Save low byte of ap
        txa
        pha                     ; Save high byte of op

; Build a stackframe for vsnprintf. To do that, we move format one word down,
; and store 0x7FF (INT_MAX) as size.

        jsr     ldax0sp         ; Get format
        jsr     pushax          ; And push it
        lda     #$7F
        ldx     #$FF            ; INT_MAX
        ldy     #2
        jsr     staxysp

; Retrieve ap and contine by jumping to _vsnprintf, which will cleanup the stack

        pla
        tax
        pla
        jmp     _vsnprintf

