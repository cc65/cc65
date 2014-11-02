;
; Pointer for library references by device drivers.
;
; Helper-routines for the interrupt handler that rejects bogus keypresses
; that are caused by mouse-like devices.
;
; 2013-07-25, Greg King
; 2014-04-26, Christian Groessler
;

        .include        "c128.inc"

        .export         mouse_libref, _pen_adjuster

        .data

mouse_libref:                   ; generic label for mouse-kernel

; A program optionally can set this pointer to a function that gives
; a calibration value to a driver.  If this pointer isn't NULL,
; then a driver that wants a value can call that function.
;
; The function might read a value from a file; or, it might ask the user
; to help calibrate the driver.
;
; void __fastcall__ (*pen_adjuster)(unsigned char *) = NULL;
;
_pen_adjuster:
        .addr   $0000

        .addr   IRQStub2
callback:                       ; callback into mouse driver after ROM IRQ handler has been run
        .addr   $0000           ; (filled in by mouse driver)
jmp_rom_hdlr:                   ; original ROM indirect IRQ handler address
        .addr   $0000           ; (filled in by mouse driver)


.segment        "LOWCODE"

; Called from irq.s when it thinks it chains to the original handler.
; ROM is banked in again. In order to call the callback we have to
; bank it out one more time.

IRQStub2:

; Call ROM handler and prepare stack so that it will return to us.

        ; setup fake IRQ stack frame which will return to "IRQCont"
        lda     #>@IRQCont
        pha
        lda     #<@IRQCont
        pha
        php

        ; mimic the contents saved on the stack by the ROM IRQ entry handler
        pha                     ; A
        pha                     ; X
        pha                     ; Y
        lda     #MMU_CFG_CC65   ; MMU configuration which will be active after the ROM handler returns
        pha

        ; map out ROM
        ldy     MMU_CR
        sta     MMU_CR

        ; push address of ROM handler on stack and jump to it
        lda     jmp_rom_hdlr+1
        pha
        lda     jmp_rom_hdlr
        pha

        sty     MMU_CR          ; map in ROM
        rts                     ; jump to ROM handler

        ; our MMU configuration byte we pushed on the stack before (MMU_CFG_CC65) is now active

@IRQCont:

        ; call mouse driver callback routine
        lda     #>(@IRQCont2-1)
        pha
        lda     #<(@IRQCont2-1)
        pha
        lda     callback+1
        pha
        lda     callback
        pha
        rts                     ; jump to callback routine

@IRQCont2:

        ; return from interrupt
        ; We could just jump to $FF33, but since I don't know whether this address is valid in all
        ; ROM versions, duplicate that code here.

        pla
        sta     MMU_CR          ; MMU configuration register
        pla
        tay
        pla
        tax
        pla
        rti
