;
; Startup code for cc65 (Plus/4 version)
;

        .export         _exit
        .export         brk_jmp
        .export         __STARTUP__ : absolute = 1      ; Mark as startup

        .import         callirq_y, initlib, donelib
        .import         callmain, zerobss
        .import         __INTERRUPTOR_COUNT__
        .import         __RAM_START__, __RAM_SIZE__     ; Linker generated
        .import         __STACKSIZE__                   ; Linker generated
        .importzp       ST

        .include        "zeropage.inc"
        .include        "plus4.inc"


; ------------------------------------------------------------------------
; Constants

IRQInd          = $500  ; JMP $0000 - used as indirect IRQ vector

; ------------------------------------------------------------------------
; Startup code

.segment        "STARTUP"

Start:

; Save the zero-page locations that we need.

        sei                     ; No interrupts since we're banking out the ROM
        sta     ENABLE_RAM
        ldx     #zpspace-1
L1:     lda     sp,x
        sta     zpsave,x
        dex
        bpl     L1
        sta     ENABLE_ROM
        cli

; Switch to the second charset.

        lda     #14
        jsr     $FFD2           ; BSOUT

; Save some system stuff; and, set up the stack. The stack starts at the top
; of the usable RAM.

        tsx
        stx     spsave          ; save system stk ptr

        lda     #<(__RAM_START__ + __RAM_SIZE__ + __STACKSIZE__)
        sta     sp
        lda     #>(__RAM_START__ + __RAM_SIZE__ + __STACKSIZE__)
        sta     sp+1

; Set up the IRQ vector in the banked RAM; and, switch off the ROM.

        ldx     #<IRQ
        ldy     #>IRQ
        sei                     ; No ints, handler not yet in place
        sta     ENABLE_RAM
        stx     $FFFE           ; Install interrupt handler
        sty     $FFFF
        cli                     ; Allow interrupts

; Clear the BSS data.

        jsr     zerobss

; Initialize irqcount, which means that, from now on, custom linked-in IRQ
; handlers will be called (via condes).

        lda     #.lobyte(__INTERRUPTOR_COUNT__*2)
        sta     irqcount

; Call the module constructors.

        jsr     initlib

; Push the command-line arguments; and, call main().

        jsr     callmain

; Back from main() [this is also the exit() entry]. Run the module destructors.

_exit:  pha                     ; Save the return code
        jsr     donelib         ; Run module destructors

; Disable the chained IRQ handlers.

        lda     #0
        sta     irqcount        ; Disable custom IRQ handlers

; Copy back the zero-page stuff.

        ldx     #zpspace-1
L2:     lda     zpsave,x
        sta     sp,x
        dex
        bpl     L2

; Place the program return code into BASIC's status variable.

        pla
        sta     ST

; Restore the stack pointer.

        ldx     spsave
        txs

; Enable the ROM; and, return to BASIC.

        sta     ENABLE_ROM
        rts

; ------------------------------------------------------------------------
; IRQ handler. The handler in the ROM enables the Kernal, and jumps to
; $CE00, where the ROM code checks for a BRK or IRQ, and branches via the
; indirect vectors at $314/$316.
; To make our stub as fast as possible, we skip the whole part of the ROM
; handler, and jump to the indirect vectors directly. We do also call our
; own interrupt handlers if we have any; so, they need not use $314.

.segment        "LOWCODE"

IRQ:    cld                     ; Just to be sure
        pha
        txa
        pha
        tya
        pha
        tsx                     ; Get the stack pointer
        lda     $0104,x         ; Get the saved status register
        and     #$10            ; Test for BRK bit
        bne     dobreak

; It's an IRQ; and, RAM is enabled. If we have handlers, call them. We will use
; a flag here instead of loading __INTERRUPTOR_COUNT__ directly, since the
; condes function is not reentrant. The irqcount flag will be set/reset from
; the main code, to avoid races.

        ldy     irqcount
        beq     @L1
        jsr     callirq_y       ; Call the IRQ functions

; Since the ROM handler will end with an RTI, we have to fake an IRQ return
; on the stack, so that we get control of the CPU after the ROM handler,
; and can switch back to RAM.

@L1:    lda     #>irq_ret       ; Push new return address
        pha
        lda     #<irq_ret
        pha
        php                     ; Push faked IRQ frame on stack
        pha                     ; Push faked A register
        pha                     ; Push faked X register
        pha                     ; Push faked Y register
        sta     ENABLE_ROM      ; Switch to ROM
        jmp     (IRQVec)        ; Jump indirect to Kernal IRQ handler

irq_ret:
        sta     ENABLE_RAM      ; Switch back to RAM
        pla
        tay
        pla
        tax
        pla
        rti

dobreak:
        lda     brk_jmp+2       ; Check high byte of address
        beq     nohandler
        jmp     brk_jmp         ; Jump to the handler

; No break handler installed, jump to ROM.

nohandler:
        sta     ENABLE_ROM
        jmp     (BRKVec)        ; Jump indirect to the break vector

; ------------------------------------------------------------------------
; Data

.data

; BRK handling
brk_jmp:        jmp     $0000

spsave:         .res    1

irqcount:       .byte   0

.segment        "INITBSS"

zpsave:         .res    zpspace


