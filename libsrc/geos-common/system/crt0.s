;
; Startup code for GEOS
;
; Maciej 'YTM/Elysium' Witkowiak
; 26.10.99, 10.3.2000, 15.8.2001, 23.12.2002

            .export _exit
            .export __STARTUP__ : absolute = 1          ; Mark as startup
            .import __STACKADDR__, __STACKSIZE__        ; Linker generated
            .import __BACKBUFSIZE__                     ; Linker generated
            .import initlib, donelib
            .import callmain
            .import zerobss
            .importzp sp

            .include "jumptab.inc"
            .include "geossym.inc"
            .include "const.inc"

; ------------------------------------------------------------------------
; Place the startup code in a special segment.

.segment        "STARTUP"

; GEOS 64/128 initializes the screen before starting an application while
; Apple GEOS does not. In order to provide identical startup conditions,
; we initialize the screen here, on Apple GEOS. For the same reason, we set
; the pattern and dispBufferOn, even on GEOS 64/128, although we don't use
; them here.

        lda #2                  ; Checkerboard pattern
        jsr SetPattern
        lda #<(ST_WR_FORE | .MIN (ST_WR_BACK, __BACKBUFSIZE__))
        sta dispBufferOn
.ifdef __GEOS_APPLE__
        jsr i_Rectangle
        .byte 0
        .byte SC_PIX_HEIGHT-1
        .word 0
        .word SC_PIX_WIDTH-1
.endif

; Clear the BSS data.

        jsr zerobss

; Set up the stack.

        lda #<(__STACKADDR__ + __STACKSIZE__)
        ldx #>(__STACKADDR__ + __STACKSIZE__)
        sta sp
        stx sp+1

; Call the module constructors.

        jsr initlib

; Push the command-line arguments; and, call main().

        cli
        jsr callmain

; Call the module destructors.

_exit:  jsr donelib

        jmp EnterDeskTop        ; Return control to the system
