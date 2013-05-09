;
; Ullrich von Bassewitz, 08.08.1998
;
; Support routines for the debugger
;

        .export         _DbgInit
        .export         _DbgSP, _DbgCS, _DbgHI
        .import         popax, return0, _DbgEntry, _set_brk, _end_brk
        .import         _DbgBreaks
        .import         _brk_pc
        .import         __ZP_START__            ; Linker generated

        .include        "zeropage.inc"


; C callable function, will install the debugger

_DbgInit:
        lda     #<DbgBreak
        ldx     #>DbgBreak
        jmp     _set_brk


; Entry for the break vector.

DbgBreak:
        pla
        sta     retsav
        pla
        sta     retsav+1

        cli
        tsx                     ; Stack pointer
        stx     _DbgSP

        jsr     DbgSwapZP       ; Swap stuff
        lda     #<DbgStack      ; Set new stack
        sta     sp
        lda     #>DbgStack
        sta     sp+1
        jsr     ResetDbgBreaks  ; Reset temporary breakpoints
        jsr     _DbgEntry       ; Call C code
        jsr     SetDbgBreaks    ; Set temporary breakpoints
        jsr     DbgSwapZP       ; Swap stuff back

        lda     retsav+1
        pha
        lda     retsav
        pha
        rts



; Stack used when in debugger mode

.bss
        .res    256
DbgStack:

; Swap space for the the C temporaries

CTemp:
_DbgCS: .res    2               ; sp
_DbgHI: .res    2               ; sreg
        .res    (zpsavespace-4) ; Other stuff

_DbgSP: .res    1
retsav: .res    2               ; Save buffer for return address

.code

; Swap the C temporaries

DbgSwapZP:
        ldy     #zpsavespace-1
Swap1:  ldx     CTemp,y
        lda     <__ZP_START__,y
        sta     CTemp,y
        txa
        sta     sp,y
        dey
        bpl     Swap1
        rts

; ----------------------------------------------------------------------------
; Utility functions


; Set/reset the breakpoints. We must do that here since the breakpoints
; may be in the runtime stuff, causing the C part to fail before it has
; reset the breakpoints. See declaration of struct breakpoint in the C
; source

MaxBreaks       = 48            ; 4*12

ResetDbgBreaks:
        ldy     #0
        ldx     #0
L4:     lda     _DbgBreaks+3,x  ; Get bk_use
        beq     L6              ; Jump if not set
        bpl     L5              ; Jump if user breakpoint
        lda     #0
        sta     _DbgBreaks+3,x  ; Clear if temp breakpoint
L5:     lda     _DbgBreaks+1,x  ; PC hi
        sta     ptr1+1
        lda     _DbgBreaks,x    ; PC lo
        sta     ptr1
        lda     _DbgBreaks+2,x  ; Old OPC
        sta     (ptr1),y        ; Reset the breakpoint
L6:     inx
        inx
        inx
        inx
        cpx     #MaxBreaks      ; Done?
        bne     L4
        rts

SetDbgBreaks:
        ldx     #0
        ldy     #0
L7:     lda     _DbgBreaks+3,x  ; Get bk_use
        beq     L8              ; Jump if not set
        lda     _DbgBreaks+1,x  ; PC hi
        sta     ptr1+1
        lda     _DbgBreaks,x    ; PC lo
        sta     ptr1
        lda     (ptr1),y        ; Get the breakpoint OPC...
        sta     _DbgBreaks+2,x  ; ...and save it
        lda     #$00            ; Load BRK opcode
        sta     (ptr1),y
L8:     inx
        inx
        inx
        inx
        cpx     #MaxBreaks      ; Done?
        bne     L7
        rts

; Get a free breakpoint slot or return 0

        .export         _DbgGetBreakSlot

_DbgGetBreakSlot:
        ldx     #0
L10:    lda     _DbgBreaks+3,x  ; Get bk_use
        beq     L11             ; Jump if not set
        inx
        inx
        inx
        inx
        cpx     #MaxBreaks      ; Done?
        bne     L10
        jmp     return0         ; No free slot

L11:    stx     tmp1
        lda     #<_DbgBreaks
        ldx     #>_DbgBreaks
        clc
        adc     tmp1
        bcc     L12
        inx
L12:    ldy     #1              ; Force != 0
        rts


; Check if a given address has a user breakpoint set, if found, return the
; slot, otherwise return 0.

        .export         _DbgIsBreak

_DbgIsBreak:
        jsr     popax           ; Get address
        sta     ptr1
        stx     ptr1+1
        ldx     #0
L20:    lda     _DbgBreaks+3,x  ; Get bk_use
        beq     L21             ; Jump if not set
        bmi     L21             ; Jump if temp breakpoint
        lda     _DbgBreaks,x    ; Low byte of address
        cmp     ptr1
        bne     L21
        lda     _DbgBreaks+1,x  ; High byte of address
        cmp     ptr1+1
        beq     L22
L21:    inx
        inx
        inx
        inx
        cpx     #MaxBreaks      ; Done?
        bne     L20
        jmp     return0         ; Not found

L22:    stx     tmp1
        lda     #<_DbgBreaks
        ldx     #>_DbgBreaks
        clc
        adc     tmp1
        bcc     L23
        inx
L23:    ldy     #1              ; Force != 0
        rts



