;
; Maciej 'YTM/Alliance' Witkowiak
;
; 21.12.99

; void BitOtherClip     (void *proc1, void* proc2, char skipl, char skipr, int skipy,
;                               struct iconpic *myGfx);

; both proc1, proc2 should be: char foo (void);
;       proc1 is called before reading a byte (.A returns next data)
;       proc2 is called before reading each byte which is not pattern (code >219)


            .import popa, popax
            .import BitOtherRegs
            .export _BitOtherClip

            .include "jumptab.inc"
            .include "geossym.inc"

_BitOtherClip:
        jsr BitOtherRegs
        jsr popax
        sta r12L
        stx r12H
        jsr popa
        sta r11H
        jsr popa
        sta r11L
        jsr popax
        sta r14L
        stx r14H
        jsr popax
        sta r13L
        stx r13H
        jmp BitOtherClip
