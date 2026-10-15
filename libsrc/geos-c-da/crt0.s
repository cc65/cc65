;
; Startup code for a GEOS desk accessory (DESK_ACC), based on
; libsrc/geos-common/system/crt0.s
;

            .export _exit
            .export __STARTUP__ : absolute = 1          ; Mark as startup
            .export __GEOS_SAVED_ZP__
            .export _RstrAppl
            .import __STACKADDR__, __STACKSIZE__        ; Linker generated
            .import __ZP_START__, __ZP_SIZE__           ; Linker generated
            .import initlib, donelib
            .import callmain
            .import zerobss
            .importzp c_sp

            .include "jumptab.inc"

; ------------------------------------------------------------------------
; Somewhere to save cc65's own runtime zero page block while this
; accessory is running.

.segment        "SAVEDZP"

__GEOS_SAVED_ZP__:

; ------------------------------------------------------------------------
; Place the startup code in a special segment.

.segment        "STARTUP"

; Save zero page before anything below gets a chance to use it.

        ldy #<(__ZP_SIZE__ - 1)
SaveZP: lda __ZP_START__,y
        sta __GEOS_SAVED_ZP__,y
        dey
        bpl SaveZP

; Clear the BSS data.

        jsr zerobss

; Set up the stack.

        lda #<(__STACKADDR__ + __STACKSIZE__)
        ldx #>(__STACKADDR__ + __STACKSIZE__)
        sta c_sp
        stx c_sp+1

; Call the module constructors.

        jsr initlib

; Push the command-line arguments; and, call main().

        jsr callmain

; Call the module destructors, then leave this DA and resume the
; calling application, restoring cc65's own runtime zero page block
; first. Whether main() just returned or RstrAppl() was called
; explicitly, both need to happen the same way, so _exit and _RstrAppl
; are the same code.

_exit:
_RstrAppl:
        jsr donelib

        ldy #<(__ZP_SIZE__ - 1)
RestoreZP:
        lda __GEOS_SAVED_ZP__,y
        sta __ZP_START__,y
        dey
        bpl RestoreZP

        jmp RstrAppl
