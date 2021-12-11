;
; Ullrich von Bassewitz, 19.03.2001
;
; Stack checking code. These are actually two routines, one to check the C
; stack, and the other one to check the 6502 hardware stack.
; For performance reasons (to avoid having to pass a parameter), the compiler
; calls the cstkchk routine *after* allocating space on the stack. So the
; stackpointer may already be invalid if this routine is called. In addition
; to that, pushs and pops that are needed for expression evaluation are not
; checked (this would be way too much overhead). As a consequence we will
; operate using a safety area at the stack bottom. Once the stack reaches this
; safety area, we consider it an overflow, even if the stack is still inside
; its' bounds.
;

        .export         stkchk, cstkchk
        .constructor    initstkchk, 25
        .import         __STACKSIZE__                   ; Linker defined
        .import         pusha0, _exit
        .importzp       sp

        ; Use macros for better readability
        .macpack        generic
        .macpack        cpu


; ----------------------------------------------------------------------------
; Initialization code. This is a constructor, so it is called on startup if
; the linker has detected references to this module.

.segment        "ONCE"

.proc   initstkchk

        lda     sp
        sta     initialsp
        sub     #<__STACKSIZE__
        sta     lowwater
        lda     sp+1
        sta     initialsp+1
        sbc     #>__STACKSIZE__
.if (.cpu .bitand ::CPU_ISET_65SC02)
        ina                     ; Add 256 bytes safety area
.else
        add     #1              ; Add 256 bytes safety area
.endif
        sta     lowwater+1
        rts

.endproc

; ----------------------------------------------------------------------------
; 6502 stack checking routine. Does not need to save any registers.
; Safety zone for the hardware stack is 12 bytes.

.code

stkchk: tsx
        cpx     #12
        bcc     Fail            ; Jump on stack overflow
        rts                     ; Return if ok

; ----------------------------------------------------------------------------
; C stack checking routine. Does not need to save any registers.

.code

cstkchk:

; Check the high byte of the software stack

@L0:    lda     lowwater+1
        cmp     sp+1
        bcs     @L1
        rts

; Check low byte

@L1:    bne     CStackOverflow
        lda     lowwater
        cmp     sp
        bcs     CStackOverflow
Done:   rts

; We have a C stack overflow. Set the stack pointer to the initial value, so
; we can continue without worrying about stack issues.

CStackOverflow:
        lda     initialsp
        sta     sp
        lda     initialsp+1
        sta     sp+1

; Generic abort entry. We should output a diagnostic here, but this is
; difficult, since we're operating at a lower level here.

Fail:   lda     #4
        ldx     #0
        jmp     _exit

; ----------------------------------------------------------------------------
; Data

.segment        "INIT"

; Initial stack pointer value. Stack is reset to this in case of overflows to
; allow program exit processing.
initialsp:      .res    2

; Stack low water mark.
lowwater:       .res    2



